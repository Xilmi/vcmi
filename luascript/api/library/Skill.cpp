/*
 * api/Skill.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Skill.h"

#include "../Registry.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::library
{

const std::vector<SkillProxy::CustomRegType> SkillProxy::REGISTER_CUSTOM =
{
	{"getIconIndex", LuaMethodWrapper<&Entity::getIconIndex, Skill>::invoke,      false},
	{"getIndex",     LuaMethodWrapper<&Entity::getIndex, Skill>::invoke,          false},
	{"getJsonKey",   LuaMethodWrapper<&Entity::getJsonKey, Skill>::invoke,        false},
	{"getName",      LuaMethodWrapper<&Entity::getNameTranslated, Skill>::invoke, false},
};

}

VCMI_LIB_NAMESPACE_END
