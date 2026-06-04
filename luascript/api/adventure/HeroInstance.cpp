/*
 * HeroInstance.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "HeroInstance.h"

#include "../Registry.h"
#include "../library/Bonus.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"

#include "../../../lib/bonuses/BonusSelector.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{
const std::vector<HeroInstanceProxy::CustomRegType> HeroInstanceProxy::REGISTER_CUSTOM =
{
	{"getStack",      LuaMethodWrapper<&CCreatureSet::getStackPtr, CGHeroInstance>::invoke, false},
	{"getOwner",      LuaMethodWrapper<&CGObjectInstance::getOwner, CGHeroInstance>::invoke, false},
	{"getNameTextID", LuaMethodWrapper<&CGHeroInstance::getNameTextID>::invoke, false},
	{"isMale",        LuaFunctionWrapper<&HeroInstanceProxy::isMale>::invoke,   false},
	{"isFemale",      LuaFunctionWrapper<&HeroInstanceProxy::isFemale>::invoke, false},
	{"getBonuses",    LuaCallWrapper<&HeroInstanceProxy::getBonuses>::invoke,    false},
};

bool HeroInstanceProxy::isMale(const CGHeroInstance * hero)
{
	return hero->gender == EHeroGender::MALE;
}

bool HeroInstanceProxy::isFemale(const CGHeroInstance * hero)
{
	return hero->gender == EHeroGender::FEMALE;
}

int HeroInstanceProxy::getBonuses(lua_State * L)
{
	LuaStack S(L);
	const CGHeroInstance * hero = nullptr;
	S.get(1, hero);

	if(!hero || !lua_isfunction(L, 2))
	{
		S.clear();
		return 0;
	}

	auto allBonuses = hero->getAllBonuses(Selector::all);

	BonusList result;
	for(const auto & bonus : *allBonuses)
	{
		lua_pushvalue(L, 2);
		S.push(*bonus);
		lua_call(L, 1, 1);
		bool keep = lua_toboolean(L, -1);
		lua_pop(L, 1);
		if(keep)
			result.push_back(bonus);
	}

	S.clear();
	S.push(result);
	return 1;
}

}

VCMI_LIB_NAMESPACE_END
