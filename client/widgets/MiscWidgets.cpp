/*
 * MiscWidgets.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "MiscWidgets.h"

#include "CComponent.h"

#include "../gui/CGuiHandler.h"
#include "../gui/CursorHandler.h"

#include "../CPlayerInterface.h"
#include "../CGameInfo.h"
#include "../PlayerLocalState.h"
#include "../gui/WindowHandler.h"
#include "../windows/CTradeWindow.h"
#include "../widgets/TextControls.h"
#include "../widgets/CGarrisonInt.h"
#include "../windows/CCastleInterface.h"
#include "../windows/InfoWindows.h"
#include "../render/Canvas.h"
#include "../render/Graphics.h"

#include "../../CCallback.h"

#include "../../lib/CConfigHandler.h"
#include "../../lib/gameState/InfoAboutArmy.h"
#include "../../lib/CGeneralTextHandler.h"
#include "../../lib/GameSettings.h"
#include "../../lib/TextOperations.h"
#include "../../lib/mapObjects/CGCreature.h"
#include "../../lib/mapObjects/CGHeroInstance.h"
#include "../../lib/mapObjects/CGTownInstance.h"

void CHoverableArea::hover (bool on)
{
	if (on)
		GH.statusbar()->write(hoverText);
	else
		GH.statusbar()->clearIfMatching(hoverText);
}

CHoverableArea::CHoverableArea()
{
	addUsedEvents(HOVER);
}

CHoverableArea::~CHoverableArea()
{
}

void LRClickableAreaWText::clickPressed(const Point & cursorPosition)
{
	if(!text.empty())
		LOCPLINT->showInfoDialog(text);
}
void LRClickableAreaWText::showPopupWindow(const Point & cursorPosition)
{
	if (!text.empty())
		CRClickPopup::createAndPush(text);
}

LRClickableAreaWText::LRClickableAreaWText()
{
	init();
}

LRClickableAreaWText::LRClickableAreaWText(const Rect &Pos, const std::string &HoverText, const std::string &ClickText)
{
	init();
	pos = Pos + pos.topLeft();
	hoverText = HoverText;
	text = ClickText;
}

LRClickableAreaWText::~LRClickableAreaWText()
{
}

void LRClickableAreaWText::init()
{
	addUsedEvents(LCLICK | SHOW_POPUP | HOVER);
}

void LRClickableAreaWTextComp::clickPressed(const Point & cursorPosition)
{
	std::vector<std::shared_ptr<CComponent>> comp(1, createComponent());
	LOCPLINT->showInfoDialog(text, comp);
}

LRClickableAreaWTextComp::LRClickableAreaWTextComp(const Rect &Pos, int BaseType)
	: LRClickableAreaWText(Pos), baseType(BaseType), bonusValue(-1)
{
	type = -1;
}

std::shared_ptr<CComponent> LRClickableAreaWTextComp::createComponent() const
{
	if(baseType >= 0)
		return std::make_shared<CComponent>(CComponent::Etype(baseType), type, bonusValue);
	else
		return std::shared_ptr<CComponent>();
}

void LRClickableAreaWTextComp::showPopupWindow(const Point & cursorPosition)
{
	if(auto comp = createComponent())
	{
		CRClickPopup::createAndPush(text, CInfoWindow::TCompsInfo(1, comp));
		return;
	}

	LRClickableAreaWText::showPopupWindow(cursorPosition); //only if with-component variant not occurred
}

CHeroArea::CHeroArea(int x, int y, const CGHeroInstance * hero)
	: CIntObject(LCLICK | HOVER),
	hero(hero),
	clickFunctor(nullptr)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);

	pos.x += x;
	pos.w = 58;
	pos.y += y;
	pos.h = 64;

	if(hero)
	{
		portrait = std::make_shared<CAnimImage>(AnimationPath::builtin("PortraitsLarge"), hero->getIconIndex());
		clickFunctor = [hero]() -> void
		{
			LOCPLINT->openHeroWindow(hero);
		};
	}
}

void CHeroArea::addClickCallback(ClickFunctor callback)
{
	clickFunctor = callback;
}

void CHeroArea::clickPressed(const Point & cursorPosition)
{
	if(clickFunctor)
		clickFunctor();
}

void CHeroArea::hover(bool on)
{
	if (on && hero)
		GH.statusbar()->write(hero->getObjectName());
	else
		GH.statusbar()->clear();
}

void LRClickableAreaOpenTown::clickPressed(const Point & cursorPosition)
{
	if(town)
	{
		LOCPLINT->openTownWindow(town);
		if ( type == 2 )
			LOCPLINT->castleInt->builds->buildingClicked(BuildingID::VILLAGE_HALL);
		else if ( type == 3 && town->fortLevel() )
			LOCPLINT->castleInt->builds->buildingClicked(BuildingID::FORT);
	}
}

LRClickableAreaOpenTown::LRClickableAreaOpenTown(const Rect & Pos, const CGTownInstance * Town)
	: LRClickableAreaWTextComp(Pos, -1), town(Town)
{
}

void LRClickableArea::clickPressed(const Point & cursorPosition)
{
	if(onClick)
		onClick();
}

void LRClickableArea::showPopupWindow(const Point & cursorPosition)
{
	if(onPopup)
		onPopup();
}

LRClickableArea::LRClickableArea(const Rect & Pos, std::function<void()> onClick, std::function<void()> onPopup)
	: CIntObject(LCLICK | SHOW_POPUP), onClick(onClick), onPopup(onPopup)
{
	pos = Pos + pos.topLeft();
}

void CMinorResDataBar::show(Canvas & to)
{
}

std::string CMinorResDataBar::buildDateString()
{
	std::string pattern = "%s: %d, %s: %d, %s: %d";

	auto formatted = boost::format(pattern)
		% CGI->generaltexth->translate("core.genrltxt.62") % LOCPLINT->cb->getDate(Date::MONTH)
		% CGI->generaltexth->translate("core.genrltxt.63") % LOCPLINT->cb->getDate(Date::WEEK)
		% CGI->generaltexth->translate("core.genrltxt.64") % LOCPLINT->cb->getDate(Date::DAY_OF_WEEK);

	return boost::str(formatted);
}

void CMinorResDataBar::showAll(Canvas & to)
{
	CIntObject::showAll(to);

	for (GameResID i=EGameResID::WOOD; i<=EGameResID::GOLD; ++i)
	{
		std::string text = std::to_string(LOCPLINT->cb->getResourceAmount(i));

		Point target(pos.x + 50 + 76 * GameResID(i), pos.y + pos.h/2);

		to.drawText(target, FONT_SMALL, Colors::WHITE, ETextAlignment::CENTER, text);
	}

	Point target(pos.x+545+(pos.w-545)/2,pos.y+pos.h/2);

	to.drawText(target, FONT_SMALL, Colors::WHITE, ETextAlignment::CENTER, buildDateString());
}

CMinorResDataBar::CMinorResDataBar()
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);

	pos.x = 7;
	pos.y = 575;

	background = std::make_shared<CPicture>(ImagePath::builtin("KRESBAR.bmp"));
	background->colorize(LOCPLINT->playerID);

	pos.w = background->pos.w;
	pos.h = background->pos.h;
}

CMinorResDataBar::~CMinorResDataBar() = default;

void CArmyTooltip::init(const InfoAboutArmy &army)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);

	title = std::make_shared<CLabel>(66, 2, FONT_SMALL, ETextAlignment::TOPLEFT, Colors::WHITE, army.name);

	std::vector<Point> slotsPos;
	slotsPos.push_back(Point(36, 73));
	slotsPos.push_back(Point(72, 73));
	slotsPos.push_back(Point(108, 73));
	slotsPos.push_back(Point(18, 122));
	slotsPos.push_back(Point(54, 122));
	slotsPos.push_back(Point(90, 122));
	slotsPos.push_back(Point(126, 122));

	for(auto & slot : army.army)
	{
		if(slot.first.getNum() >= GameConstants::ARMY_SIZE)
		{
			logGlobal->warn("%s has stack in slot %d", army.name, slot.first.getNum());
			continue;
		}

		icons.push_back(std::make_shared<CAnimImage>(AnimationPath::builtin("CPRSMALL"), slot.second.type->getIconIndex(), 0, slotsPos[slot.first.getNum()].x, slotsPos[slot.first.getNum()].y));

		std::string subtitle;
		if(army.army.isDetailed)
		{
			subtitle = TextOperations::formatMetric(slot.second.count, 4);
		}
		else
		{
			//if =0 - we have no information about stack size at all
			if(slot.second.count)
			{
				if(settings["gameTweaks"]["numericCreaturesQuantities"].Bool())
				{
					subtitle = CCreature::getQuantityRangeStringForId((CCreature::CreatureQuantityId)slot.second.count);
				}
				else
				{
					subtitle = CGI->generaltexth->arraytxt[171 + 3*(slot.second.count)];
				}
			}
		}

		subtitles.push_back(std::make_shared<CLabel>(slotsPos[slot.first.getNum()].x + 17, slotsPos[slot.first.getNum()].y + 39, FONT_TINY, ETextAlignment::CENTER, Colors::WHITE, subtitle));
	}

}

CArmyTooltip::CArmyTooltip(Point pos, const InfoAboutArmy & army):
	CIntObject(0, pos)
{
	init(army);
}

CArmyTooltip::CArmyTooltip(Point pos, const CArmedInstance * army):
	CIntObject(0, pos)
{
	init(InfoAboutArmy(army, true));
}

void CHeroTooltip::init(const InfoAboutHero & hero)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);
	portrait = std::make_shared<CAnimImage>(AnimationPath::builtin("PortraitsLarge"), hero.getIconIndex(), 0, 3, 2);

	if(hero.details)
	{
		for(size_t i = 0; i < hero.details->primskills.size(); i++)
			labels.push_back(std::make_shared<CLabel>(75 + 28 * (int)i, 58, FONT_SMALL, ETextAlignment::CENTER, Colors::WHITE,
					   std::to_string(hero.details->primskills[i])));

		labels.push_back(std::make_shared<CLabel>(158, 98, FONT_TINY, ETextAlignment::CENTER, Colors::WHITE, std::to_string(hero.details->mana)));

		morale = std::make_shared<CAnimImage>(AnimationPath::builtin("IMRL22"), hero.details->morale + 3, 0, 5, 74);
		luck = std::make_shared<CAnimImage>(AnimationPath::builtin("ILCK22"), hero.details->luck + 3, 0, 5, 91);
	}
}

CHeroTooltip::CHeroTooltip(Point pos, const InfoAboutHero &hero):
	CArmyTooltip(pos, hero)
{
	init(hero);
}

CHeroTooltip::CHeroTooltip(Point pos, const CGHeroInstance * hero):
	CArmyTooltip(pos, InfoAboutHero(hero, InfoAboutHero::EInfoLevel::DETAILED))
{
	init(InfoAboutHero(hero, InfoAboutHero::EInfoLevel::DETAILED));
}

CInteractableHeroTooltip::CInteractableHeroTooltip(Point pos, const CGHeroInstance * hero)
{
	init(InfoAboutHero(hero, InfoAboutHero::EInfoLevel::DETAILED));

	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);
	garrison = std::make_shared<CGarrisonInt>(pos + Point(0, 73), 4, Point(0, 0), hero, nullptr, true, true, CGarrisonInt::ESlotsLayout::REVERSED_TWO_ROWS);
}

void CInteractableHeroTooltip::init(const InfoAboutHero & hero)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);
	portrait = std::make_shared<CAnimImage>(AnimationPath::builtin("PortraitsLarge"), hero.getIconIndex(), 0, 3, 2);
	title = std::make_shared<CLabel>(66, 2, FONT_SMALL, ETextAlignment::TOPLEFT, Colors::WHITE, hero.name);

	if(hero.details)
	{
		for(size_t i = 0; i < hero.details->primskills.size(); i++)
			labels.push_back(std::make_shared<CLabel>(75 + 28 * (int)i, 58, FONT_SMALL, ETextAlignment::CENTER, Colors::WHITE,
													  std::to_string(hero.details->primskills[i])));

		labels.push_back(std::make_shared<CLabel>(158, 98, FONT_TINY, ETextAlignment::CENTER, Colors::WHITE, std::to_string(hero.details->mana)));

		morale = std::make_shared<CAnimImage>(AnimationPath::builtin("IMRL22"), hero.details->morale + 3, 0, 5, 74);
		luck = std::make_shared<CAnimImage>(AnimationPath::builtin("ILCK22"), hero.details->luck + 3, 0, 5, 91);
	}
}

void CTownTooltip::init(const InfoAboutTown & town)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);

	//order of icons in def: fort, citadel, castle, no fort
	size_t fortIndex = town.fortLevel ? town.fortLevel - 1 : 3;

	fort = std::make_shared<CAnimImage>(AnimationPath::builtin("ITMCLS"), fortIndex, 0, 105, 31);

	assert(town.tType);

	size_t iconIndex = town.tType->clientInfo.icons[town.fortLevel > 0][town.built >= CGI->settings()->getInteger(EGameSettings::TOWNS_BUILDINGS_PER_TURN_CAP)];

	build = std::make_shared<CAnimImage>(AnimationPath::builtin("itpt"), iconIndex, 0, 3, 2);

	if(town.details)
	{
		hall = std::make_shared<CAnimImage>(AnimationPath::builtin("ITMTLS"), town.details->hallLevel, 0, 67, 31);

		if(town.details->goldIncome)
		{
			income = std::make_shared<CLabel>(157, 58, FONT_TINY, ETextAlignment::CENTER, Colors::WHITE,
					   std::to_string(town.details->goldIncome));
		}
		if(town.details->garrisonedHero) //garrisoned hero icon
			garrisonedHero = std::make_shared<CPicture>(ImagePath::builtin("TOWNQKGH"), 149, 76);

		if(town.details->customRes)//silo is built
		{
			if(town.tType->primaryRes == EGameResID::WOOD_AND_ORE )// wood & ore
			{
				res1 = std::make_shared<CAnimImage>(AnimationPath::builtin("SMALRES"), GameResID(EGameResID::WOOD), 0, 7, 75);
				res2 = std::make_shared<CAnimImage>(AnimationPath::builtin("SMALRES"), GameResID(EGameResID::ORE), 0, 7, 88);
			}
			else
			{
				res1 = std::make_shared<CAnimImage>(AnimationPath::builtin("SMALRES"), town.tType->primaryRes, 0, 7, 81);
			}
		}
	}
}

CTownTooltip::CTownTooltip(Point pos, const InfoAboutTown & town)
	: CArmyTooltip(pos, town)
{
	init(town);
}

CTownTooltip::CTownTooltip(Point pos, const CGTownInstance * town)
	: CArmyTooltip(pos, InfoAboutTown(town, true))
{
	init(InfoAboutTown(town, true));
}

CInteractableTownTooltip::CInteractableTownTooltip(Point pos, const CGTownInstance * town)
{
	init(town);

	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);
	garrison = std::make_shared<CGarrisonInt>(pos + Point(0, 73), 4, Point(0, 0), town->getUpperArmy(), nullptr, true, true, CGarrisonInt::ESlotsLayout::REVERSED_TWO_ROWS);
}

void CInteractableTownTooltip::init(const CGTownInstance * town)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);

	const InfoAboutTown townInfo = InfoAboutTown(town, true);
	int townId = town->id;

	//order of icons in def: fort, citadel, castle, no fort
	size_t fortIndex = townInfo.fortLevel ? townInfo.fortLevel - 1 : 3;

	fort = std::make_shared<CAnimImage>(AnimationPath::builtin("ITMCLS"), fortIndex, 0, 105, 31);
	fastArmyPurchase = std::make_shared<LRClickableArea>(Rect(105, 31, 34, 34), [townId]()
	{
		std::vector<const CGTownInstance*> towns = LOCPLINT->cb->getTownsInfo(true);
		for(auto & town : towns)
		{
			if(town->id == townId)
				std::make_shared<CCastleBuildings>(town)->enterToTheQuickRecruitmentWindow();
		}
	});
	fastMarket = std::make_shared<LRClickableArea>(Rect(143, 31, 30, 34), []()
	{
		std::vector<const CGTownInstance*> towns = LOCPLINT->cb->getTownsInfo(true);
		for(auto & town : towns)
		{
			if(town->builtBuildings.count(BuildingID::MARKETPLACE))
			{
				GH.windows().createAndPushWindow<CMarketplaceWindow>(town, nullptr, nullptr, EMarketMode::RESOURCE_RESOURCE);
				return;
			}
		}
		LOCPLINT->showInfoDialog(CGI->generaltexth->translate("vcmi.adventureMap.noTownWithMarket"));
	});

	assert(townInfo.tType);

	size_t iconIndex = townInfo.tType->clientInfo.icons[townInfo.fortLevel > 0][townInfo.built >= CGI->settings()->getInteger(EGameSettings::TOWNS_BUILDINGS_PER_TURN_CAP)];

	build = std::make_shared<CAnimImage>(AnimationPath::builtin("itpt"), iconIndex, 0, 3, 2);
	title = std::make_shared<CLabel>(66, 2, FONT_SMALL, ETextAlignment::TOPLEFT, Colors::WHITE, townInfo.name);

	if(townInfo.details)
	{
		hall = std::make_shared<CAnimImage>(AnimationPath::builtin("ITMTLS"), townInfo.details->hallLevel, 0, 67, 31);
		fastTownHall = std::make_shared<LRClickableArea>(Rect(67, 31, 34, 34), [townId]()
		{
			std::vector<const CGTownInstance*> towns = LOCPLINT->cb->getTownsInfo(true);
			for(auto & town : towns)
			{
				if(town->id == townId)
					std::make_shared<CCastleBuildings>(town)->enterTownHall();
			}
		});

		if(townInfo.details->goldIncome)
		{
			income = std::make_shared<CLabel>(157, 58, FONT_TINY, ETextAlignment::CENTER, Colors::WHITE,
											  std::to_string(townInfo.details->goldIncome));
		}
		if(townInfo.details->garrisonedHero) //garrisoned hero icon
			garrisonedHero = std::make_shared<CPicture>(ImagePath::builtin("TOWNQKGH"), 149, 76);

		if(townInfo.details->customRes)//silo is built
		{
			if(townInfo.tType->primaryRes == EGameResID::WOOD_AND_ORE )// wood & ore
			{
				res1 = std::make_shared<CAnimImage>(AnimationPath::builtin("SMALRES"), GameResID(EGameResID::WOOD), 0, 7, 75);
				res2 = std::make_shared<CAnimImage>(AnimationPath::builtin("SMALRES"), GameResID(EGameResID::ORE), 0, 7, 88);
			}
			else
			{
				res1 = std::make_shared<CAnimImage>(AnimationPath::builtin("SMALRES"), townInfo.tType->primaryRes, 0, 7, 81);
			}
		}
	}
}

CreatureTooltip::CreatureTooltip(Point pos, const CGCreature * creature)
{
	OBJ_CONSTRUCTION_CAPTURING_ALL_NO_DISPOSE;

	auto creatureData = (*CGI->creh)[creature->stacks.begin()->second->getCreatureID()].get();
	creatureImage = std::make_shared<CAnimImage>(graphics->getAnimation(AnimationPath::builtin("TWCRPORT")), creatureData->getIconIndex());
	creatureImage->center(Point(parent->pos.x + parent->pos.w / 2, parent->pos.y + creatureImage->pos.h / 2 + 11));

	bool isHeroSelected = LOCPLINT->localState->getCurrentHero() != nullptr;
	std::string textContent = isHeroSelected
			? creature->getHoverText(LOCPLINT->localState->getCurrentHero())
			: creature->getHoverText(LOCPLINT->playerID);

	//TODO: window is bigger than OH3
	//TODO: vertical alignment does not match H3. Commented below example that matches H3 for creatures count but supports only 1 line:
	/*std::shared_ptr<CLabel> = std::make_shared<CLabel>(parent->pos.w / 2, 103,
			FONT_SMALL, ETextAlignment::CENTER, Colors::WHITE, creature->getHoverText(LOCPLINT->playerID));*/

	tooltipTextbox = std::make_shared<CTextBox>(textContent, Rect(15, 95, 230, 150), 0, FONT_SMALL, ETextAlignment::TOPCENTER, Colors::WHITE);
}

