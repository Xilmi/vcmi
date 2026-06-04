/*
 * BattleHexArray.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"

#include "BattleHexArray.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::battle
{

const std::vector<BattleHexArrayProxy::CustomRegType> BattleHexArrayProxy::REGISTER_CUSTOM =
{
	{"insert",   LuaCallWrapper<&BattleHexArrayProxy::insert>::invoke,                false},
	{"erase",    LuaCallWrapper<&BattleHexArrayProxy::erase>::invoke,                 false},
	{"contains", LuaMethodWrapper<&BattleHexArray::contains>::invoke,                 false},
	{"size",     LuaMethodWrapper<&BattleHexArray::size>::invoke,                     false},
	{"at",       LuaFunctionWrapper<&BattleHexArrayProxy::at>::invoke,                false},
};

int BattleHexArrayProxy::insert(lua_State * L)
{
	LuaStack S(L);
	void * raw = luaL_checkudata(L, 1, api::Registry::get()->getTypeName<BattleHexArray>());
	BattleHex hex;
	S.get(2, hex);
	S.clear();
	if(raw)
		static_cast<BattleHexArray *>(raw)->insert(hex);
	return 0;
}

int BattleHexArrayProxy::erase(lua_State * L)
{
	LuaStack S(L);
	void * raw = luaL_checkudata(L, 1, api::Registry::get()->getTypeName<BattleHexArray>());
	BattleHex hex;
	S.get(2, hex);
	S.clear();
	if(raw)
	{
		auto * hexes = static_cast<BattleHexArray *>(raw);
		if(hexes->contains(hex))
			hexes->erase(hex);
	}
	return 0;
}

BattleHex BattleHexArrayProxy::at(const BattleHexArray & hexes, int index)
{
	return hexes.at(index - 1);
}

}

VCMI_LIB_NAMESPACE_END
