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
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#define FORBIDDEN_SYMBOL_ALLOW_ALL

#include "file.h"
#include "str.h"
#include "winexe_pe.h"
#include "common/array.h"
#include "common/endian.h"

namespace Common {

PEResources::PEResources() {
	_exe = 0;
}

PEResources::~PEResources() {
	clear();
}

void PEResources::clear() {
	_sections.clear();
	_resources.clear();
	delete _exe; _exe = 0;
}

bool PEResources::loadFromEXE(const String &fileName) {
	if (fileName.empty())
		return false;

	File *file = new File();

	if (!file->open(fileName.c_str())) {
		delete file;
		return false;
	}

	return loadFromEXE(file);
}

bool PEResources::loadFromEXE(File *stream) {
	clear();

	if (!stream)
		return false;

	if (stream->readUint16BE() != MKTAG16('M', 'Z'))
		return false;

	stream->skip(58);

	uint32 peOffset = stream->readUint32LE();

	if (!peOffset || peOffset >= (uint32)stream->size())
		return false;

	stream->seek(peOffset);

	if (stream->readUint32BE() != MKTAG('P','E',0,0))
		return false;

	stream->skip(2);
	uint16 sectionCount = stream->readUint16LE();
	stream->skip(12);
	uint16 optionalHeaderSize = stream->readUint16LE();
	stream->skip(optionalHeaderSize + 2);

	// Read in all the sections
	for (uint16 i = 0; i < sectionCount; i++) {
		char sectionName[9];
		stream->read(sectionName, 8);
		sectionName[8] = 0;

		Section section;
		stream->skip(4);
		section.virtualAddress = stream->readUint32LE();
		section.size = stream->readUint32LE();
		section.offset = stream->readUint32LE();
		stream->skip(16);

		_sections[sectionName] = section;
	}

	// Currently, we require loading a resource section
	if (!_sections.contains(".rsrc")) {
		clear();
		return false;
	}

	_exe = stream;

	Section &resSection = _sections[".rsrc"];
	parseResourceLevel(resSection, resSection.offset, 0);

	return true;
}

void PEResources::parseResourceLevel(Section &section, uint32 offset, int level) {
	_exe->seek(offset + 12);

	uint16 namedEntryCount = _exe->readUint16LE();
	uint16 intEntryCount = _exe->readUint16LE();

	for (uint32 i = 0; i < (uint32)(namedEntryCount + intEntryCount); i++) {
		uint32 value = _exe->readUint32LE();

		WinResourceID id;

		if (value & 0x80000000) {
			value &= 0x7fffffff;

			uint32 startPos = _exe->pos();
			_exe->seek(section.offset + (value & 0x7fffffff));

			// Read in the name, truncating from unicode to ascii
			String name;
			uint16 nameLength = _exe->readUint16LE();
			while (nameLength--)
				name += (char)(_exe->readUint16LE() & 0xff);

			_exe->seek(startPos);

			id = name;
		} else {
			id = value;
		}

		uint32 nextOffset = _exe->readUint32LE();
		uint32 lastOffset = _exe->pos();

		if (level == 0)
			_curType = id;
		else if (level == 1)
			_curID = id;
		else if (level == 2)
			_curLang = id;

		if (level < 2) {
			// Time to dive down further
			parseResourceLevel(section, section.offset + (nextOffset & 0x7fffffff), level + 1);
		} else {
			_exe->seek(section.offset + nextOffset);

			Resource resource;
			resource.offset = _exe->readUint32LE() + section.offset - section.virtualAddress;
			resource.size = _exe->readUint32LE();

			_resources[_curType][_curID][_curLang] = resource;
		}

		_exe->seek(lastOffset);
	}
}

const Array<WinResourceID> PEResources::getTypeList() const {
	Array<WinResourceID> array;

	if (!_exe)
		return array;

	for (TypeMap::const_iterator it = _resources.begin(); it != _resources.end(); it++)
		array.push_back(it->_key);

	return array;
}

const Array<WinResourceID> PEResources::getIDList(const WinResourceID &type) const {
	Array<WinResourceID> array;

	if (!_exe || !_resources.contains(type))
		return array;

	const IDMap &idMap = _resources[type];

	for (IDMap::const_iterator it = idMap.begin(); it != idMap.end(); it++)
		array.push_back(it->_key);

	return array;
}

const Array<WinResourceID> PEResources::getLangList(const WinResourceID &type, const WinResourceID &id) const {
	Array<WinResourceID> array;

	if (!_exe || !_resources.contains(type))
		return array;

	const IDMap &idMap = _resources[type];

	if (!idMap.contains(id))
		return array;

	const LangMap &langMap = idMap[id];

	for (LangMap::const_iterator it = langMap.begin(); it != langMap.end(); it++)
		array.push_back(it->_key);

	return array;
}

File *PEResources::getResource(const WinResourceID &type, const WinResourceID &id) {
	Array<WinResourceID> langList = getLangList(type, id);

	if (langList.empty())
		return 0;

	const Resource &resource = _resources[type][id][langList[0]];
	byte *data = (byte *)malloc(resource.size);
	_exe->seek(resource.offset);
	_exe->read(data, resource.size);

	File *file = new File();
	file->open(data, resource.size);
	return file;
}

File *PEResources::getResource(const WinResourceID &type, const WinResourceID &id, const WinResourceID &lang) {
	if (!_exe || !_resources.contains(type))
		return 0;

	const IDMap &idMap = _resources[type];

	if (!idMap.contains(id))
		return 0;

	const LangMap &langMap = idMap[id];

	if (!langMap.contains(lang))
		return 0;

	const Resource &resource = langMap[lang];
	byte *data = (byte *)malloc(resource.size);
	_exe->seek(resource.offset);
	_exe->read(data, resource.size);

	File *file = new File();
	file->open(data, resource.size);
	return file;
}

} // End of namespace Common
