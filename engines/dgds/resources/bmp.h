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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/bmp.h $
 * $Id: bmp.h 19 2010-01-24 00:40:01Z alexbevi $
 *
 */

#ifndef DGDS_BMP_H
#define DGDS_BMP_H

#include "dgds/resource.h"

namespace Dgds {

class Bmp {
public:
	Bmp(Resource *res);
	~Bmp();

private:
	void empty();

	bool loadINF(Resource *resINF);
	bool loadOFF(Resource *resOFF);
	bool loadVQT(Resource *resVQT);

	uint16 _numInf;
	uint16 *_dataInf;

	uint16 _numOff;
	uint32 *_dataOff;

	Resource *_res;
};

} // End of namespace Dgds

#endif // DGDS_BMP_H
