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
 * $URL: http://scummvm-dgds.googlecode.com/svn/trunk/resources/bmp.cpp $
 * $Id: bmp.cpp 12 2010-01-23 12:01:58Z alexbevi $
 *
 */

#include "dgds/resources/bmp.h"
#include "dgds/compression.h"

namespace Dgds {

Bmp::Bmp(Resource *res) :
	_numInf(0), _dataInf(NULL), _numOff(0), _dataOff(NULL), _res(res) {

	empty();

	Resource *resBMP = res->getSubResource("BMP:");
	if (resBMP) {
		if (!loadINF(resBMP->getSubResource("INF:")))
			error("Error while loading BMP INF");
			
		if (!loadOFF(resBMP->getSubResource("OFF:")))
			error("Error while loading BMP offset");

		if (!loadVQT(resBMP->getSubResource("VQT:")))
			error("Error while loading BMP surface");

		delete resBMP;
	} else {
		error("The resource doesn't contain a bitmap");
	}
}

Bmp::~Bmp() {
	empty();
	delete _res;
}

void Bmp::empty() {
	_numInf = 0;
	if (_dataInf) {
		delete [] _dataInf;
		_dataInf = NULL;
	}

	_numOff = 0;
	if (_dataOff) {
		delete [] _dataOff;
		_dataOff = NULL;
	}
}

bool Bmp::loadINF(Resource *resINF) {
	bool ret = false;

	if (resINF) {
		_numInf = resINF->readUint16LE();
		_dataInf = new uint16[_numInf * 2];

		for (int i = 0; i < _numInf; i++)
			_dataInf[i * 2] = resINF->readUint16LE();

		for (int i = 0; i < _numInf; i++)
			_dataInf[i * 2 + 1] = resINF->readUint16LE();

		//for (int i = 0; i < _numInf; i++)
		//	debug("INF: %d,%d\n", _dataInf[i * 2], _dataInf[i * 2 + 1]);

		delete resINF;
		ret = true;
	} else {
		warning("The resource doesn't contain bitmap information");
	}

	return ret;
}

bool Bmp::loadOFF(Resource *resOFF) {
	bool ret = false;

	if (resOFF) {
		if (resOFF->size() % 4) {
			// This image doesn't contain offsets
			if (resOFF->readUint16LE() == 0xFFFF)
				ret = true;
		} else {
			_numOff = resOFF->size() / 4;
			if (_numOff == _numInf)
				ret = true;

			_dataOff = new uint32[_numOff];
			for (int i = 0; i < _numOff; i++) {
				_dataOff[i] = resOFF->readUint32LE();
				debug("OFF: %u\n", _dataOff[i]);
			}
		}

		delete resOFF;
	} else {
		// The resource doesn't contain bitmap offsets
		ret = true;
	}

	return ret;
}

bool Bmp::loadVQT(Resource *resVQT) {
	assert (_numInf == 1);	// image dimensions
	uint16 width = _dataInf[0];
	uint16 height = _dataInf[1];

	if (resVQT) {
		debug("Image dimensions: %dx%d data size: %d", width, height, resVQT->size());

#if 0
		uint16 unk1 = resVQT->readUint16LE();
		uint16 unk2 = resVQT->readUint16LE();

		uint32 size = 4008;//width * height;	// TODO: Hardcoded!
		//uint32 size = resVQT->size();
		Common::MemoryReadStream *vqt = decompRLE(resVQT, size);

		byte *data = new byte[size];
		//resVQT->read(data, size);
		vqt->read(data, size);
		Common::hexdump(data, size);
		delete data;
#endif

	}

	return true;
}

} // End of namespace Dgds
