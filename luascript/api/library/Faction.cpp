/*
 * api/Faction.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Faction.h"

#include "../Registry.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::library
{

const std::vector<FactionProxy::CustomRegType> FactionProxy::REGISTER_CUSTOM =
{
	{"getIconIndex", LuaMethodWrapper<&Entity::getIconIndex, Faction>::invoke,      false},
	{"getIndex",     LuaMethodWrapper<&Entity::getIndex, Faction>::invoke,          false},
	{"getJsonKey",   LuaMethodWrapper<&Entity::getJsonKey, Faction>::invoke,        false},
	{"getName",      LuaMethodWrapper<&Entity::getNameTranslated, Faction>::invoke, false},
	{"hasTown",      LuaMethodWrapper<&Faction::hasTown>::invoke,                   false},
};

}

VCMI_LIB_NAMESPACE_END
