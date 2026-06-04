/*
 * api/Artifact.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Artifact.h"

#include "../Registry.h"

#include "../../LuaStack.h"
#include "../../LuaCallWrapper.h"
#include "../../../lib/bonuses/Bonus.h"
#include "../../../lib/bonuses/IBonusBearer.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api::library
{

const std::vector<ArtifactProxy::CustomRegType> ArtifactProxy::REGISTER_CUSTOM =
{
	{"getIconIndex",   LuaMethodWrapper<&Entity::getIconIndex, Artifact>::invoke,           false},
	{"getIndex",       LuaMethodWrapper<&Entity::getIndex, Artifact>::invoke,               false},
	{"getJsonKey",     LuaMethodWrapper<&Entity::getJsonKey, Artifact>::invoke,             false},
	{"getName",        LuaMethodWrapper<&Entity::getNameTranslated, Artifact>::invoke,      false},

	{"getId",          LuaMethodWrapper<&EntityT<ArtifactID>::getId, Artifact>::invoke,     false},
	{"getBonusBearer", LuaMethodWrapper<&IConstBonusProvider::getBonusBearer, Artifact>::invoke, false},

	{"getDescription", LuaMethodWrapper<&Artifact::getDescriptionTranslated>::invoke,      false},
	{"getEventText",   LuaMethodWrapper<&Artifact::getEventTranslated>::invoke,            false},
	{"isBig",          LuaMethodWrapper<&Artifact::isBig>::invoke,                         false},
	{"isTradable",     LuaMethodWrapper<&Artifact::isTradable>::invoke,                    false},
	{"getPrice",       LuaMethodWrapper<&Artifact::getPrice>::invoke,                      false},
};

}

VCMI_LIB_NAMESPACE_END
