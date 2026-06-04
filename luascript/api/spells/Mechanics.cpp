/*
 * Mechanics.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Mechanics.h"

#include "../Registry.h"
#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"

#include "../../../lib/battle/CBattleInfoCallback.h"
#include "../../../lib/spells/CSpell.h"
#include "../../../lib/battle/Unit.h"
#include "../../../lib/spells/Problem.h"
#include "../../../lib/mapObjects/CGHeroInstance.h"
#include "../../../lib/constants/EntityIdentifiers.h"
#include "../../../lib/GameLibrary.h"
#include "../../../lib/texts/CGeneralTextHandler.h"
#include "../../../lib/texts/Languages.h"

#include <vcmi/spells/Service.h>

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{
		using ::spells::Mechanics;

		bool MechanicsProxy::ownerMatchesUnit(const Mechanics * m, const battle::Unit * unit)
		{
			return m->ownerMatches(unit);
		}

		const spells::Spell * MechanicsProxy::getSpellByKey(const spells::Mechanics * m, const std::string & key)
		{
			return m->spells()->getByIndex(SpellID::decode(key));
		}

		std::string MechanicsProxy::getPluralFormTextID(const spells::Mechanics * m, const std::string & baseTextID, int32_t count)
		{
			std::string lang = LIBRARY->generaltexth->getPreferredLanguage();
			return Languages::getPluralFormTextID(lang, count, baseTextID);
		}

		const std::vector<MechanicsProxy::CustomRegType> MechanicsProxy::REGISTER_CUSTOM =
		{
			{"isPositive",             LuaMethodWrapper<&Mechanics::isPositiveSpell>::invoke,          false},
			{"isNegative",             LuaMethodWrapper<&Mechanics::isNegativeSpell>::invoke,          false},
			{"isSmart",                LuaMethodWrapper<&Mechanics::isSmart>::invoke,                  false},
			{"isMassive",              LuaMethodWrapper<&Mechanics::isMassive>::invoke,                false},
			{"alwaysHitFirstTarget",   LuaMethodWrapper<&Mechanics::alwaysHitFirstTarget>::invoke,     false},
			{"wouldResist",            LuaMethodWrapper<&Mechanics::wouldResist>::invoke,              false},

			{"getEffectLevel",         LuaMethodWrapper<&Mechanics::getEffectLevel>::invoke,           false},
			{"getRangeLevel",          LuaMethodWrapper<&Mechanics::getRangeLevel>::invoke,            false},
			{"getEffectPower",         LuaMethodWrapper<&Mechanics::getEffectPower>::invoke,           false},
			{"getEffectDuration",      LuaMethodWrapper<&Mechanics::getEffectDuration>::invoke,        false},
			{"getEffectValue",         LuaMethodWrapper<&Mechanics::getEffectValue>::invoke,           false},
			{"getCasterColor",         LuaMethodWrapper<&Mechanics::getCasterColor>::invoke,           false},
			{"getCasterSide",          LuaMethodWrapper<&Mechanics::getCasterSide>::invoke,            false},
			{"getHeroCaster",          LuaMethodWrapper<&Mechanics::getHeroCaster>::invoke,            false},
			{"getUnitCaster",          LuaMethodWrapper<&Mechanics::getUnitCaster>::invoke,            false},
			{"getCasterNameTextID",    LuaMethodWrapper<&Mechanics::getCasterNameTextID>::invoke,      false},
			{"getBattle",              LuaMethodWrapper<&Mechanics::battle>::invoke,                   false},
			{"getBattleID",            LuaMethodWrapper<&Mechanics::getBattleID>::invoke,              false},
			{"calculateRawEffectValue", LuaMethodWrapper<&Mechanics::calculateRawEffectValue>::invoke, false},
			{"applySpecificSpellBonus", LuaMethodWrapper<&Mechanics::applySpecificSpellBonus>::invoke, false},
			{"applySpellBonus",        LuaMethodWrapper<&Mechanics::applySpellBonus>::invoke,          false},
			{"isReceptive",            LuaMethodWrapper<&Mechanics::isReceptive>::invoke,              false},
			{"ownerMatches",           LuaFunctionWrapper<&ownerMatchesUnit>::invoke,                  false},
			{"getSpell",               LuaMethodWrapper<&Mechanics::getSpell>::invoke,                 false},
			{"getSpellByKey",          LuaFunctionWrapper<&MechanicsProxy::getSpellByKey>::invoke,     false},
			{"adjustEffectValue",      LuaMethodWrapper<&Mechanics::adjustEffectValue>::invoke,        false},
			{"getPluralFormTextID",    LuaFunctionWrapper<&MechanicsProxy::getPluralFormTextID>::invoke, false},
		};
}

VCMI_LIB_NAMESPACE_END
