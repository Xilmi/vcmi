/*
 * Unit.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"

#include "Unit.h"

#include "../../../lib/GameLibrary.h"
#include "../../../lib/battle/CUnitState.h"
#include "../../../lib/bonuses/BonusSelector.h"
#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"
#include "../Registry.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::battle
{

const std::vector<UnitProxy::CustomRegType> UnitProxy::REGISTER_CUSTOM =
{
	{"getMinDamage",        LuaMethodWrapper<&ACreature::getMinDamage, Unit>::invoke,  false},
	{"getMaxDamage",        LuaMethodWrapper<&ACreature::getMaxDamage, Unit>::invoke,  false},
	{"getAttack",           LuaMethodWrapper<&ACreature::getAttack, Unit>::invoke,     false},
	{"getDefense",          LuaMethodWrapper<&ACreature::getDefense, Unit>::invoke,    false},
	{"isAlive",             LuaMethodWrapper<&Unit::alive>::invoke,                    false},
	{"isClone",             LuaMethodWrapper<&Unit::isClone>::invoke,                  false},
	{"hasClone",            LuaMethodWrapper<&Unit::hasClone>::invoke,                 false},
	{"isDead",              LuaMethodWrapper<&Unit::isDead>::invoke,                   false},
	{"isGhost",             LuaMethodWrapper<&Unit::isGhost>::invoke,                  false},
	{"isValidTarget",       LuaMethodWrapper<&Unit::isValidTarget>::invoke,            false},
	{"isInvincible",        LuaMethodWrapper<&Unit::isInvincible>::invoke,             false},
	{"hasAbsoluteImmunity", LuaFunctionWrapper<&UnitProxy::hasAbsoluteImmunity>::invoke, false},
	{"isSummoned",          LuaMethodWrapper<&Unit::isSummoned>::invoke,               false},
	{"getOwner",            LuaMethodWrapper<&IUnitInfo::unitOwner, Unit>::invoke,     false},
	{"getSlot",             LuaMethodWrapper<&IUnitInfo::unitSlot, Unit>::invoke,      false},
	{"unitSide",            LuaMethodWrapper<&IUnitInfo::unitSide, Unit>::invoke,      false},
	{"getPosition",         LuaMethodWrapper<&Unit::getPosition>::invoke,              false},
	{"getTotalHealth",      LuaMethodWrapper<&Unit::getTotalHealth>::invoke,           false},
	{"getAvailableHealth",  LuaMethodWrapper<&Unit::getAvailableHealth>::invoke,       false},
	{"getCount",            LuaMethodWrapper<&Unit::getCount>::invoke,                 false},
	{"getMaxHealth",        LuaMethodWrapper<&ACreature::getMaxHealth, Unit>::invoke,  false},
	{"coversPos",           LuaMethodWrapper<&Unit::coversPos>::invoke,                false},

	{"getCreature",    LuaFunctionWrapper<&UnitProxy::getCreature>::invoke,  false},
	{"getBaseAmount",  LuaMethodWrapper<&Unit::unitBaseAmount, Unit>::invoke, false},
	{"getHexes",       LuaFunctionWrapper<&UnitProxy::getHexes>::invoke,     false},
	{"copy",           LuaFunctionWrapper<&UnitProxy::copy>::invoke,         false},
	{"getBonuses",     LuaCallWrapper<&UnitProxy::getBonuses>::invoke,       false},
	{"creatureLevel",  LuaMethodWrapper<&Unit::creatureLevel>::invoke,       false},
	{"unitId",         LuaMethodWrapper<&IUnitInfo::unitId, Unit>::invoke,   false},
};

const Creature * UnitProxy::getCreature(const Unit * unit)
{
	return unit->creatureId().toEntity(LIBRARY);
}

BattleHexArray UnitProxy::getHexes(const Unit * unit)
{
	return unit->getHexes();
}

bool UnitProxy::hasAbsoluteImmunity(const Unit * unit, const spells::Spell * spell)
{
	return unit->hasAbsoluteImmunity(spell->getId());
}

LuaUnitState UnitProxy::copy(const Unit * unit)
{
	return LuaUnitState(unit->acquireState());
}

int UnitProxy::getBonuses(lua_State * L)
{
	LuaStack S(L);
	const Unit * unit = nullptr;
	S.get(1, unit);

	if (!unit || !lua_isfunction(L, 2))
	{
		S.clear();
		return 0;
	}

	auto allBonuses = unit->getAllBonuses(Selector::all);

	BonusList result;
	for (const auto & bonus : *allBonuses)
	{
		lua_pushvalue(L, 2);
		S.push(*bonus);
		lua_call(L, 1, 1);
		bool keep = lua_toboolean(L, -1);
		lua_pop(L, 1);
		if (keep)
			result.push_back(bonus);
	}

	S.clear();
	S.push(result);
	return 1;
}

}

VCMI_LIB_NAMESPACE_END
