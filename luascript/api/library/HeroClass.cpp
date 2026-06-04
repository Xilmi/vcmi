/*
 * api/HeroClass.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "HeroClass.h"

#include "../Registry.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::library
{

const std::vector<HeroClassProxy::CustomRegType> HeroClassProxy::REGISTER_CUSTOM =
{
	{"getIconIndex", LuaMethodWrapper<&Entity::getIconIndex, HeroClass>::invoke,      false},
	{"getIndex",     LuaMethodWrapper<&Entity::getIndex, HeroClass>::invoke,          false},
	{"getJsonKey",   LuaMethodWrapper<&Entity::getJsonKey, HeroClass>::invoke,        false},
	{"getName",      LuaMethodWrapper<&Entity::getNameTranslated, HeroClass>::invoke, false},
};

}

VCMI_LIB_NAMESPACE_END
