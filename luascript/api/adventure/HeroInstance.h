/*
 * HeroInstance.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include <vcmi/HeroClass.h>

#include "../../LuaWrapper.h"

#include "../../../lib/mapObjects/CGHeroInstance.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

class HeroInstanceProxy : public RawPointerWrapper<const CGHeroInstance, HeroInstanceProxy>
{
	static bool isMale(const CGHeroInstance * hero);
	static bool isFemale(const CGHeroInstance * hero);

public:
	using Wrapper = RawPointerWrapper<const CGHeroInstance, HeroInstanceProxy>;
	static const std::vector<typename Wrapper::CustomRegType> REGISTER_CUSTOM;

	static int getBonuses(lua_State * L);
};


}

VCMI_LIB_NAMESPACE_END
