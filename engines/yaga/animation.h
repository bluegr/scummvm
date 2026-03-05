
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

#ifndef YAGA_ANIMATION_H
#define YAGA_ANIMATION_H

#include "common/archive.h"
#include "common/rect.h"
#include "common/str.h"
#include "graphics/surface.h"

namespace Yaga {

// Animations have frames, frames are made of layers
struct YagaLayer {
	Common::Point pos;
	int mask;
	Graphics::Surface surface;
};

struct YagaFrame {
	Common::Array<YagaLayer> layers;
};

struct YagaAnimation {
	Common::Array<YagaFrame> frames;
};

class Animation {
public:
	Animation(const Common::String &filename);
	~Animation();
	void load(const Common::String &filePath);
	const byte *getPalette() { return _palette; }
	YagaLayer *getLayer(uint animationIndex, uint frameIndex, uint layerIndex);
	Graphics::Surface *getImage();
	uint getAnimationCount() { return _animations.size(); }
	uint getFrameCount(uint animationIndex) {
		if (animationIndex >= _animations.size())
			return 0;
		return _animations[animationIndex].frames.size();
	}

private:
	Common::Archive *_archive;
	Common::Array<YagaAnimation> _animations;
	byte _palette[256 * 4]; // RGBA palette
	byte _colorType = 0;

	Graphics::PixelFormat getPixelFormatFromColorType(byte colorType);

	void loadMng(const Common::String &filePath);
	void loadRle(const Common::String &filePath);
	void decodeRleLayer(Common::SeekableReadStream *animStream, YagaLayer *layer);
};

} // End of namespace Yaga

#endif // YAGA_ANIMATION_H