void MoraleLuckBox::set(const AFactionMember * node)
{
	OBJECT_CONSTRUCTION_CUSTOM_CAPTURING(255-DISPOSE);

	const int textId[] = {62, 88}; //eg %s \n\n\n {Current Luck Modifiers:}
	const int noneTxtId = 108; //Russian version uses same text for neutral morale\luck
	const int neutralDescr[] = {60, 86}; //eg {Neutral Morale} \n\n Neutral morale means your armies will neither be blessed with extra attacks or freeze in combat.
	const int componentType[] = {CComponent::luck, CComponent::morale};
	const int hoverTextBase[] = {7, 4};
	TConstBonusListPtr modifierList = std::make_shared<const BonusList>();
	bonusValue = 0;

	if(node)
		bonusValue = morale ? node->moraleValAndBonusList(modifierList) : node->luckValAndBonusList(modifierList);

	int mrlt = (bonusValue>0)-(bonusValue<0); //signum: -1 - bad luck / morale, 0 - neutral, 1 - good
	hoverText = CGI->generaltexth->heroscrn[hoverTextBase[morale] - mrlt];
	baseType = componentType[morale];
	text = CGI->generaltexth->arraytxt[textId[morale]];
	boost::algorithm::replace_first(text,"%s",CGI->generaltexth->arraytxt[neutralDescr[morale]-mrlt]);

	if (morale && node && (node->getBonusBearer()->hasBonusOfType(BonusType::UNDEAD)
			|| node->getBonusBearer()->hasBonusOfType(BonusType::NON_LIVING)))
	{
		text += CGI->generaltexth->arraytxt[113]; //unaffected by morale
		bonusValue = 0;
	}
	else if(morale && node && node->getBonusBearer()->hasBonusOfType(BonusType::NO_MORALE))
	{
		auto noMorale = node->getBonusBearer()->getBonus(Selector::type()(BonusType::NO_MORALE));
		text += "\n" + noMorale->Description();
		bonusValue = 0;
	}
	else if (!morale && node && node->getBonusBearer()->hasBonusOfType(BonusType::NO_LUCK))
	{
		auto noLuck = node->getBonusBearer()->getBonus(Selector::type()(BonusType::NO_LUCK));
		text += "\n" + noLuck->Description();
		bonusValue = 0;
	}
	else
	{
		std::string addInfo = "";
		for(auto & bonus : * modifierList)
		{
			if(bonus->val)
				addInfo += "\n" + bonus->Description();
		}
		text = addInfo.empty() 
			? text + CGI->generaltexth->arraytxt[noneTxtId] 
			: text + addInfo;
	}
	std::string imageName;
	if (small)
		imageName = morale ? "IMRL30": "ILCK30";
	else
		imageName = morale ? "IMRL42" : "ILCK42";

	image = std::make_shared<CAnimImage>(AnimationPath::builtin(imageName), bonusValue + 3);
	image->moveBy(Point(pos.w/2 - image->pos.w/2, pos.h/2 - image->pos.h/2));//center icon
}

