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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "dgds/resources/movie.h"
#include "dgds/compression.h"

namespace Dgds {

Movie::Movie(Resource *res):
	_version(""), _pages(0), _chunks() {

	Resource *resPAG = res->getSubResource("PAG:");
	Resource *resTT3 = res->getSubResource("TT3:");
	Resource *resVER = res->getSubResource("VER:");

	loadVER(resVER);
	loadPAG(resPAG);
	loadTT3(resTT3);

	delete resPAG;
	delete resTT3;
	delete resVER;
}

Movie::~Movie() {
	for (uint32 i = 0; i < _chunks.size(); i++) {
		_chunks[i]->data.clear();
		delete _chunks[i];
	}
	_chunks.clear();
}

void Movie::loadVER(Resource *res) {
	res->seek(0);
	_version = res->readLine();
	debugC(1, "[%s] version: %s",res->getName(), _version.c_str());
}

void Movie::loadPAG(Resource *res) {
	res->seek(0);
	_pages = res->readUint16LE();
	debugC(1, "[%s] pages: %d",res->getName(), _pages);
}

void Movie::loadTT3(Resource *res) {
	res->seek(0);
	byte comprMethod = res->readByte();
	int32 streamSize = res->readUint32LE();
	assert (comprMethod == 2);

	Common::SeekableReadStream *decomp = decompLZW(res, streamSize);

	while (!decomp->eos()) {
		MovieChunk *mc = new MovieChunk;
		uint16 code = decomp->readUint16LE();
		uint32 size = code & 0x000F;
		code &= 0xFFF0;
		mc->code = code;
		if ((code == 0x1110) && (size == 1)) {
			uint16 id = decomp->readUint16LE();
			mc->data.push_back(id);
			// XXX if there were a TAG sub resource we could pull
			// a filename by index from the tag buffer ... but there
			// isn't always a tag buffer.
		} else if (size == 15) {
			byte c;
			do {
				c = decomp->readByte();
				mc->name += c;
			} while (c != 0x00);
			if ((decomp->size() - decomp->pos()) & 1)
				decomp->skip(1);
		} else {
			for (uint32 i = 0; i < size; i++)
				mc->data.push_back(decomp->readSint16LE());
		}
		_chunks.push_back(mc);
	}

	delete decomp;
}

void Movie::play() {
	for (uint32 i = 0; i < _chunks.size(); i++) {
		debugN("%4d %04x %-39s: ", i, _chunks[i]->code, _chunks[i]->name.c_str() );

		for (uint32 j = 0; j < _chunks[i]->data.size(); j++)
			debugN(" %4d", _chunks[i]->data[j]);

		for (uint32 j = _chunks[i]->data.size(); j < 8; j++)
			debugN("     ");

		switch (_chunks[i]->code) {
		case kChunkSaveBackground:
			debugN(" save background");
			break;
		case kChunkDrawBackground:
			debugN(" draw background");
			break;
		case kChunkPurgeSavedImages:
			debugN(" purge saved images");
			break;
		case kChunkUpdate:
			debugN(" update");
			break;
		case kChunkDelay:
			debugN(" delay (delay)");
			break;
		case kChunkSelectImage:
			debugN(" select image (image)");
			break;
		case kChunkSelectPalette:
			debugN(" select palette (palette)");
			break;
		case kChunkSetScene:
			debugN(" set scene (scene)");
			break;
		case kChunkSetFrame_1:
			debugN(" set frame (?, frame)");
			break;
		case kChunkSetFrame_2:
			debugN(" set frame (?, frame)");
			break;
		case kChunkSetWindow_1:
			debugN(" set window (x, y, w, h)");
			break;
		case kChunkFadeOut:
			debugN(" fade out (first, n, steps, delay)");
			break;
		case kChunkFadeIn:
			debugN(" fade in (first, n, steps, delay)");
			break;
		case kChunkSaveImage_1:
			debugN(" save image (x, y, w, h)");
			break;
		case kChunkSaveImage_2:
			debugN(" save image (x, y, w, h)");
			break;
		case kChunkSetWindow_2:
			debugN(" set window (x, y, w, h)");
			break;
		case kChunkDrawSprite_1:
			debugN(" draw sprite (x, y, frame, image)");
			break;
		case kChunkDrawSprite_2:
			debugN(" draw sprite (x, y, frame, image)");
			break;
		case kChunkDrawSprite_3:
			debugN(" draw sprite (x, y, frame, image)");
			break;
		case kChunkDrawSprite_4:
			debugN(" draw sprite (x, y, frame, image)");
			break;
		case kChunkDrawScreen:
			debugN(" draw screen (x, y, w, h, ?, ?)");
			break;
		case kChunkLoadSound:
			debugN(" load sound resource");
			break;
		case kChunkSelectSound:
			debugN(" select sound (sound)");
			break;
		case kChunkDisableSound:
			debugN(" disable sound (sound)");
			break;
		case kChunkPlaySound:
			debugN(" play sound (sound)");
			break;
		case kChunkStopSound:
			debugN(" stop sound (sound)");
			break;
		case kChunkLoadScreen:
			debugN(" load screen resource");
			break;
		case kChunkLoadImage:
			debugN(" load image resource");
			break;
		case kChunkLoadPalette:
			debugN(" load palette resource");
			break;
		default:
			debugN(" unknown");
			break;
		}
		debug(" ");
	}
}

}
