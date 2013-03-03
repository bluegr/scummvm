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

#include "common/hashmap.h"


#include "common/file.h"
#include "graphics/surface.h"

#include "interspective/resources.h"
#include "interspective/innocent.h"
#include "interspective/inter.h"
#include "interspective/logic.h"
#include "interspective/main_dat.h"
#include "interspective/mapfile.h"
#include "interspective/prog_dat.h"
#include "interspective/program.h"
#include "interspective/sprite.h"

using namespace Common;
using namespace std;

namespace Common {
	DECLARE_SINGLETON(Interspective::Resources);
}

namespace Interspective {
//

void Surface::blit(const Surface *s, Common::Rect r, int transparent, const byte (*tinted)[256]) {
	enum {
		kSemitransparent = 0xbe
	};

	const byte *src = reinterpret_cast<byte *>(s->pixels);
	byte *dest = reinterpret_cast<byte *>(getBasePtr(r.left, r.top));

	int rw = r.width(), rh = r.height();
	for (int y = 0; y < rh; ++y) {
		for (int x = 0; x < rw; ++x) {
			if (tinted && src[x] == kSemitransparent)
				dest[x] = (*tinted)[dest[x]];
			else if (src[x] != transparent)
				dest[x] = src[x];
		}
		src += s->pitch;
		dest += pitch;
	}
}

void Resources::setEngine(Engine *vm) {
	_main = Common::SharedPtr<MainDat>(new MainDat(this));
	_graphicsMap = Common::SharedPtr<MapFile>(new MapFile("iuc_graf.dat"));
	_tuneMap = Common::SharedPtr<MapFile>(new MapFile("iuc_tune.dat"));
	_progDat = Common::SharedPtr<ProgDat>(new ProgDat(this));
	_graphicFiles = 0;
	_vm = vm;
}

Resources::~Resources() {
	if (_graphicFiles) {
		delete[] _graphicFiles;

		for (int i = 0; i < kFrameNum; i++)
			delete _frames[i];
	}
	if (_musicFiles)
		delete[] _musicFiles;
}

void Resources::load() {
	_main.get()->load();
	_graphicsMap.get()->load();
	_tuneMap.get()->load();

	loadGraphicFiles();
	loadMusicFiles();

	_progDat.get()->load();

	loadFrames();
	loadSpeechBubbles();
}

void Resources::loadFrames() {
	#define FRAME(p) _frames[p] = loadSprite(_main.get()->getFrameId(p))
	FRAME(kFrameTopLeft);
	FRAME(kFrameTop);
	FRAME(kFrameTopRight);
	FRAME(kFrameLeft);
	FRAME(kFrameFill);
	FRAME(kFrameRight);
	FRAME(kFrameBottomLeft);
	FRAME(kFrameBottom);
	FRAME(kFrameBottomRight);
	#undef FRAME
}

void Resources::loadSpeechBubbles() {
	#define BUBBLE(p) _bubbles[p] = loadSprite(_main.get()->getBubbleId(p))
	BUBBLE(kBubbleTopLeft);
	BUBBLE(kBubbleLeft);
	BUBBLE(kBubbleBottomLeft);
	BUBBLE(kBubbleTop);
	BUBBLE(kBubbleFill);
	BUBBLE(kBubbleBottom);
	BUBBLE(kBubbleTopRight);
	BUBBLE(kBubbleRight);
	BUBBLE(kBubbleBottomRight);

	BUBBLE(kBubbleBottomLeftPoint);
	BUBBLE(kBubbleBottomRightPoint);
	BUBBLE(kBubbleTopLeftPoint);
	BUBBLE(kBubbleTopRightPoint);
	#undef BUBBLE
}

void Resources::init() {
	load();
}

void Resources::loadActors() {
	_main.get()->loadActors(_vm->logic()->mainInterpreter());
}

byte *Resources::getGlobalByteVariable(uint16 var) const {
	return _main.get()->getByteVariable(var);
}

byte *Resources::getGlobalWordVariable(uint16 var) const {
	return _main.get()->getWordVariable(var);
}

void Resources::loadGraphicFiles() {
	const Common::List<MainDat::GraphicFile> files(_main.get()->graphicFiles());

	_graphicFiles = new Common::SharedPtr<SeekableReadStream>[files.size()];

	Common::SharedPtr<SeekableReadStream> *ptr = _graphicFiles;
	for (Common::List<MainDat::GraphicFile>::const_iterator it = files.begin(); it != files.end(); ++it) {
		File *file = new File();
		file->open(String(it->filename));
		Common::SharedPtr<SeekableReadStream> pointer(file);
		*(ptr++) = pointer;
	}
}

void Resources::loadMusicFiles() {
	const Common::List<Common::String> files(_main.get()->musicFiles());

	_musicFiles = new Common::SharedPtr<SeekableReadStream>[files.size()];

	Common::SharedPtr<SeekableReadStream> *ptr = _musicFiles;
	for (Common::List<Common::String>::const_iterator it = files.begin(); it != files.end(); ++it) {
		debugC(1, kDebugLevelFiles | kDebugLevelMusic, "opening music file %s", it->c_str());
		File *file = new File();
		file->open(*it);
		Common::SharedPtr<SeekableReadStream> pointer(file);
		*(ptr++) = pointer;
	}
}

Common::ReadStream *Resources::imageStream(uint16 index) const {
	uint16 file_index = _main.get()->fileIndexOfImage(index);
	uint32 offset = _graphicsMap.get()->offsetOfEntry(index);

	SeekableReadStream *file = _graphicFiles[file_index].get();
	file->seek(offset);

	return file;
}

Common::ReadStream *Resources::tuneStream(uint16 index) const {
	uint16 file_index = _main.get()->fileIndexOfTune(index)/2;
	uint32 offset = _tuneMap.get()->offsetOfEntry(index);

	debugC(2, kDebugLevelFiles | kDebugLevelMusic, "loading tune %d from file %d at offset 0x%x", index, file_index, offset);

	SeekableReadStream *file = _musicFiles[file_index].get();
	file->seek(offset);

	return file;
}

void Resources::readPalette(Common::ReadStream *stream, byte *palette) {
	for(int i = 256; i; i--) { // convert to rgba
		stream->read(palette, 3);
		palette[3] = 0;
		palette += 4;
	}
}

void Resources::loadImage(uint16 index, byte *target, uint16 size, byte *palette) const {
	Common::ReadStream *file = imageStream(index);
	(void) file->readUint16LE();
	(void) file->readUint16LE(); // we know size alright

	decodeImage(file, target, size);

	if (!palette)
		return;

	file->readByte(); // skip zero

	readPalette(file, palette);
}

Image *Resources::loadImage(uint16 index) const {
	Image * img;
	static Common::HashMap<uint16, Image *> cache;

	if ((img = cache[index]))
		return img;

	img = new Image;
	img->create(320, 200);
	assert(img->pitch == 320);
	loadImage(index, reinterpret_cast<byte *>(img->pixels), 320*200);
	cache[index] = img;
	return img;
}

void Resources::loadTune(uint16 index, byte *target) const {
	Common::ReadStream *file = tuneStream(index);
	file->read(target, 6000);
}

void Resources::decodeImage(Common::ReadStream *stream, byte *target, uint16 size) {
	enum {
		kRunFlag = 0xc0
	};

	while (size) {
		byte color = stream->readByte();

		uint8 runLength = 1;
		if ((color & kRunFlag) == kRunFlag) {
			runLength = color & (~kRunFlag);
			color = stream->readByte();
		}

		for (; runLength; runLength--) {
			*(target++) = color;
			if (!--size)
				return;
		}
	}
}

uint16 Resources::blockOfRoom(uint16 room) const {
	return _main.get()->getRoomScriptId(room);
}

Program *Resources::loadCodeBlock(uint16 block) const {
	return _progDat.get()->getScript(block);
}

void Resources::descramble(byte *data, uint32 len) {
	for (uint32 i = 0; i < len; i++)
		data[i] ^= 0x6f;
}

byte *Resources::mainBase() const {
	return _main.get()->_data;
}

uint16 Resources::mainEntryPoint() const {
	return _main.get()->getEntryPoint() - mainBase();
}

Surface *Resources::loadBackdrop(uint16 index, byte *palette) {
	Common::ReadStream *stream = imageStream(index);

	uint16 width = stream->readUint16LE();
	uint16 height = stream->readUint16LE();


	Surface *backdrop = new Surface;
	backdrop->create(width, height);

	decodeImage(stream, reinterpret_cast<byte *>(backdrop->pixels), width * height);

	stream->readByte(); // skip zero

	readPalette(stream, palette);

	return backdrop;
}

Sprite *Resources::getGlyph(byte ch) const {
	if (ch <= ' ' || ch > '~')
		return 0;
	uint16 id = _main.get()->getGlyphSpriteId(ch);
	Sprite *s = loadSprite(id);
	return s;
}

Sprite *Resources::loadSprite(uint16 id) const {
	debugC(4, kDebugLevelFiles, "loading sprite %d", id);
	SpriteInfo info = getSpriteInfo(id);
	Image *image = loadImage(info.image);
	Sprite *sprite = image->cut(Common::Rect(info.left, info.top, info.left + info.width, info.top + info.height));
	sprite->_hotPoint = Common::Point(info.hotLeft, info.hotTop);
	return sprite;
}

SpriteInfo Resources::getSpriteInfo(uint16 id) const {
	if (id < _main.get()->spriteCount())
		return _main.get()->getSpriteInfo(id);
	else
		return _vm->logic()->blockProgram()->getSpriteInfo(id - _main.get()->spriteCount());
}

Sprite *Image::cut(Common::Rect rect) const {
	Sprite *sprite = new Sprite;
	sprite->create(rect.width(), rect.height());

	const byte *src = reinterpret_cast<const byte *>(getBasePtr(rect.left, rect.top));
	byte *dest = reinterpret_cast<byte *>(sprite->pixels);
	for (uint16 y = 0; y < rect.height(); y++) {
		Common::copy(src, src + rect.width(), dest);
		src += pitch;
		dest += sprite->pitch;
	}
	return sprite;
}

enum {
	kChangeableColour = 235
};

void Sprite::recolour(byte colour) {
	// TODO: std::replace
	//byte *data = reinterpret_cast<byte *>(pixels);
	//std::replace(data, data + h * pitch, byte(kChangeableColour), colour);
}

template<>
Common::SharedPtr<Interspective::Sprite> &CodePointer::field<Common::SharedPtr<Interspective::Sprite> >(Common::SharedPtr<Interspective::Sprite> &p, int off) const {
	uint16 sprite;
	field(sprite, off);
	p = Common::SharedPtr<Interspective::Sprite>(_interpreter->resources()->loadSprite(sprite));
	return p;
}

Sprite *Resources::getCursor() const {
	return loadSprite(_main.get()->getCursorSpriteId());
}

} // End of namespace Interspective
