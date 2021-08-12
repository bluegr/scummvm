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

#include "dune/dune.h"
#include "dune/hsq.h"
#include "dune/intro.h"

#include "common/config-manager.h"
#include "common/debug-channels.h"
#include "common/debug.h"
#include "common/file.h"
#include "common/fs.h"
#include "common/scummsys.h"
#include "engines/util.h"
#include "graphics/palette.h"
#include "gui/EventRecorder.h"

namespace Dune {
DuneEngine::DuneEngine(OSystem *syst, const ADGameDescription *gameDesc)
	: Engine(syst) {
	_timerTicks = 0;
	_rnd = new Common::RandomSource("dune_randomseed");
}

DuneEngine::~DuneEngine() {
	delete _rnd;
	DebugMan.removeAllDebugChannels();
}

Common::Error DuneEngine::run() {
	initGraphics(320, 200);

	if (!_archive.openArchive("DUNE.DAT")) {
		debug("Failed to open DUNE.DAT");
	}

	byte pal[3 * 256] = {0};
	for (int i = 0; i != 256; ++i) {
		pal[3 * i + 0] = pal[3 * i + 1] = pal[3 * i + 2] = i;
	}
	_system->getPaletteManager()->setPalette(pal, 0, 255);

	Intro *intro = new Intro(this);
	intro->runIntro();

	return Common::kNoError;
}

bool DuneEngine::isCD() {
	return _gameDescription->flags & ADGF_CD;
}

void DuneEngine::dumpResource(const char *filename) {
	Common::SeekableReadStream *r = _archive.openMember(filename);
	Common::DumpFile f;
	f.open(filename);

	int size = r->size();
	byte *buf = new byte[size];
	r->read(buf, size);

	f.write(buf, size);
	r->seek(0);
	delete[] buf;
}

Common::SeekableReadStream *DuneEngine::openMember(const char *filename) {
	return _archive.openMember(filename);
}

} // End of namespace Dune
