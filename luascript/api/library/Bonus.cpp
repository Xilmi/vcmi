/*
 * Bonus.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"

#include "Bonus.h"

#include "../Registry.h"
#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"
#include "../../../lib/GameLibrary.h"
#include "../../../lib/CBonusTypeHandler.h"
#include "../../../lib/bonuses/BonusParameters.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::library
{

// --- BonusProxy ---

const std::vector<BonusProxy::CustomRegType> BonusProxy::REGISTER_CUSTOM =
{
	{"getType",               LuaFunctionWrapper<&BonusProxy::getType>::invoke,               false},
	{"getVal",                LuaFunctionWrapper<&BonusProxy::getVal>::invoke,                false},
	{"getSubtype",            LuaFunctionWrapper<&BonusProxy::getSubtype>::invoke,            false},
	{"getSourceID",           LuaFunctionWrapper<&BonusProxy::getSourceID>::invoke,           false},
	{"getSource",             LuaFunctionWrapper<&BonusProxy::getSource>::invoke,             false},
	{"getDuration",           LuaFunctionWrapper<&BonusProxy::getDuration>::invoke,           false},
	{"getValType",            LuaFunctionWrapper<&BonusProxy::getValType>::invoke,            false},
	{"getStacking",           LuaFunctionWrapper<&BonusProxy::getStacking>::invoke,           false},
	{"getTurnsRemain",        LuaFunctionWrapper<&BonusProxy::getTurnsRemain>::invoke,        false},
	{"isHidden",              LuaFunctionWrapper<&BonusProxy::isHidden>::invoke,              false},
	{"getParametersAsNumber", LuaFunctionWrapper<&BonusProxy::getParametersAsNumber>::invoke, false},
};

std::string BonusProxy::getType(const Bonus & b)
{
	return LIBRARY->bth->bonusToString(b.type);
}

si32        BonusProxy::getVal(const Bonus & b)        { return b.val; }
std::string BonusProxy::getSubtype(const Bonus & b)    { return b.subtype.toString(); }
std::string BonusProxy::getSourceID(const Bonus & b)   { return b.sid.toString(); }
si32        BonusProxy::getSource(const Bonus & b)     { return static_cast<si32>(b.source); }
si32        BonusProxy::getValType(const Bonus & b)    { return static_cast<si32>(b.valType); }
std::string BonusProxy::getStacking(const Bonus & b)   { return b.stacking; }
si16        BonusProxy::getTurnsRemain(const Bonus & b) { return b.turnsRemain; }
bool        BonusProxy::isHidden(const Bonus & b)      { return b.hidden; }
si32        BonusProxy::getParametersAsNumber(const Bonus & b) { return b.parameters ? b.parameters->toNumber() : 0; }

std::vector<BonusDuration::BonusDuration> BonusProxy::getDuration(const Bonus & b)
{
	static constexpr BonusDuration::BonusDuration all[] = {
		BonusDuration::PERMANENT,
		BonusDuration::ONE_BATTLE,
		BonusDuration::ONE_DAY,
		BonusDuration::ONE_WEEK,
		BonusDuration::N_TURNS,
		BonusDuration::N_DAYS,
		BonusDuration::UNTIL_BEING_ATTACKED,
		BonusDuration::UNTIL_ATTACK,
		BonusDuration::STACK_GETS_TURN,
		BonusDuration::COMMANDER_KILLED,
		BonusDuration::UNTIL_OWN_ATTACK,
		BonusDuration::UNTIL_TAKING_INDIRECT_DAMAGE,
		BonusDuration::UNTIL_AFTER_ATTACK_SEQUENCE,
	};
	std::vector<BonusDuration::BonusDuration> result;
	for (auto d : all)
		if (b.duration & d)
			result.push_back(d);
	return result;
}

// --- BonusListProxy ---

const std::vector<BonusListProxy::CustomRegType> BonusListProxy::REGISTER_CUSTOM =
{
	{"size",     LuaFunctionWrapper<&BonusListProxy::size>::invoke,     false},
	{"getBonus", LuaFunctionWrapper<&BonusListProxy::getBonus>::invoke, false},
};

int32_t BonusListProxy::size(const BonusList & list)
{
	return static_cast<int32_t>(list.size());
}

Bonus BonusListProxy::getBonus(const BonusList & list, int32_t index)
{
	if (index < 1 || index > static_cast<int32_t>(list.size()))
		throw std::out_of_range("BonusList index out of range");
	return *list[index - 1];
}

}

VCMI_LIB_NAMESPACE_END
