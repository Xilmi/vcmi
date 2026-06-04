/*
 * api/Creature.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Creature.h"

#include "../Registry.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"
#include "../../../lib/bonuses/Bonus.h"
#include "../../../lib/bonuses/IBonusBearer.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::library
{

const std::vector<CreatureProxy::CustomRegType> CreatureProxy::REGISTER_CUSTOM =
{
	{"getIconIndex",          LuaMethodWrapper<&Entity::getIconIndex, Creature>::invoke,           false},
	{"getIndex",              LuaMethodWrapper<&Entity::getIndex, Creature>::invoke,               false},
	{"getJsonKey",            LuaMethodWrapper<&Entity::getJsonKey, Creature>::invoke,             false},
	{"getName",               LuaMethodWrapper<&Entity::getNameTranslated, Creature>::invoke,      false},
	{"getBonusBearer",        LuaMethodWrapper<&IConstBonusProvider::getBonusBearer, Creature>::invoke, false},

	{"getMaxHealth",          LuaMethodWrapper<&Creature::getMaxHealth, Creature>::invoke,          false},
	{"getPluralName",         LuaMethodWrapper<&Creature::getNamePluralTranslated>::invoke,        false},
	{"getSingularName",       LuaMethodWrapper<&Creature::getNameSingularTranslated>::invoke,      false},
	{"getNamePluralTextID",   LuaMethodWrapper<&Creature::getNamePluralTextID>::invoke,            false},
	{"getNameSingularTextID", LuaMethodWrapper<&Creature::getNameSingularTextID>::invoke,          false},
	{"getNameTextID",         LuaFunctionWrapper<&CreatureProxy::getNameTextID>::invoke,           false},

	{"getAdvMapAmountMin",    LuaMethodWrapper<&Creature::getAdvMapAmountMin>::invoke,             false},
	{"getAdvMapAmountMax",    LuaMethodWrapper<&Creature::getAdvMapAmountMax>::invoke,             false},
	{"getAIValue",            LuaMethodWrapper<&Creature::getAIValue>::invoke,                     false},
	{"getFightValue",         LuaMethodWrapper<&Creature::getFightValue>::invoke,                  false},
	{"getLevel",              LuaMethodWrapper<&Creature::getLevel>::invoke,                       false},
	{"getGrowth",             LuaMethodWrapper<&Creature::getGrowth>::invoke,                      false},
	{"getHorde",              LuaMethodWrapper<&Creature::getHorde>::invoke,                       false},
	{"getFactionID",          LuaMethodWrapper<&Creature::getFactionID, Creature>::invoke,          false},

	{"getBaseAttack",         LuaMethodWrapper<&Creature::getBaseAttack>::invoke,                  false},
	{"getBaseDefense",        LuaMethodWrapper<&Creature::getBaseDefense>::invoke,                 false},
	{"getBaseDamageMin",      LuaMethodWrapper<&Creature::getBaseDamageMin>::invoke,               false},
	{"getBaseDamageMax",      LuaMethodWrapper<&Creature::getBaseDamageMax>::invoke,               false},
	{"getBaseHitPoints",      LuaMethodWrapper<&Creature::getBaseHitPoints>::invoke,               false},
	{"getBaseSpellPoints",    LuaMethodWrapper<&Creature::getBaseSpellPoints>::invoke,             false},
	{"getBaseSpeed",          LuaMethodWrapper<&Creature::getBaseSpeed>::invoke,                   false},
	{"getBaseShots",          LuaMethodWrapper<&Creature::getBaseShots>::invoke,                   false},

	{"getRecruitCost",        LuaMethodWrapper<&Creature::getRecruitCost>::invoke,                 false},
	{"isDoubleWide",          LuaMethodWrapper<&Creature::isDoubleWide>::invoke,                   false},
};

std::string CreatureProxy::getNameTextID(const Creature * creature, int amount)
{
	return amount == 1 ? creature->getNameSingularTextID() : creature->getNamePluralTextID();
}

}

VCMI_LIB_NAMESPACE_END
