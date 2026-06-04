/*
 * IGameInfoCallback.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "IGameInfoCallback.h"

#include <vcmi/Player.h>

#include "../../LuaCallWrapper.h"

#include "../../../lib/callback/IGameInfoCallback.h"
#include "../../../lib/mapObjects/CGHeroInstance.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{

const std::vector<IGameInfoCallbackProxy::CustomRegType> IGameInfoCallbackProxy::REGISTER_CUSTOM =
{
	{"getHero", LuaMethodWrapper<&GameCb::getHero>::invoke, false},
	{"getObj",  LuaMethodWrapper<&GameCb::getObj>::invoke,  false},
};

}

VCMI_LIB_NAMESPACE_END
