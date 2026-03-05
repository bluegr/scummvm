
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

#ifndef YAGA_TEXT_H
#define YAGA_TEXT_H

#include "common/str.h"
#include "yaga/animation.h"

namespace Yaga {

class YagaText {
public:
	YagaText(const Common::String &filename);
	~YagaText();
	void loadFont(const Common::String &filePath);
	void unloadFont();
	void drawText(const Common::String &text, uint16 x, uint16 y);

private:
	Animation *_fontResource = nullptr;
	Graphics::Surface *_font = nullptr;
	Common::Array<uint16> _fontWidths;
	Common::Array<uint16> _fontX;

	const char kMinChar = '!';
	const char kMaxChar = '~';

	void calculateFontWidths();
};

} // End of namespace Yaga

#endif // YAGA_TEXT_H
