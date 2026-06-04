/*
 * IBattleInfoCallback.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include <vcmi/scripting/Service.h>
#include "../../../lib/battle/IBattleInfoCallback.h"

#include "../../LuaWrapper.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

class IBattleInfoCallbackProxy : public RawPointerWrapper<const IBattleInfoCallback, IBattleInfoCallbackProxy>
{
public:
	using Wrapper = RawPointerWrapper<const IBattleInfoCallback, IBattleInfoCallbackProxy>;

	static const std::vector<typename Wrapper::CustomRegType> REGISTER_CUSTOM;

	static int getAvailableHex(lua_State * L);
	static int getUnitsIf(lua_State * L);
	static bool isAccessibleForUnit(const IBattleInfoCallback * object, const battle::Unit * unit, BattleHex hex);
	static bool hasPenaltyOnLine(const IBattleInfoCallback * object, BattleHex from, BattleHex dest, bool checkWall, bool checkMoat);
	static const battle::Unit * getUnitByPos(const IBattleInfoCallback * object, BattleHex hex, bool onlyAlive);
};

}

VCMI_LIB_NAMESPACE_END
