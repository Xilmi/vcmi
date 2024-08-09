/*
 * GameStatistics.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "GameStatistics.h"
#include "../CPlayerState.h"
#include "../constants/StringConstants.h"
#include "CGameState.h"
#include "TerrainHandler.h"
#include "CHeroHandler.h"
#include "StartInfo.h"
#include "HighScore.h"
#include "../mapObjects/CGHeroInstance.h"
#include "../mapObjects/CGTownInstance.h"
#include "../mapObjects/CGObjectInstance.h"
#include "../mapObjects/MiscObjects.h"
#include "../mapping/CMap.h"
#include "../entities/building/CBuilding.h"


VCMI_LIB_NAMESPACE_BEGIN

void StatisticDataSet::add(StatisticDataSetEntry entry)
{
	data.push_back(entry);
}

StatisticDataSetEntry StatisticDataSet::createEntry(const PlayerState * ps, const CGameState * gs)
{
	StatisticDataSetEntry data;

	HighScoreParameter param = HighScore::prepareHighScores(gs, ps->color, false);
	HighScoreCalculation scenarioHighScores;
	scenarioHighScores.parameters.push_back(param);
	scenarioHighScores.isCampaign = false;

	data.map = gs->map->name.toString();
	data.timestamp = std::time(0);
	data.day = gs->getDate(Date::DAY);
	data.player = ps->color;
	data.team = ps->team;
	data.isHuman = ps->isHuman();
	data.status = ps->status;
	data.resources = ps->resources;
	data.numberHeroes = ps->heroes.size();
	data.numberTowns = gs->howManyTowns(ps->color);
	data.numberArtifacts = Statistic::getNumberOfArts(ps);
	data.numberDwellings = gs->getPlayerState(ps->color)->dwellings.size();
	data.armyStrength = Statistic::getArmyStrength(ps, true);
	data.totalExperience = Statistic::getTotalExperience(ps);
	data.income = Statistic::getIncome(gs, ps);
	data.mapExploredRatio = Statistic::getMapExploredRatio(gs, ps->color);
	data.obeliskVisitedRatio = Statistic::getObeliskVisitedRatio(gs, ps->team);
	data.townBuiltRatio = Statistic::getTownBuiltRatio(ps);
	data.hasGrail = param.hasGrail;
	data.numMines = Statistic::getNumMines(gs, ps);
	data.score = scenarioHighScores.calculate().total;
	data.maxHeroLevel = Statistic::findBestHero(gs, ps->color) ? Statistic::findBestHero(gs, ps->color)->level : 0;
	data.numBattlesNeutral = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).numBattlesNeutral : 0;
	data.numBattlesPlayer = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).numBattlesPlayer : 0;
	data.numWinBattlesNeutral = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).numWinBattlesNeutral : 0;
	data.numWinBattlesPlayer = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).numWinBattlesPlayer : 0;
	data.numHeroSurrendered = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).numHeroSurrendered : 0;
	data.numHeroEscaped = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).numHeroEscaped : 0;
	data.spentResourcesForArmy = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).spentResourcesForArmy : TResources();
	data.spentResourcesForBuildings = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).spentResourcesForBuildings : TResources();
	data.tradeVolume = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).tradeVolume : TResources();
	data.movementPointsUsed = gs->statistic.accumulatedValues.count(ps->color) ? gs->statistic.accumulatedValues.at(ps->color).movementPointsUsed : 0;

	return data;
}

std::string StatisticDataSet::toCsv()
{
	std::stringstream ss;

	auto resources = std::vector<EGameResID>{EGameResID::GOLD, EGameResID::WOOD, EGameResID::MERCURY, EGameResID::ORE, EGameResID::SULFUR, EGameResID::CRYSTAL, EGameResID::GEMS};

	ss << "Map" << ";";
	ss << "Timestamp" << ";";
	ss << "Day" << ";";
	ss << "Player" << ";";
	ss << "Team" << ";";
	ss << "IsHuman" << ";";
	ss << "Status" << ";";
	ss << "NumberHeroes" << ";";
	ss << "NumberTowns" << ";";
	ss << "NumberArtifacts" << ";";
	ss << "NumberDwellings" << ";";
	ss << "ArmyStrength" << ";";
	ss << "TotalExperience" << ";";
	ss << "Income" << ";";
	ss << "MapExploredRatio" << ";";
	ss << "ObeliskVisitedRatio" << ";";
	ss << "TownBuiltRatio" << ";";
	ss << "HasGrail" << ";";
	ss << "Score" << ";";
	ss << "MaxHeroLevel" << ";";
	ss << "NumBattlesNeutral" << ";";
	ss << "NumBattlesPlayer" << ";";
	ss << "NumWinBattlesNeutral" << ";";
	ss << "NumWinBattlesPlayer" << ";";
	ss << "NumHeroSurrendered" << ";";
	ss << "NumHeroEscaped" << ";";
	ss << "MovementPointsUsed";
	for(auto & resource : resources)
		ss << ";" << GameConstants::RESOURCE_NAMES[resource];
	for(auto & resource : resources)
		ss << ";" << GameConstants::RESOURCE_NAMES[resource] + "Mines";
	for(auto & resource : resources)
		ss << ";" << GameConstants::RESOURCE_NAMES[resource] + "SpentResourcesForArmy";
	for(auto & resource : resources)
		ss << ";" << GameConstants::RESOURCE_NAMES[resource] + "SpentResourcesForBuildings";
	for(auto & resource : resources)
		ss << ";" << GameConstants::RESOURCE_NAMES[resource] + "TradeVolume";
	ss << "\r\n";

	for(auto & entry : data)
	{
		ss << entry.map << ";";
		ss << vstd::getFormattedDateTime(entry.timestamp, "%Y-%m-%dT%H:%M:%S") << ";";
		ss << entry.day << ";";
		ss << GameConstants::PLAYER_COLOR_NAMES[entry.player] << ";";
		ss << entry.team.getNum() << ";";
		ss << entry.isHuman << ";";
		ss << (int)entry.status << ";";
		ss << entry.numberHeroes << ";";
		ss << entry.numberTowns <<  ";";
		ss << entry.numberArtifacts << ";";
		ss << entry.numberDwellings << ";";
		ss << entry.armyStrength << ";";
		ss << entry.totalExperience << ";";
		ss << entry.income << ";";
		ss << entry.mapExploredRatio << ";";
		ss << entry.obeliskVisitedRatio << ";";
		ss << entry.townBuiltRatio << ";";
		ss << entry.hasGrail << ";";
		ss << entry.score << ";";
		ss << entry.maxHeroLevel << ";";
		ss << entry.numBattlesNeutral << ";";
		ss << entry.numBattlesPlayer << ";";
		ss << entry.numWinBattlesNeutral << ";";
		ss << entry.numWinBattlesPlayer << ";";
		ss << entry.numHeroSurrendered << ";";
		ss << entry.numHeroEscaped << ";";
		ss << entry.movementPointsUsed;
		for(auto & resource : resources)
			ss << ";" << entry.resources[resource];
		for(auto & resource : resources)
			ss << ";" << entry.numMines[resource];
		for(auto & resource : resources)
			ss << ";" << entry.spentResourcesForArmy[resource];
		for(auto & resource : resources)
			ss << ";" << entry.spentResourcesForBuildings[resource];
		for(auto & resource : resources)
			ss << ";" << entry.tradeVolume[resource];
		ss << "\r\n";
	}

	return ss.str();
}

std::vector<const CGMine *> Statistic::getMines(const CGameState * gs, const PlayerState * ps)
{
	std::vector<const CGMine *> tmp;

	/// FIXME: Dirty dirty hack
	/// Stats helper need some access to gamestate.
	std::vector<const CGObjectInstance *> ownedObjects;
	for(const CGObjectInstance * obj : gs->map->objects)
	{
		if(obj && obj->tempOwner == ps->color)
			ownedObjects.push_back(obj);
	}
	/// This is code from CPlayerSpecificInfoCallback::getMyObjects
	/// I'm really need to find out about callback interface design...

	for(const auto * object : ownedObjects)
	{
		//Mines
		if ( object->ID == Obj::MINE )
		{
			const auto * mine = dynamic_cast<const CGMine *>(object);
			assert(mine);

			tmp.push_back(mine);
		}
	}

	return tmp;
}

//calculates total number of artifacts that belong to given player
int Statistic::getNumberOfArts(const PlayerState * ps)
{
	int ret = 0;
	for(auto h : ps->heroes)
	{
		ret += (int)h->artifactsInBackpack.size() + (int)h->artifactsWorn.size();
	}
	return ret;
}

// get total strength of player army
si64 Statistic::getArmyStrength(const PlayerState * ps, bool withTownGarrison)
{
	si64 str = 0;

	for(auto h : ps->heroes)
	{
		if(!h->inTownGarrison || withTownGarrison)		//original h3 behavior
			str += h->getArmyStrength();
	}
	return str;
}

// get total experience of all heroes
si64 Statistic::getTotalExperience(const PlayerState * ps)
{
	si64 tmp = 0;

	for(auto h : ps->heroes)
		tmp += h->exp;
	
	return tmp;
}

// get total gold income
int Statistic::getIncome(const CGameState * gs, const PlayerState * ps)
{
	int percentIncome = gs->getStartInfo()->getIthPlayersSettings(ps->color).handicap.percentIncome;
	int totalIncome = 0;

	//Heroes can produce gold as well - skill, specialty or arts
	for(const auto & h : ps->heroes)
		totalIncome += h->valOfBonuses(Selector::typeSubtype(BonusType::GENERATE_RESOURCE, BonusSubtypeID(GameResID(GameResID::GOLD)))) * percentIncome / 100;

	//Add town income of all towns
	for(const auto & t : ps->towns)
		totalIncome += t->dailyIncome()[EGameResID::GOLD];

	for(const CGMine * mine : getMines(gs, ps))
		if(mine->producedResource == EGameResID::GOLD)
			totalIncome += mine->getProducedQuantity();

	return totalIncome;
}

float Statistic::getMapExploredRatio(const CGameState * gs, PlayerColor player)
{
	float visible = 0.0;
	float numTiles = 0.0;

	for(int layer = 0; layer < (gs->map->twoLevel ? 2 : 1); layer++)
		for(int y = 0; y < gs->map->height; ++y)
			for(int x = 0; x < gs->map->width; ++x)
			{
				TerrainTile tile = gs->map->getTile(int3(x, y, layer));

				if(tile.blocked && (!tile.visitable))
					continue;

				if(gs->isVisible(int3(x, y, layer), player))
					visible++;
				numTiles++;
			}
	
	return visible / numTiles;
}

const CGHeroInstance * Statistic::findBestHero(const CGameState * gs, const PlayerColor & color)
{
	auto &h = gs->players.at(color).heroes;
	if(h.empty())
		return nullptr;
	//best hero will be that with highest exp
	int best = 0;
	for(int b=1; b<h.size(); ++b)
	{
		if(h[b]->exp > h[best]->exp)
		{
			best = b;
		}
	}
	return h[best];
}

std::vector<std::vector<PlayerColor>> Statistic::getRank(std::vector<std::pair<PlayerColor, si64>> stats)
{
	std::sort(stats.begin(), stats.end(), [](const std::pair<PlayerColor, si64> & a, const std::pair<PlayerColor, si64> & b) { return a.second > b.second; });

	//put first element
	std::vector< std::vector<PlayerColor> > ret;
	std::vector<PlayerColor> tmp;
	tmp.push_back( stats[0].first );
	ret.push_back( tmp );

	//the rest of elements
	for(int g=1; g<stats.size(); ++g)
	{
		if(stats[g].second == stats[g-1].second)
		{
			(ret.end()-1)->push_back( stats[g].first );
		}
		else
		{
			//create next occupied rank
			std::vector<PlayerColor> tmp;
			tmp.push_back(stats[g].first);
			ret.push_back(tmp);
		}
	}

	return ret;
}

int Statistic::getObeliskVisited(const CGameState * gs, const TeamID & t)
{
	if(gs->map->obelisksVisited.count(t))
		return gs->map->obelisksVisited.at(t);
	else
		return 0;
}

float Statistic::getObeliskVisitedRatio(const CGameState * gs, const TeamID & t)
{
	if(!gs->map->obeliskCount)
		return 0;
	return (float)getObeliskVisited(gs, t) / (float)gs->map->obeliskCount;
}

std::map<EGameResID, int> Statistic::getNumMines(const CGameState * gs, const PlayerState * ps)
{
	std::map<EGameResID, int> tmp;

	for(auto & res : EGameResID::ALL_RESOURCES())
		tmp[res] = 0;

	for(const CGMine * mine : getMines(gs, ps))
		tmp[mine->producedResource]++;
	
	return tmp;
}

float Statistic::getTownBuiltRatio(const PlayerState * ps)
{
	float built = 0.0;
	float total = 0.0;

	for(const auto & t : ps->towns)
	{
		built += t->builtBuildings.size();
		for(const auto & b : t->town->buildings)
			if(!t->forbiddenBuildings.count(b.first))
				total += 1;
	}

	if(total < 1)
		return 0;
	
	return built / total;
}

VCMI_LIB_NAMESPACE_END
