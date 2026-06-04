/*
 * Problem.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"

#include "Problem.h"

#include "../../../lib/json/JsonNode.h"
#include "../../../lib/spells/ISpellMechanics.h"
#include "../../../lib/texts/MetaString.h"

VCMI_LIB_NAMESPACE_BEGIN

namespace scripting::api
{
using ::spells::Problem;
using ::spells::Mechanics;

void ProblemProxy::addCustom(Problem * problem, const JsonNode & config)
{
	problem->add(MetaString::createFromLua(config));
}

void ProblemProxy::addGeneric(Problem * problem, const Mechanics * mechanics)
{
	mechanics->adaptGenericProblem(*problem);
}

void ProblemProxy::addStandard(Problem * problem, const Mechanics * mechanics, ESpellCastProblem spellProblem)
{
	mechanics->adaptProblem(spellProblem, *problem);
}

const std::vector<ProblemProxy::CustomRegType> ProblemProxy::REGISTER_CUSTOM =
{
	{"addCustom",   LuaFunctionWrapper<&ProblemProxy::addCustom>::invoke,   false},
	{"addGeneric",  LuaFunctionWrapper<&ProblemProxy::addGeneric>::invoke,  false},
	{"addStandard", LuaFunctionWrapper<&ProblemProxy::addStandard>::invoke, false},
};

}

VCMI_LIB_NAMESPACE_END
