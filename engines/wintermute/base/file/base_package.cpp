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
 */

/*
 * This file is based on WME Lite.
 * http://dead-code.org/redir.php?target=wmelite
 * Copyright (c) 2011 Jan Nedoma
 */

#include "engines/wintermute/base/file/base_package.h"
#include "engines/wintermute/base/base_file_manager.h"
#include "common/file.h"
#include "common/stream.h"

namespace WinterMute {
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
BasePackage::BasePackage(BaseFileManager *fileMan)/*: BaseClass(inGame) */{
	_file = NULL;
	_name = NULL;
	_cd = 0;
	_priority = 0;
	_boundToExe = false;
	_fileManager = fileMan;
}


//////////////////////////////////////////////////////////////////////////
BasePackage::~BasePackage() {
	if (_name) delete [] _name;
	closeFilePointer(_file);
}


//////////////////////////////////////////////////////////////////////////
bool BasePackage::open() {
	if (_file) return true;
	else {
		_file = getFilePointer();
		return _file ? true : false;
	}
}


//////////////////////////////////////////////////////////////////////////
bool BasePackage::close() {
	delete _file;
	_file = NULL;
	return true;
}


//////////////////////////////////////////////////////////////////////////
bool BasePackage::read(Common::SeekableReadStream *file, uint32 offset, byte *buffer, uint32 size) {
	bool ret;
	if (!(ret = open())) return ret;
	else {
		if (file->seek(offset, SEEK_SET)) return false;
		if (file->read(buffer, size) != 1) return false;
		else return true;
	}
}

//////////////////////////////////////////////////////////////////////////
Common::SeekableReadStream *BasePackage::getFilePointer() {
	Common::SeekableReadStream *stream = _fsnode.createReadStream();
/*	Common::File *file = _fileManager->openPackage(_name);
	if (!file) {
		_fileManager->requestCD(_cd, _name, "");
		file = _fileManager->openPackage(_name);
	}*/
	return stream;
}

//////////////////////////////////////////////////////////////////////////
void BasePackage::closeFilePointer(Common::SeekableReadStream *&file) {
	delete file;
	file = NULL;
}

} // end of namespace WinterMute
