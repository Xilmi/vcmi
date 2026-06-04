/*
 * Enums.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Enums.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

Enums::EnumMap<EHealLevel> Enums::exportHealLevel() const
{
	return {
		{ "heal", EHealLevel::HEAL },
		{ "resurrect", EHealLevel::RESURRECT },
		{ "overheal", EHealLevel::OVERHEAL },
	};
}

Enums::EnumMap<EHealPower> Enums::exportHealPower() const
{
	return {
		{ "oneBattle", EHealPower::ONE_BATTLE },
		{ "permanent", EHealPower::PERMANENT },
	};
}

Enums::EnumMap<ESpellCastProblem> Enums::exportSpellCastProblem() const
{
	return {
		{ "noHeroToCastSpell", ESpellCastProblem::NO_HERO_TO_CAST_SPELL},
		{ "castsPerTurnLimit", ESpellCastProblem::CASTS_PER_TURN_LIMIT},
		{ "noSpellbook", ESpellCastProblem::NO_SPELLBOOK},
		{ "heroDoesntKnowSpell", ESpellCastProblem::HERO_DOESNT_KNOW_SPELL},
		{ "notEnoughMana", ESpellCastProblem::NOT_ENOUGH_MANA},
		{ "advmapSpellInsteadOfBattleSpell", ESpellCastProblem::ADVMAP_SPELL_INSTEAD_OF_BATTLE_SPELL},
		{ "spellLevelLimitExceeded", ESpellCastProblem::SPELL_LEVEL_LIMIT_EXCEEDED},
		{ "noSpellsToDispel", ESpellCastProblem::NO_SPELLS_TO_DISPEL},
		{ "noAppropriateTarget", ESpellCastProblem::NO_APPROPRIATE_TARGET},
		{ "stackImmuneToSpell", ESpellCastProblem::STACK_IMMUNE_TO_SPELL},
		{ "wrongSpellTarget", ESpellCastProblem::WRONG_SPELL_TARGET},
		{ "ongoinTacticPhase", ESpellCastProblem::ONGOING_TACTIC_PHASE},
		{ "magicIsBlocked", ESpellCastProblem::MAGIC_IS_BLOCKED}
	};
}

Enums::EnumMap<spells::AimType> Enums::exportAimType() const
{
	return {
		{ "nothing", spells::AimType::NOTHING },
		{ "creature", spells::AimType::CREATURE },
		{ "obstacle", spells::AimType::OBSTACLE },
		{ "location", spells::AimType::LOCATION },
	};
}

Enums::EnumMap<BonusDuration::BonusDuration> Enums::exportBonusDuration() const
{
	return {
		{ "permanent",                  BonusDuration::PERMANENT },
		{ "oneBattle",                  BonusDuration::ONE_BATTLE },
		{ "oneDay",                     BonusDuration::ONE_DAY },
		{ "oneWeek",                    BonusDuration::ONE_WEEK },
		{ "nTurns",                     BonusDuration::N_TURNS },
		{ "nDays",                      BonusDuration::N_DAYS },
		{ "untilBeingAttacked",         BonusDuration::UNTIL_BEING_ATTACKED },
		{ "untilAttack",                BonusDuration::UNTIL_ATTACK },
		{ "stackGetsTurn",              BonusDuration::STACK_GETS_TURN },
		{ "commanderKilled",            BonusDuration::COMMANDER_KILLED },
		{ "untilOwnAttack",             BonusDuration::UNTIL_OWN_ATTACK },
		{ "untilTakingIndirectDamage",  BonusDuration::UNTIL_TAKING_INDIRECT_DAMAGE },
		{ "untilAfterAttackSequence",   BonusDuration::UNTIL_AFTER_ATTACK_SEQUENCE },
	};
}

Enums::EnumMap<si32> Enums::exportBonusSource() const
{
	return {
		{ "artifact",          static_cast<si32>(BonusSource::ARTIFACT) },
		{ "artifactInstance",  static_cast<si32>(BonusSource::ARTIFACT_INSTANCE) },
		{ "objectType",        static_cast<si32>(BonusSource::OBJECT_TYPE) },
		{ "objectInstance",    static_cast<si32>(BonusSource::OBJECT_INSTANCE) },
		{ "creatureAbility",   static_cast<si32>(BonusSource::CREATURE_ABILITY) },
		{ "terrainNative",     static_cast<si32>(BonusSource::TERRAIN_NATIVE) },
		{ "terrainOverlay",    static_cast<si32>(BonusSource::TERRAIN_OVERLAY) },
		{ "spellEffect",       static_cast<si32>(BonusSource::SPELL_EFFECT) },
		{ "townStructure",     static_cast<si32>(BonusSource::TOWN_STRUCTURE) },
		{ "heroBaseSkill",     static_cast<si32>(BonusSource::HERO_BASE_SKILL) },
		{ "secondarySkill",    static_cast<si32>(BonusSource::SECONDARY_SKILL) },
		{ "heroSpecial",       static_cast<si32>(BonusSource::HERO_SPECIAL) },
		{ "army",              static_cast<si32>(BonusSource::ARMY) },
		{ "campaignBonus",     static_cast<si32>(BonusSource::CAMPAIGN_BONUS) },
		{ "stackExperience",   static_cast<si32>(BonusSource::STACK_EXPERIENCE) },
		{ "commander",         static_cast<si32>(BonusSource::COMMANDER) },
		{ "global",            static_cast<si32>(BonusSource::GLOBAL) },
		{ "other",             static_cast<si32>(BonusSource::OTHER) },
	};
}

}

VCMI_LIB_NAMESPACE_END
