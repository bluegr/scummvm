/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef DUNE_H
#define DUNE_H

#include "dune/archive.h"
#include "dune/graphics.h"
#include "dune/sound/music.h"
#include "dune/sound/sound.h"
#include "dune/video.h"

#include "common/random.h"
#include "engines/advancedDetector.h"
#include "engines/engine.h"
#include "gui/debugger.h"

namespace Dune {
class HnmPlayer;
class MidiMusic;
class SoundPlayer;

enum DuneActions {
	kDuneActionSkipCutscene,
};


class DuneEngine : public Engine {
public:
	DuneEngine(OSystem *syst, const ADGameDescription *desc);
	~DuneEngine() override;

	Common::Error run() override;

	int _soundVolume;
	int _speechVolume;
	bool isCD();

	int _timerTicks;
	Graphics _graphics;
	Common::SeekableReadStream *openMember(const char *filename);
	OSystem *getSystem() { return _system; }

private:
	Common::RandomSource *_rnd;
	const ADGameDescription *_gameDescription;

	Archive _archive;
	void dumpResource(const char *filename);
	HnmPlayer *_video;
	MidiMusic *_music;
	SoundPlayer *_sound;
	void stopMusic();
};
} // End of namespace Dune

#endif
