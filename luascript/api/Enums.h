/*
 * Enums.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include <vcmi/scripting/ApiTags.h>
#include <vcmi/spells/Magic.h>

#include "../../lib/constants/Enumerations.h"
#include "../../lib/bonuses/BonusEnum.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

class Enums : public scripting::ApiSerializable<Enums>
{
	template<typename EnumType>
	using EnumMap = std::map<std::string, EnumType>;

	EnumMap<EHealLevel> exportHealLevel() const;
	EnumMap<EHealPower> exportHealPower() const;
	EnumMap<ESpellCastProblem> exportSpellCastProblem() const;
	EnumMap<spells::AimType> exportAimType() const;
	EnumMap<BonusDuration::BonusDuration> exportBonusDuration() const;
	EnumMap<si32> exportBonusSource() const;

public:
	template<typename Serializer>
	void serializeScript(Serializer & s)
	{
		s("HealLevel", exportHealLevel());
		s("HealPower", exportHealPower());
		s("SpellCastProblem", exportSpellCastProblem());
		s("AimType", exportAimType());
		s("BonusDuration", exportBonusDuration());
		s("BonusSource", exportBonusSource());
	}
};

}

VCMI_LIB_NAMESPACE_END
