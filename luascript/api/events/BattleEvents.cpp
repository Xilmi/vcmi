/*
 * BattleEvents.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "BattleEvents.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"
#include "../Registry.h"
#include "../../../lib/battle/Unit.h"
#include "SubscriptionRegistry.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::events
{
using ::events::ApplyDamage;

const std::vector<ApplyDamageProxy::CustomRegType> ApplyDamageProxy::REGISTER_CUSTOM =
{
	{
		"subscribeBefore",
		&SubscriptionRegistryProxy<ApplyDamageProxy>::subscribeBefore,
		true
	},
	{
		"subscribeAfter",
		&SubscriptionRegistryProxy<ApplyDamageProxy>::subscribeAfter,
		true
	},
	{"getInitialDamage", LuaMethodWrapper<&ApplyDamage::getInitialDamage>::invoke, false},
	{"getDamage",        LuaMethodWrapper<&ApplyDamage::getDamage>::invoke,        false},
	{"setDamage",        LuaMethodWrapper<&ApplyDamage::setDamage>::invoke,        false},
	{"getTarget",        LuaMethodWrapper<&ApplyDamage::getTarget>::invoke,        false},

};

}


VCMI_LIB_NAMESPACE_END
