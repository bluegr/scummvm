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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/palette.cpp $
 * $Id: palette.cpp 12 2010-01-23 12:01:58Z alexbevi $
 *
 */

#include "common/system.h"
#include "graphics/palette.h"

#include "dgds/resources/palette.h"

namespace Dgds {

Palette::Palette(Resource *res) :
	_res(res) {
	Resource *resPAL = res->getSubResource("PAL:");
	if (resPAL) {
		if (!loadVGA(resPAL->getSubResource("VGA:")))
			error("Error loading palette resource");
		delete resPAL;
	} else {
		error("The resource doesn't contain a palette");
	}
}

Palette::~Palette() {
	delete _res;
}

bool Palette::loadVGA(Resource *resVGA) {
	bool ret = false;

	if (resVGA) {
		if (resVGA->size() == 768) {
			byte shift = 2;
			for (int i = 0; i < 256 * 4; i += 4) {
				_palette[i + 0] = resVGA->readByte() << shift;
				_palette[i + 1] = resVGA->readByte() << shift;
				_palette[i + 2] = resVGA->readByte() << shift;
				_palette[i + 3] = 255;
			}
			ret = true;
		} else {
			warning("Bad size for a VGA palette: %d", resVGA->size());
		}
		delete resVGA;
	} else {
		warning("The resource doesn't contain a VGA palette");
	}

	return ret;
}

void Palette::apply() {
	g_system->getPaletteManager()->setPalette(_palette, 0, 256);
}

} // End of namespace Dgds
