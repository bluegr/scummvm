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

#ifndef KYRA_TEXT_V3_H
#define KYRA_TEXT_V3_H

#include "kyra/text.h"

#include "kyra/kyra_v3.h"

namespace Kyra {

class TextDisplayer_v3 : public TextDisplayer {
public:
	TextDisplayer_v3(KyraEngine_v3 *vm, Screen_v3 *screen);

	void printText(const char *str, int x, int y, uint8 c0, uint8 c1, uint8 c2, Screen::FontId font=Screen::FID_8_FNT);
protected:
	KyraEngine_v3 *_vm;
	Screen_v3 *_screen;
};

} // end of namespace Kyra

#endif
