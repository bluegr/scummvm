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

#include "common/system.h"
#include "graphics/surface.h"
#include "yaga/animation.h"
#include "yaga/text.h"

namespace Yaga {

YagaText::YagaText(const Common::String &filename) {
	_fontResource = new Animation(filename);
}

YagaText::~YagaText() {
	unloadFont();
	delete _fontResource;
}

void YagaText::loadFont(const Common::String &filePath) {
	unloadFont();

	_fontResource->load(filePath);
	_font = _fontResource->getImage();
	calculateFontWidths();
}

void YagaText::unloadFont() {
	_fontWidths.clear();
	_fontX.clear();

	//if (_font) {
	//	_font->free();
	//	delete _font;
	//	_font = nullptr;
	//}
}

void YagaText::calculateFontWidths() {
	bool waitingForChar = true;
	int16 startX = 0;
	byte a, r, g, b;

	_fontWidths.clear();
	_fontX.clear();

	for (int16 x = 0; x < _font->w; ++x) {
		bool transparentColumn = true;
		for (int16 y = 0; y < _font->h; ++y) {
			_font->format.colorToARGB(_font->getPixel(x, y), a, r, g, b);
			if (a > 0) {
				transparentColumn = false;
				break;
			}
		}

		if (!transparentColumn) {
			if (waitingForChar) {
				// Start of a new character
				startX = x;
				waitingForChar = false;
			}
		} else {
			if (!waitingForChar) {
				// End of the current character
				_fontWidths.push_back(x - startX);
				_fontX.push_back(startX);
				waitingForChar = true;
			}
		}
	}

	assert(_fontWidths.size() == kMaxChar - kMinChar);
}

void YagaText::drawText(const Common::String &text, uint16 x, uint16 y) {
	byte a, r, g, b;
	uint16 curX = 0;

	Graphics::Surface *screen = g_system->lockScreen();
	Graphics::PixelFormat screenFormat = screen->format;

	for (uint i = 0; i < text.size(); ++i) {
		char c = text[i];
		if (c <= ' ') {
			curX += _fontWidths['A' - kMinChar];
			continue;
		}

		uint16 charWidth = _fontWidths[c - kMinChar];
		uint16 charX = _fontX[c - kMinChar];

		for (int16 fontY = 0; fontY < _font->h; ++fontY) {
			for (int16 fontX = 0; fontX < charWidth; ++fontX) {
				uint32 pixel = _font->getPixel(charX + fontX, fontY);
				_font->format.colorToARGB(pixel, a, r, g, b);
				uint32 outPixel = screenFormat.ARGBToColor(a, r, g, b);
				if (a > 0) {
					screen->setPixel(x + fontX + curX, y + fontY, outPixel);
				}
			}
		}

		curX += charWidth;
	}

	g_system->unlockScreen();
}

} // End of namespace Yaga
