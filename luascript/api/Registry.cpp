/*
 * Registry.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Registry.h"

#include "battle/Unit.h"
#include "battle/UnitState.h"
#include "battle/BattleHex.h"
#include "battle/BattleHexArray.h"
#include "events/BattleEvents.h"
#include "events/EventBus.h"
#include "events/GenericEvents.h"
#include "events/SubscriptionRegistry.h"
#include "spells/Mechanics.h"
#include "spells/Problem.h"
#include "library/Artifact.h"
#include "library/Bonus.h"
#include "callback/IBattleInfoCallback.h"
#include "library/Creature.h"
#include "library/Faction.h"
#include "callback/IGameInfoCallback.h"
#include "library/HeroClass.h"
#include "adventure/HeroInstance.h"
#include "library/HeroType.h"
#include "Registry.h"
#include "callback/ServerCallback.h"
#include "library/Services.h"
#include "library/Skill.h"
#include "library/Spell.h"
#include "adventure/StackInstance.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

Registry::Registry()
{
	registerPrivate<library::ServicesProxy>("library.Services");
	registerPrivate<library::ArtifactProxy>("library.Artifact");
	registerPrivate<library::BonusProxy>("library.Bonus");
	registerPrivate<library::BonusListProxy>("library.BonusList");
	registerPrivate<library::CreatureProxy>("library.Creature");
	registerPrivate<library::FactionProxy>("library.Faction");
	registerPrivate<library::HeroClassProxy>("library.HeroClass");
	registerPrivate<library::HeroTypeProxy>("library.HeroType");
	registerPrivate<library::SkillProxy>("library.Skill");
	registerPrivate<library::SpellProxy>("library.Spell");

	registerPrivate<HeroInstanceProxy>("adventure.HeroInstance");
	registerPrivate<StackInstanceProxy>("adventure.StackInstance");

	registerPrivate<battle::BattleHexProxy>("battle.BattleHex");
	registerPrivate<battle::BattleHexArrayProxy>("battle.BattleHexArray");
	registerPrivate<battle::UnitProxy>("battle.Unit");
	registerPrivate<battle::LuaUnitStateProxy>("battle.UnitState");
	registerPrivate<ProblemProxy>("battle.SpellProblem");
	registerPrivate<MechanicsProxy>("battle.SpellMechanics");

	registerPrivate<events::ApplyDamageProxy>("events.ApplyDamage");
	registerPrivate<events::GameResumedProxy>("events.GameResumed");
	registerPrivate<events::PlayerGotTurnProxy>("events.PlayerGotTurn");
	registerPrivate<events::TurnStartedProxy>("events.TurnStarted");

	registerPrivate<IBattleInfoCallbackProxy>("game.Battle");
	registerPrivate<IGameInfoCallbackProxy>("game.Game");
	registerPrivate<ServerCallbackProxy>("game.Server");
	registerPrivate<events::EventBusProxy>("game.EventBus");
	registerPrivate<events::EventSubscriptionProxy>("game.EventSubscription");
}

const Registry * Registry::get()
{
	static const Registry instance;
	return &instance;
}

void Registry::addPrivate(const std::string & name, const std::shared_ptr<Registar> & item)
{
	privateData[name] = item;
}

void Registry::addPublic(const std::string & name, const std::shared_ptr<Registar> & item)
{
	privateData[name] = item;
	publicData[name] = item;
}

const Registar * Registry::find(const std::string & name) const
{
	auto iter = publicData.find(name);
	if(iter == publicData.end())
		return nullptr;
	else
		return iter->second.get();
}

}

VCMI_LIB_NAMESPACE_END
