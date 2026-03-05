/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "common/scummsys.h"
#include "common/config-manager.h"
#include "common/events.h"
#include "common/file.h"
#include "common/system.h"
#include "engines/util.h"
#include "graphics/cursorman.h"
#include "graphics/framelimiter.h"
#include "graphics/wincursor.h"
#include "image/icocur.h"

#include "yaga/yaga.h"

#include "yaga/animation.h"
#include "yaga/audiofile.h"
#include "yaga/console.h"
#include "yaga/detection.h"
#include "yaga/scripthandler.h"
#include "yaga/text.h"
#include "yaga/video.h"

namespace Yaga {

YagaEngine *g_engine;

YagaEngine::YagaEngine(OSystem *syst, const ADGameDescription *gameDesc) : Engine(syst),
	_gameDescription(gameDesc), _randomSource("Yaga") {
	g_engine = this;

	const Common::FSNode gameDataDir(ConfMan.getPath("path"));
	SearchMan.addSubDirectoryMatching(gameDataDir, "MaxFiles");
	SearchMan.addSubDirectoryMatching(gameDataDir, "Pajama Sam LRS", 0, 4);
	SearchMan.addSubDirectoryMatching(gameDataDir, "Pajama Sam LRS/interface", 0, 4);
	SearchMan.addSubDirectoryMatching(gameDataDir, "Pajama Sam LRS/interface/cursors", 0, 4);
	SearchMan.addSubDirectoryMatching(gameDataDir, "Pajama Sam LRS/movies", 0, 4);
	SearchMan.addSubDirectoryMatching(gameDataDir, "Putt-Putt PBS", 0, 4);
}

YagaEngine::~YagaEngine() {
	delete _screen;
}

uint32 YagaEngine::getFeatures() const {
	return _gameDescription->flags;
}

Common::String YagaEngine::getGameId() const {
	return _gameDescription->gameId;
}

Common::Error YagaEngine::run() {
	//Graphics::PixelFormat outputPixelFormat = Graphics::PixelFormat(2, 5, 5, 5, 0, 10, 5, 0, 0);
	Graphics::PixelFormat outputPixelFormat = Graphics::PixelFormat::createFormatRGBA32();

	Common::List<Graphics::PixelFormat> tryModes = _system->getSupportedFormats();
	for (Common::List<Graphics::PixelFormat>::iterator g = tryModes.begin(); g != tryModes.end(); ++g) {
		if (g->bytesPerPixel != 2 || g->aBits()) {
			g = tryModes.reverse_erase(g);
		} else if (*g == outputPixelFormat) {
			tryModes.clear();
			tryModes.push_back(outputPixelFormat);
			break;
		}
	}

	initGraphics(640, 480, tryModes);
	if (_system->getScreenFormat().bytesPerPixel != 2)
		return Common::kUnsupportedColorMode;

	_screen = new Graphics::Screen();

	// Set the engine's debugger console
	setDebugger(new Console());

	// If a savegame was selected from the launcher, load it
	int saveSlot = ConfMan.getInt("save_slot");
	if (saveSlot != -1)
		(void)loadGameState(saveSlot);

	Common::Event e;

	// Test: Load a cursor
	changeCursor("hw_cursorHighlight");
	CursorMan.showMouse(true);

	// Test: Show a background
	Animation *roomResource = new Animation("rooms.he");
	roomResource->load("aquarium/bg_aquarium.mng");
	Graphics::Surface *background = roomResource->getImage();

	uint currentFrame1 = 0, currentFrame2 = 0;

	// Test: Play an MNG animation
	roomResource->load("aquarium/animation/sam/aqu_sam_kickfeet.mng");

	// Test: Play an RLE animation
	roomResource->load("aquarium/animation/sam/aqu_en.rle");

	// Test: Load a font and show text
	YagaText *text = new YagaText("menus.he");
	text->loadFont("fonts/arial_10_black_outline.mng");

	// Test: Play a video
	//VideoFile video;
	//video.play("agi_soap_sock.da2");

	// Test: Play speech, SFX and music
	AudioFile *talkies = new AudioFile("talkies.he", Audio::Mixer::kSpeechSoundType, _mixer);
	talkies->play("announcer/pj4pc_announcer_00001.mp3");
	AudioFile *clickPoints = new AudioFile("clickpoints.he", Audio::Mixer::kSFXSoundType, _mixer);
	clickPoints->play("22k/rooms/agitator_lake/agi_bat1.wav");
	AudioFile *music = new AudioFile("music.he", Audio::Mixer::kMusicSoundType, _mixer);
	music->play("agi_cleansock.mp3");

	// Test: Read Python scripts from EXE
	ScriptHandler *handler = new ScriptHandler("PajamaLRS.exe");
	delete handler;

	Graphics::FrameLimiter limiter(g_system, 60);
	uint elapsedFrames = 0;
	YagaLayer *animationLayer;

	while (!shouldQuit()) {
		while (g_system->getEventManager()->pollEvent(e)) {
			if (e.type == Common::EVENT_KEYUP) {
				if (e.kbd.keycode == Common::KEYCODE_ESCAPE)
					g_engine->quitGame();
			}
		}

		_screen->blitFrom(*background);

		// Draw MNG animation
		animationLayer = roomResource->getLayer(1, currentFrame1, 0);
		_screen->transBlitFrom(animationLayer->surface, animationLayer->pos, 0);

		// Draw RLE animation
		animationLayer = roomResource->getLayer(2, currentFrame2, 0);
		_screen->transBlitFrom(animationLayer->surface, animationLayer->pos, 0);

		if (elapsedFrames >= 10) {
			currentFrame1++;
			if (currentFrame1 >= roomResource->getFrameCount(1))
				currentFrame1 = 0;
			currentFrame2++;
			if (currentFrame2 >= roomResource->getFrameCount(2))
				currentFrame2 = 0;
			elapsedFrames = 0;
		}
		elapsedFrames++;

		// Draw font test
		text->drawText("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG", 40, 15);
		text->drawText("The quick brown fox jumps over the lazy dog", 40, 30);

		// Delay for a bit. All events loops should have a delay
		// to prevent the system being unduly loaded
		limiter.delayBeforeSwap();
		_screen->update();
		limiter.startFrame();
	}

	delete roomResource;
	delete text;

	delete music;
	delete clickPoints;
	delete talkies;

	return Common::kNoError;
}

Common::Error YagaEngine::syncGame(Common::Serializer &s) {
	// The Serializer has methods isLoading() and isSaving()
	// if you need to specific steps; for example setting
	// an array size after reading it's length, whereas
	// for saving it would write the existing array's length
	int dummy = 0;
	s.syncAsUint32LE(dummy);

	return Common::kNoError;
}

void YagaEngine::changeCursor(const Common::String &cursorName) {
	Common::File cursorFile;
	cursorFile.open(Common::Path("interface/cursors/" + cursorName + ".cur"));
	Image::IcoCurDecoder cursorDecoder;
	cursorDecoder.open(cursorFile);
	Graphics::Cursor *cursor = cursorDecoder.loadItemAsCursor(0);
	CursorMan.replaceCursor(cursor);
	delete cursor;
	cursorDecoder.close();
	cursorFile.close();
}

} // End of namespace Yaga
