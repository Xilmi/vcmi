/*
 * LuaScriptingContext.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "LuaContext.h"

#include "LuaStack.h"
#include "LuaReference.h"

#include "api/Enums.h"

#include "../lib/callback/IGameInfoCallback.h"
#include "../lib/json/JsonNode.h"
#include "../lib/filesystem/Filesystem.h"
#include "../lib/modding/ModScope.h"

#include <vstd/StringUtils.h>
#include <vcmi/ServerCallback.h>
#include <vcmi/Services.h>

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting
{

int LuaContext::luaError(lua_State * L)
{
	int level = luaL_optinteger(L, 2, 1);

	if(level > 0 && lua_isstring(L, 1))
	{
		luaL_where(L, level);
		lua_pushvalue(L, 1);
		lua_concat(L, 2);
		lua_replace(L, 1);
	}

	const char * msg = lua_tostring(L, 1);
	if(msg)
		logScript->warn("%s", msg);

	lua_settop(L, 1);
	return lua_error(L);
}

int LuaContext::luaAssert(lua_State * L)
{
	if(lua_toboolean(L, 1))
		return lua_gettop(L);

	luaL_where(L, 1);
	lua_pushstring(L, luaL_optstring(L, 2, "assertion failed!"));
	lua_concat(L, 2);

	const char * msg = lua_tostring(L, -1);
	if(msg)
		logScript->warn("%s", msg);

	return lua_error(L);
}

int LuaContext::luaPrint(lua_State *L) {
	int n = lua_gettop(L);
	lua_getglobal(L, "tostring");
	std::string out;
	for (int i = 1; i <= n; ++i)
	{
		lua_pushvalue(L, -1);
		lua_pushvalue(L, i);
		lua_call(L, 1, 1);
		const char *s = lua_tostring(L, -1);
		if (s)
			out += s;
		if (i > 1)
			out += '\t';

		lua_pop(L, 1);
	}

	logScript->info("%s", out);
	return 0;
}

LuaContext::LuaContext(const Script * source, const Environment * env_):
	L(luaL_newstate()),
	script(source),
	env(env_)
{
	static constexpr std::array<luaL_Reg, 4> STD_LIBS =
	{{
		{"_G", luaopen_base},
		{LUA_TABLIBNAME, luaopen_table},
		{LUA_STRLIBNAME, luaopen_string},
		{LUA_MATHLIBNAME, luaopen_math}
	}};

	for(const luaL_Reg & lib : STD_LIBS)
	{
		lib.func(L);
		lua_setglobal(L, lib.name);
	}

	lua_settop(L, 0);

	cleanupGlobals();

	lua_settop(L, 0);

	lua_newtable(L);
	modules = std::make_shared<LuaReference>(L);
	lua_settop(L, 0);

	registerPublicTypes();

	lua_settop(L, 0);

	LuaStack S(L);
	api::Enums enums;

	S.push(env->game());
	lua_setglobal(L, "GAME");

	S.push(env->services());
	lua_setglobal(L, "LIBRARY");

	S.push(enums);
	lua_setglobal(L, "ENUM");

	lua_settop(L, 0);
}

LuaContext::~LuaContext()
{
	modules.reset();
	scriptClosure.reset();
	scriptTable.reset();
	lua_close(L);
}

void LuaContext::cleanupGlobals()
{
	LuaStack S(L);
	S.clear();
	S.pushNil();
	lua_setglobal(L, "collectgarbage");

	S.pushNil();
	lua_setglobal(L, "dofile");

	S.pushNil();
	lua_setglobal(L, "load");

	S.pushNil();
	lua_setglobal(L, "loadfile");

	S.pushNil();
	lua_setglobal(L, "loadstring");

	lua_pushcfunction(L, luaPrint);
	lua_setglobal(L, "print");

	lua_pushcfunction(L, luaError);
	lua_setglobal(L, "error");

	lua_pushcfunction(L, luaAssert);
	lua_setglobal(L, "assert");

	S.clear();

	lua_getglobal(L, LUA_STRLIBNAME);

	S.push("dump");
	S.pushNil();
	lua_rawset(L, -3);
	S.clear();

	lua_getglobal(L, LUA_MATHLIBNAME);

	S.push("random");
	S.pushNil();
	lua_rawset(L, -3);

	S.push("randomseed");
	S.pushNil();
	lua_rawset(L, -3);
	S.clear();
}

bool LuaContext::hasFunction(const std::string & name)
{
	std::lock_guard guard(mutex);
	if(!scriptTable)
		return false;
	LuaStack S(L);
	scriptTable->push();
	lua_getfield(L, -1, name.c_str());
	bool result = S.isFunction(-1);
	S.clear();
	return result;
}

void LuaContext::initialize()
{
	std::lock_guard guard(mutex);
	int ret = luaL_loadbuffer(L, script->getSource().c_str(), script->getSource().size(), script->getIdentifier().c_str());

	if(ret)
	{
		logScript->error("Script '%s' failed to compile: %s", script->getIdentifier(), toStringRaw(-1));
		lua_settop(L, 0);
		return;
	}

	scriptClosure = std::make_shared<LuaReference>(L);
	lua_settop(L, 0);
	scriptClosure->push();

	ret = lua_pcall(L, 0, 1, 0);

	if(ret)
	{
		logScript->error("Script '%s' failed to run: %s", script->getIdentifier(), toStringRaw(-1));
		lua_settop(L, 0);
		return;
	}

	if(!lua_istable(L, -1))
	{
		logScript->error("Script '%s' did not return a table", script->getIdentifier());
		lua_settop(L, 0);
		return;
	}

	scriptTable = std::make_shared<LuaReference>(L);
}

int LuaContext::errorRetVoid(const std::string & message)
{
	logScript->error(message);
	lua_settop(L, 0);
	return 0;
}

std::string LuaContext::toStringRaw(int index)
{
	size_t len = 0;
	const auto *raw = lua_tolstring(L, index, &len);
	return std::string(raw, len);
}

void LuaContext::registerPublicTypes()
{
	lua_pushlightuserdata(L, this);
	lua_pushcclosure(L, &LuaContext::require, 1);
	lua_setglobal(L, "require");

	lua_settop(L, 0);

	LuaStack S(L);

	for(const auto & registar : api::Registry::get()->getAllTypes())
	{
		registar.second->pushMetatable(L); //table

		modules->push(); //table modules
		S.push(registar.first); //table modules name
		lua_pushvalue(L, -3); //table modules name table
		lua_rawset(L, -3);

		lua_settop(L, 0);
	}
}

int LuaContext::require(lua_State * L)
{
	auto * self = static_cast<LuaContext *>(lua_touserdata(L, lua_upvalueindex(1)));

	if(!self)
	{
		lua_pushstring(L, "internal error");
		return lua_error(L);
	}

	int result = self->loadModule();
	if(result < 0)
		return lua_error(L); // error string was pushed by loadModule; its locals are already destroyed
	return result;
}

int LuaContext::loadModule()
{
	int argc = lua_gettop(L);

	if(argc != 1)
	{
		lua_pushstring(L, "require: module name expected");
		return -1;
	}

	if(!lua_isstring(L, 1))
	{
		lua_pushstring(L, "require: module name must be a string");
		return -1;
	}

	std::string resourceName = toStringRaw(1);

	if(resourceName.empty())
	{
		lua_pushstring(L, "require: module name is empty");
		return -1;
	}

	auto temp = vstd::split(resourceName, ":");

	std::string scope;
	std::string modulePath;

	if(temp.size() <= 1)
	{
		modulePath = temp.at(0);
	}
	else
	{
		scope = temp.at(0);
		modulePath = temp.at(1);
	}

	auto * loader = scope.empty() ? CResourceHandler::get() : CResourceHandler::get(scope);

	ScriptPath id = ScriptPath::builtinTODO(modulePath).addPrefix("SCRIPTS/");

	if(!loader->existsResource(id))
	{
		lua_pushfstring(L, "require: module not found: %s", modulePath.c_str());
		return -1;
	}

	auto rawData = loader->load(id)->readAll();
	auto sourceText = std::string(reinterpret_cast<char *>(rawData.first.get()), rawData.second);
	int ret = luaL_loadbuffer(L, sourceText.c_str(), sourceText.size(), modulePath.c_str());

	if(ret)
		return -1; // error string already on stack from luaL_loadbuffer

	ret = lua_pcall(L, 0, 1, 0);

	if(ret)
		return -1; // error string already on stack from lua_pcall

	if(!lua_istable(L, -1))
	{
		lua_pushfstring(L, "require: module '%s' did not return a table", modulePath.c_str());
		return -1;
	}

	return 1;
}

}

VCMI_LIB_NAMESPACE_END
