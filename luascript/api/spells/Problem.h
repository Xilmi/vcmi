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
#include "../../../lib/spells/Problem.h"
#include "../../../lib/json/JsonNode.h"
#include "../../../lib/constants/Enumerations.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace spells { class Mechanics; }

namespace scripting::api
{
		class ProblemProxy : public RawPointerWrapper<::spells::Problem, ProblemProxy>
		{
		public:
			using Wrapper = RawPointerWrapper<::spells::Problem, ProblemProxy>;

			static const std::vector<typename Wrapper::CustomRegType> REGISTER_CUSTOM;

			static void addCustom(::spells::Problem * problem, const JsonNode & config);
			static void addGeneric(::spells::Problem * problem, const ::spells::Mechanics * mechanics);
			static void addStandard(::spells::Problem * problem, const ::spells::Mechanics * mechanics, ESpellCastProblem spellProblem);
		};
}

VCMI_LIB_NAMESPACE_END
