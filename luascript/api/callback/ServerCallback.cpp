/*
 * ServerCallback.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "GameLibrary.h"
#include "ServerCallback.h"

#include "../Registry.h"
#include "../battle/UnitState.h"

#include "../../LuaStack.h"
#include "../../../lib/networkPacks/PacksForClientBattle.h"
#include "../../../lib/networkPacks/SetStackEffect.h"
#include "../../../lib/battle/Unit.h"
#include "../../../lib/CStack.h"
#include "../../../lib/bonuses/BonusList.h"
#include "../../../lib/bonuses/Bonus.h"
#include "../../../lib/battle/CUnitState.h"
#include "../../../lib/json/JsonNode.h"
#include "../../../lib/texts/MetaString.h"
#include "../../../lib/constants/EntityIdentifiers.h"
#include "modding/IdentifierStorage.h"
#include "modding/ModScope.h"
#include "json/JsonBonus.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

const std::vector<ServerCallbackProxy::CustomRegType> ServerCallbackProxy::REGISTER_CUSTOM =
{
	{ "createUnit",        LuaFunctionWrapper<&ServerCallbackProxy::createUnit>::invoke,        false },
	{ "updateUnit",        LuaFunctionWrapper<&ServerCallbackProxy::updateUnit>::invoke,        false },
	{ "healUnit",          LuaCallWrapper<&ServerCallbackProxy::healUnit>::invoke,              false },
	{ "changeUnit",        LuaCallWrapper<&ServerCallbackProxy::changeUnit>::invoke,            false },
	{ "damageUnit",        LuaCallWrapper<&ServerCallbackProxy::damageUnit>::invoke,            false },
	{ "removeUnit",        LuaFunctionWrapper<&ServerCallbackProxy::removeUnit>::invoke,        false },
	{ "moveUnit",          LuaFunctionWrapper<&ServerCallbackProxy::moveUnit>::invoke,          false },
	{ "appendLog",         LuaFunctionWrapper<&ServerCallbackProxy::appendLog>::invoke,         false },
	{ "describeChanges",   LuaFunctionWrapper<&ServerCallbackProxy::describeChanges>::invoke,   false },
	{ "removeUnitBonuses", LuaFunctionWrapper<&ServerCallbackProxy::removeUnitBonuses>::invoke, false },
	{ "addUnitBonus",      LuaFunctionWrapper<&ServerCallbackProxy::addUnitBonus>::invoke,      false },
	{ "applyUnitBonuses",  LuaFunctionWrapper<&ServerCallbackProxy::applyUnitBonuses>::invoke,  false },
};

bool ServerCallbackProxy::describeChanges(ServerCallback * object)
{
	return object->describeChanges();
}

void ServerCallbackProxy::removeUnitBonuses(ServerCallback * object, BattleID battleID, const battle::Unit * unit, const BonusList & bonusList)
{
	std::vector<Bonus> buffer;
	for(const auto & b : bonusList)
		buffer.emplace_back(*b);

	if(buffer.empty())
		return;

	SetStackEffect sse;
	sse.battleID = battleID;
	sse.toRemove.emplace_back(unit->unitId(), buffer);
	object->apply(sse);
}

void ServerCallbackProxy::addUnitBonus(ServerCallback * object, BattleID battleID, uint32_t unitId, const JsonNode & data)
{
	Bonus b;
	JsonUtils::parseBonus(data, &b);

	SetStackEffect sse;
	sse.battleID = battleID;
	sse.toAdd.emplace_back(unitId, std::vector<Bonus>{b});
	object->apply(sse);
}

void ServerCallbackProxy::applyUnitBonuses(ServerCallback * object, BattleID battleID, const battle::Unit * unit, const JsonNode & bonuses, bool cumulative)
{
	std::vector<Bonus> buffer;
	for(const auto & [name, bonusJson] : bonuses.Struct())
	{
		auto b = JsonUtils::parseBonus(bonusJson);
		if(b)
			buffer.push_back(*b);
	}
	if(buffer.empty())
		return;

	SetStackEffect sse;
	sse.battleID = battleID;
	if(cumulative)
		sse.toAdd.emplace_back(unit->unitId(), buffer);
	else
		sse.toUpdate.emplace_back(unit->unitId(), buffer);
	object->apply(sse);
}

void ServerCallbackProxy::createUnit(ServerCallback * object, BattleID battleID, uint32_t id, JsonNode data)
{
	BattleUnitsChanged buc;
	UnitChanges uc;
	uc.operation = UnitChanges::EOperation::ADD;
	buc.battleID = battleID;
	uc.id = id;
	uc.data = std::move(data);
	buc.changedStacks.push_back(uc);
	object->apply(buc);
}

void ServerCallbackProxy::updateUnit(ServerCallback * object, BattleID battleID, uint32_t id, JsonNode data, int64_t healthDelta)
{
	BattleUnitsChanged buc;
	UnitChanges uc;
	uc.operation = UnitChanges::EOperation::UPDATE;
	buc.battleID = battleID;
	uc.id = id;
	uc.data = std::move(data);
	uc.healthDelta = healthDelta;
	buc.changedStacks.push_back(uc);
	object->apply(buc);
}

void ServerCallbackProxy::removeUnit(ServerCallback * object, BattleID battleID, const battle::Unit * unit)
{
	BattleUnitsChanged buc;
	UnitChanges uc;
	buc.battleID = battleID;
	uc.operation = UnitChanges::EOperation::REMOVE;
	uc.id = unit->unitId();
	buc.changedStacks.push_back(uc);
	object->apply(buc);
}

void ServerCallbackProxy::moveUnit(ServerCallback * object, BattleID battleID, const battle::Unit * unit, BattleHex destination, bool isTeleport)
{
	BattleStackMoved pack;
	pack.battleID = battleID;
	pack.stack = unit->unitId();
	pack.distance = 0;
	pack.teleporting = isTeleport;
	BattleHexArray tiles;
	tiles.insert(destination);
	pack.tilesToMove = tiles;
	object->apply(pack);
}

void ServerCallbackProxy::appendLog(ServerCallback * object, BattleID battleID, JsonNode config)
{
	BattleLogMessage msg;
	msg.battleID = battleID;
	msg.lines.push_back(MetaString::createFromLua(config));
	object->apply(msg);
}

int ServerCallbackProxy::changeUnit(lua_State * L)
{
	LuaStack S(L);

	ServerCallback * object = nullptr;
	BattleID battleID;
	battle::LuaUnitState unitState;
	int64_t healthDelta = 0;

	S.get(1, object);
	S.get(2, battleID);
	S.get(3, unitState);
	if(S.stackSize() >= 4)
		S.get(4, healthDelta);

	auto * cstate = unitState.getState();

	BattleUnitsChanged buc;
	UnitChanges uc(cstate->unitId(), UnitChanges::EOperation::UPDATE);
	buc.battleID = battleID;
	uc.data = cstate->save();
	uc.healthDelta = healthDelta;
	buc.changedStacks.push_back(uc);
	object->apply(buc);

	return S.retVoid();
}

int ServerCallbackProxy::damageUnit(lua_State * L)
{
	LuaStack S(L);

	ServerCallback * object = nullptr;
	BattleID battleID;
	const battle::Unit * unit = nullptr;
	int64_t damageAmount = 0;

	S.get(1, object);
	S.get(2, battleID);
	S.getNonNull(3, unit);
	S.get(4, damageAmount);

	BattleStackAttacked bsa;
	bsa.damageAmount = damageAmount;
	bsa.stackAttacked = unit->unitId();
	bsa.attackerID = -1;
	auto newState = unit->acquireState();
	CStack::prepareAttacked(bsa, *object->getRNG(), newState);

	StacksInjured si;
	si.battleID = battleID;
	si.stacks.push_back(bsa);
	object->apply(si);

	S.clear();
	S.push(bsa.damageAmount);
	S.push(static_cast<int64_t>(bsa.killedAmount));
	return 2;
}

int ServerCallbackProxy::healUnit(lua_State * L)
{
	LuaStack S(L);

	ServerCallback * object = nullptr;
	BattleID battleID;
	const battle::Unit * unit = nullptr;
	int64_t healthDelta;
	EHealLevel healLevel;
	EHealPower healPower;

	S.get(1, object);
	S.get(2, battleID);
	S.getNonNull(3, unit);
	S.get(4, healthDelta);
	S.get(5, healLevel);
	S.get(6, healPower);

	auto changedUnit = unit->acquire();
	battle::HealInfo info = changedUnit->heal(healthDelta, healLevel, healPower);

	BattleUnitsChanged buc;
	UnitChanges uc;
	buc.battleID = battleID;
	uc.operation = UnitChanges::EOperation::UPDATE;
	uc.id = unit->unitId();
	uc.data = changedUnit->save();
	uc.healthDelta = info.healedHealthPoints;
	buc.changedStacks.push_back(uc);

	object->apply(buc);

	S.clear();
	S.push(info.healedHealthPoints);
	S.push(static_cast<int64_t>(info.resurrectedCount));
	return 2;
}

}

VCMI_LIB_NAMESPACE_END
