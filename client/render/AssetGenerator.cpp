/*
 * AssetGenerator.cpp, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */
#include "StdInc.h"
#include "AssetGenerator.h"

#include "../gui/CGuiHandler.h"
#include "../render/IImage.h"
#include "../render/IImageLoader.h"
#include "../render/Canvas.h"
#include "../render/ColorFilter.h"
#include "../render/IRenderHandler.h"

#include "../lib/filesystem/Filesystem.h"

void AssetGenerator::generateAll()
{
	createBigSpellBook();
	createAdventureOptionsCleanBackground();
	for (int i = 0; i < PlayerColor::PLAYER_LIMIT_I; ++i)
		createPlayerColoredBackground(PlayerColor(i));
}

void AssetGenerator::createAdventureOptionsCleanBackground()
{
	std::string filename = "data/AdventureOptionsBackgroundClear.png";

	if(CResourceHandler::get()->existsResource(ResourcePath(filename))) // overridden by mod, no generation
		return;

	if(!CResourceHandler::get("local")->createResource(filename))
		return;
	ResourcePath savePath(filename, EResType::IMAGE);

	auto locator = ImageLocator(ImagePath::builtin("ADVOPTBK"));
	locator.scalingFactor = 1;

	std::shared_ptr<IImage> img = GH.renderHandler().loadImage(locator, EImageBlitMode::OPAQUE);

	Canvas canvas = Canvas(Point(575, 585), CanvasScalingPolicy::IGNORE);
	canvas.draw(img, Point(0, 0), Rect(0, 0, 575, 585));
	canvas.draw(img, Point(54, 121), Rect(54, 123, 335, 1));
	canvas.draw(img, Point(158, 84), Rect(156, 84, 2, 37));
	canvas.draw(img, Point(234, 84), Rect(232, 84, 2, 37));
	canvas.draw(img, Point(310, 84), Rect(308, 84, 2, 37));
	canvas.draw(img, Point(53, 567), Rect(53, 520, 339, 3));
	canvas.draw(img, Point(53, 520), Rect(53, 264, 339, 47));

	std::shared_ptr<IImage> image = GH.renderHandler().createImage(canvas.getInternalSurface());

	image->exportBitmap(*CResourceHandler::get("local")->getResourceName(savePath));
}

