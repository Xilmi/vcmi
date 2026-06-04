/*
 * Mechanics.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include "../../LuaWrapper.h"
#include "../../../lib/spells/ISpellMechanics.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{
	class MechanicsProxy : public RawPointerWrapper<::spells::Mechanics, MechanicsProxy>
	{
	public:
		using Wrapper = RawPointerWrapper<::spells::Mechanics, MechanicsProxy>;

		static const std::vector<typename Wrapper::CustomRegType> REGISTER_CUSTOM;

		static bool ownerMatchesUnit(const ::spells::Mechanics * m, const battle::Unit * unit);
		static const ::spells::Spell * getSpellByKey(const ::spells::Mechanics * m, const std::string & key);
		static std::string getPluralFormTextID(const ::spells::Mechanics * m, const std::string & baseTextID, int32_t count);
	};
}

VCMI_LIB_NAMESPACE_END
