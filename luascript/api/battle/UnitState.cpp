/*
 * UnitState.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#include "StdInc.h"

#include "UnitState.h"

#include "../../../lib/GameLibrary.h"
#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"
#include "../Registry.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::battle
{

const std::vector<LuaUnitStateProxy::CustomRegType> LuaUnitStateProxy::REGISTER_CUSTOM =
{
	// Read-only API
	{"getMinDamage",        LuaMethodWrapper<&LuaUnitState::getMinDamage>::invoke,     false},
	{"getMaxDamage",        LuaMethodWrapper<&LuaUnitState::getMaxDamage>::invoke,     false},
	{"getAttack",           LuaMethodWrapper<&LuaUnitState::getAttack>::invoke,        false},
	{"getDefense",          LuaMethodWrapper<&LuaUnitState::getDefense>::invoke,       false},
	{"isAlive",             LuaMethodWrapper<&LuaUnitState::isAlive>::invoke,          false},
	{"isClone",             LuaMethodWrapper<&LuaUnitState::isClone>::invoke,          false},
	{"isDead",              LuaMethodWrapper<&LuaUnitState::isDead>::invoke,           false},
	{"isGhost",             LuaMethodWrapper<&LuaUnitState::isGhost>::invoke,          false},
	{"isValidTarget",       LuaMethodWrapper<&LuaUnitState::isValidTarget>::invoke,    false},
	{"isInvincible",        LuaMethodWrapper<&LuaUnitState::isInvincible>::invoke,     false},
	{"isSummoned",          LuaMethodWrapper<&LuaUnitState::isSummoned>::invoke,       false},
	{"getOwner",            LuaMethodWrapper<&LuaUnitState::getOwner>::invoke,         false},
	{"getSlot",             LuaMethodWrapper<&LuaUnitState::getSlot>::invoke,          false},
	{"getPosition",         LuaMethodWrapper<&LuaUnitState::getPosition>::invoke,      false},
	{"getTotalHealth",      LuaMethodWrapper<&LuaUnitState::getTotalHealth>::invoke,   false},
	{"getAvailableHealth",  LuaMethodWrapper<&LuaUnitState::getAvailableHealth>::invoke, false},
	{"getCount",            LuaMethodWrapper<&LuaUnitState::getCount>::invoke,         false},
	{"getMaxHealth",        LuaMethodWrapper<&LuaUnitState::getMaxHealth>::invoke,     false},
	{"coversPos",           LuaMethodWrapper<&LuaUnitState::coversPos>::invoke,        false},
	{"getBaseAmount",       LuaMethodWrapper<&LuaUnitState::getBaseAmount>::invoke,    false},
	{"hasAbsoluteImmunity", LuaFunctionWrapper<&LuaUnitStateProxy::hasAbsoluteImmunity>::invoke, false},
	{"getCreature",         LuaFunctionWrapper<&LuaUnitStateProxy::getCreature>::invoke,         false},
	{"getHexes",            LuaFunctionWrapper<&LuaUnitStateProxy::getHexes>::invoke,            false},

	// Mutable — flags
	{"setDefending",        LuaMethodWrapper<&LuaUnitState::setDefending>::invoke,        false},
	{"setDefendingAnim",    LuaMethodWrapper<&LuaUnitState::setDefendingAnim>::invoke,    false},
	{"setCloned",           LuaMethodWrapper<&LuaUnitState::setCloned>::invoke,           false},
	{"setDrainedMana",      LuaMethodWrapper<&LuaUnitState::setDrainedMana>::invoke,      false},
	{"setFear",             LuaMethodWrapper<&LuaUnitState::setFear>::invoke,             false},
	{"setHadMorale",        LuaMethodWrapper<&LuaUnitState::setHadMorale>::invoke,        false},
	{"setCastSpellThisTurn",LuaMethodWrapper<&LuaUnitState::setCastSpellThisTurn>::invoke,false},
	{"setGhost",            LuaMethodWrapper<&LuaUnitState::setGhost>::invoke,            false},
	{"setGhostPending",     LuaMethodWrapper<&LuaUnitState::setGhostPending>::invoke,     false},
	{"setMoved",            LuaMethodWrapper<&LuaUnitState::setMoved>::invoke,            false},
	{"setSummoned",         LuaMethodWrapper<&LuaUnitState::setSummoned>::invoke,         false},
	{"setWaiting",          LuaMethodWrapper<&LuaUnitState::setWaiting>::invoke,          false},
	{"setWaitedThisTurn",   LuaMethodWrapper<&LuaUnitState::setWaitedThisTurn>::invoke,   false},

	// Mutable — other fields
	{"setPosition",         LuaMethodWrapper<&LuaUnitState::setPosition>::invoke,         false},
	{"setCloneID",          LuaMethodWrapper<&LuaUnitState::setCloneID>::invoke,          false},

	// Mutable — health
	{"damage",              LuaMethodWrapper<&LuaUnitState::damage>::invoke,              false},
	{"heal",                LuaCallWrapper<&LuaUnitStateProxy::heal>::invoke,             false},
};

// --- LuaUnitState implementation ---

LuaUnitState::LuaUnitState(std::shared_ptr<::battle::CUnitState> state_)
	: state(std::move(state_))
{
}

int LuaUnitState::getMinDamage(bool ranged) const { return state->getMinDamage(ranged); }
int LuaUnitState::getMaxDamage(bool ranged) const { return state->getMaxDamage(ranged); }
int LuaUnitState::getAttack(bool ranged)    const { return state->getAttack(ranged); }
int LuaUnitState::getDefense(bool ranged)   const { return state->getDefense(ranged); }

bool LuaUnitState::isAlive()                    const { return state->alive(); }
bool LuaUnitState::isClone()                    const { return state->isClone(); }
bool LuaUnitState::isDead()                     const { return state->isDead(); }
bool LuaUnitState::isGhost()                    const { return state->isGhost(); }
bool LuaUnitState::isValidTarget(bool allowDead) const { return state->isValidTarget(allowDead); }
bool LuaUnitState::isInvincible()               const { return state->isInvincible(); }
bool LuaUnitState::isSummoned()                 const { return state->summoned; }

PlayerColor LuaUnitState::getOwner()     const { return state->unitOwner(); }
SlotID      LuaUnitState::getSlot()      const { return state->unitSlot(); }
BattleHex   LuaUnitState::getPosition()  const { return state->getPosition(); }
int64_t     LuaUnitState::getTotalHealth()    const { return state->getTotalHealth(); }
int64_t     LuaUnitState::getAvailableHealth() const { return state->getAvailableHealth(); }
int32_t     LuaUnitState::getCount()         const { return state->getCount(); }
uint32_t    LuaUnitState::getMaxHealth()     const { return state->getMaxHealth(); }
bool        LuaUnitState::coversPos(BattleHex pos) const { return state->coversPos(pos); }
int32_t     LuaUnitState::getBaseAmount()    const { return state->unitBaseAmount(); }

void LuaUnitState::setDefending(bool v)         { state->defending = v; }
void LuaUnitState::setDefendingAnim(bool v)     { state->defendingAnim = v; }
void LuaUnitState::setCloned(bool v)            { state->cloned = v; }
void LuaUnitState::setDrainedMana(bool v)       { state->drainedMana = v; }
void LuaUnitState::setFear(bool v)              { state->fear = v; }
void LuaUnitState::setHadMorale(bool v)         { state->hadMorale = v; }
void LuaUnitState::setCastSpellThisTurn(bool v) { state->castSpellThisTurn = v; }
void LuaUnitState::setGhost(bool v)             { state->ghost = v; }
void LuaUnitState::setGhostPending(bool v)      { state->ghostPending = v; }
void LuaUnitState::setMoved(bool v)             { state->movedThisRound = v; }
void LuaUnitState::setSummoned(bool v)          { state->summoned = v; }
void LuaUnitState::setWaiting(bool v)           { state->waiting = v; }
void LuaUnitState::setWaitedThisTurn(bool v)    { state->waitedThisTurn = v; }

void LuaUnitState::setPosition(BattleHex hex)   { state->setPosition(hex); }
void LuaUnitState::setCloneID(int32_t id)        { state->cloneID = id; }

int64_t LuaUnitState::damage(int64_t amount)
{
	state->damage(amount);
	return amount;
}

// --- LuaUnitStateProxy static methods ---

bool LuaUnitStateProxy::hasAbsoluteImmunity(LuaUnitState state, const spells::Spell * spell)
{
	return state.getState()->hasAbsoluteImmunity(spell->getId());
}

const Creature * LuaUnitStateProxy::getCreature(LuaUnitState state)
{
	return state.getState()->creatureId().toEntity(LIBRARY);
}

BattleHexArray LuaUnitStateProxy::getHexes(LuaUnitState state)
{
	return state.getState()->getHexes();
}

int LuaUnitStateProxy::heal(lua_State * L)
{
	LuaStack S(L);

	LuaUnitState unitState;
	int64_t amount = 0;
	EHealLevel healLevel = EHealLevel::HEAL;
	EHealPower healPower = EHealPower::PERMANENT;

	S.get(1, unitState);
	S.get(2, amount);
	S.get(3, healLevel);
	S.get(4, healPower);

	::battle::HealInfo info = unitState.getState()->heal(amount, healLevel, healPower);

	S.clear();
	S.push(info.healedHealthPoints);
	S.push(static_cast<int64_t>(info.resurrectedCount));
	return 2;
}

}

VCMI_LIB_NAMESPACE_END