MoraleLuckBox::MoraleLuckBox(bool Morale, const Rect &r, bool Small)
	: morale(Morale),
	small(Small)
{
	bonusValue = 0;
	pos = r + pos.topLeft();
	defActions = 255-DISPOSE;
}

CCreaturePic::CCreaturePic(int x, int y, const CCreature * cre, bool Big, bool Animated)
{
	OBJECT_CONSTRUCTION_CAPTURING(255-DISPOSE);
	pos.x+=x;
	pos.y+=y;

	auto faction = cre->getFaction();

	assert(CGI->townh->size() > faction);

	if(Big)
		bg = std::make_shared<CPicture>((*CGI->townh)[faction]->creatureBg130);
	else
		bg = std::make_shared<CPicture>((*CGI->townh)[faction]->creatureBg120);
	anim = std::make_shared<CCreatureAnim>(0, 0, cre->animDefName);
	anim->clipRect(cre->isDoubleWide()?170:150, 155, bg->pos.w, bg->pos.h);
	anim->startPreview(cre->hasBonusOfType(BonusType::SIEGE_WEAPON));

	amount = std::make_shared<CLabel>(bg->pos.w, bg->pos.h, FONT_MEDIUM, ETextAlignment::BOTTOMRIGHT, Colors::WHITE);

	pos.w = bg->pos.w;
	pos.h = bg->pos.h;
}