void AssetGenerator::createBigSpellBook()
{
	std::string filename = "data/SpellBookLarge.png";

	if(CResourceHandler::get()->existsResource(ResourcePath(filename))) // overridden by mod, no generation
		return;

	if(!CResourceHandler::get("local")->createResource(filename))
		return;
	ResourcePath savePath(filename, EResType::IMAGE);

	auto locator = ImageLocator(ImagePath::builtin("SpelBack"));
	locator.scalingFactor = 1;

	std::shared_ptr<IImage> img = GH.renderHandler().loadImage(locator, EImageBlitMode::OPAQUE);
	Canvas canvas = Canvas(Point(800, 600), CanvasScalingPolicy::IGNORE);
	// edges
	canvas.draw(img, Point(0, 0), Rect(15, 38, 90, 45));
	canvas.draw(img, Point(0, 460), Rect(15, 400, 90, 141));
	canvas.draw(img, Point(705, 0), Rect(509, 38, 95, 45));
	canvas.draw(img, Point(705, 460), Rect(509, 400, 95, 141));
	// left / right
	Canvas tmp1 = Canvas(Point(90, 355 - 45), CanvasScalingPolicy::IGNORE);
	tmp1.draw(img, Point(0, 0), Rect(15, 38 + 45, 90, 355 - 45));
	canvas.drawScaled(tmp1, Point(0, 45), Point(90, 415));
	Canvas tmp2 = Canvas(Point(95, 355 - 45), CanvasScalingPolicy::IGNORE);
	tmp2.draw(img, Point(0, 0), Rect(509, 38 + 45, 95, 355 - 45));
	canvas.drawScaled(tmp2, Point(705, 45), Point(95, 415));
	// top / bottom
	Canvas tmp3 = Canvas(Point(409, 45), CanvasScalingPolicy::IGNORE);
	tmp3.draw(img, Point(0, 0), Rect(100, 38, 409, 45));
	canvas.drawScaled(tmp3, Point(90, 0), Point(615, 45));
	Canvas tmp4 = Canvas(Point(409, 141), CanvasScalingPolicy::IGNORE);
	tmp4.draw(img, Point(0, 0), Rect(100, 400, 409, 141));
	canvas.drawScaled(tmp4, Point(90, 460), Point(615, 141));
	// middle
	Canvas tmp5 = Canvas(Point(409, 141), CanvasScalingPolicy::IGNORE);
	tmp5.draw(img, Point(0, 0), Rect(100, 38 + 45, 509 - 15, 400 - 38));
	canvas.drawScaled(tmp5, Point(90, 45), Point(615, 415));
	// carpet
	Canvas tmp6 = Canvas(Point(590, 59), CanvasScalingPolicy::IGNORE);
	tmp6.draw(img, Point(0, 0), Rect(15, 484, 590, 59));
	canvas.drawScaled(tmp6, Point(0, 545), Point(800, 59));
	// remove bookmarks
	for (int i = 0; i < 56; i++)
		canvas.draw(Canvas(canvas, Rect(i < 30 ? 268 : 327, 464, 1, 46)), Point(269 + i, 464));
	for (int i = 0; i < 56; i++)
		canvas.draw(Canvas(canvas, Rect(469, 464, 1, 42)), Point(470 + i, 464));
	for (int i = 0; i < 57; i++)
		canvas.draw(Canvas(canvas, Rect(i < 30 ? 564 : 630, 464, 1, 44)), Point(565 + i, 464));
	for (int i = 0; i < 56; i++)
		canvas.draw(Canvas(canvas, Rect(656, 464, 1, 47)), Point(657 + i, 464));
	// draw bookmarks
	canvas.draw(img, Point(278, 464), Rect(220, 405, 37, 47));
	canvas.draw(img, Point(481, 465), Rect(354, 406, 37, 41));
	canvas.draw(img, Point(575, 465), Rect(417, 406, 37, 45));
	canvas.draw(img, Point(667, 465), Rect(478, 406, 37, 47));

	std::shared_ptr<IImage> image = GH.renderHandler().createImage(canvas.getInternalSurface());

	image->exportBitmap(*CResourceHandler::get("local")->getResourceName(savePath));
}

void AssetGenerator::createPlayerColoredBackground(const PlayerColor & player)
{
	std::string filename = "data/DialogBoxBackground_" + player.toString() + ".png";

	if(CResourceHandler::get()->existsResource(ResourcePath(filename))) // overridden by mod, no generation
		return;

	if(!CResourceHandler::get("local")->createResource(filename))
		return;

	ResourcePath savePath(filename, EResType::IMAGE);

	auto locator = ImageLocator(ImagePath::builtin("DiBoxBck"));
	locator.scalingFactor = 1;

	std::shared_ptr<IImage> texture = GH.renderHandler().loadImage(locator, EImageBlitMode::OPAQUE);

	// Color transform to make color of brown DIBOX.PCX texture match color of specified player
	static const std::array<ColorFilter, PlayerColor::PLAYER_LIMIT_I> filters = {
		ColorFilter::genRangeShifter(  0.25,  0,     0,     1.25, 0.00, 0.00 ), // red
		ColorFilter::genRangeShifter(  0,     0,     0,     0.45, 1.20, 4.50 ), // blue
		ColorFilter::genRangeShifter(  0.40,  0.27,  0.23,  1.10, 1.20, 1.15 ), // tan
		ColorFilter::genRangeShifter( -0.27,  0.10, -0.27,  0.70, 1.70, 0.70 ), // green
		ColorFilter::genRangeShifter(  0.47,  0.17, -0.27,  1.60, 1.20, 0.70 ), // orange
		ColorFilter::genRangeShifter(  0.12, -0.1,   0.25,  1.15, 1.20, 2.20 ), // purple
		ColorFilter::genRangeShifter( -0.13,  0.23,  0.23,  0.90, 1.20, 2.20 ), // teal
		ColorFilter::genRangeShifter(  0.44,  0.15,  0.25,  1.00, 1.00, 1.75 )  // pink
	};

	assert(player.isValidPlayer());
	if (!player.isValidPlayer())
	{
		logGlobal->error("Unable to colorize to invalid player color %d!", static_cast<int>(player.getNum()));
		return;
	}

	texture->adjustPalette(filters[player.getNum()], 0);
	texture->exportBitmap(*CResourceHandler::get("local")->getResourceName(savePath));
}