void CCreaturePic::show(Canvas & to)
{
	// redraw everything in a proper order
	bg->showAll(to);
	anim->show(to);
	amount->showAll(to);
}

void CCreaturePic::setAmount(int newAmount)
{
	if(newAmount != 0)
		amount->setText(std::to_string(newAmount));
	else
		amount->setText("");
}

TransparentFilledRectangle::TransparentFilledRectangle(Rect position, ColorRGBA color) :
	color(color), colorLine(ColorRGBA()), drawLine(false)
{
	pos = position + pos.topLeft();
}

TransparentFilledRectangle::TransparentFilledRectangle(Rect position, ColorRGBA color, ColorRGBA colorLine) :
	color(color), colorLine(colorLine), drawLine(true)
{
	pos = position + pos.topLeft();
}

void TransparentFilledRectangle::showAll(Canvas & to) 
{
	to.drawColorBlended(pos, color);
	if(drawLine)
		to.drawBorder(pos, colorLine);
}

SimpleLine::SimpleLine(Point pos1, Point pos2, ColorRGBA color) :
	pos1(pos1), pos2(pos2), color(color)
{}

void SimpleLine::showAll(Canvas & to) 
{
	to.drawLine(pos1 + pos.topLeft(), pos2 + pos.topLeft(), color, color);
}
