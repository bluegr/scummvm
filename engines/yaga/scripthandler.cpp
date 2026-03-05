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

#include "common/file.h"
#include "yaga/scripthandler.h"

namespace Yaga {

ScriptHandler::ScriptHandler(const Common::String &filename) {
	readTableOfContents(filename);
}

ScriptHandler::~ScriptHandler() {
	
}

void ScriptHandler::readTableOfContents(const Common::String &filename) {
	static const byte SIG[] = { 0x4D, 0x45, 0x49, 0x0C, 0x0B, 0x0A, 0x0B, 0x0E };
	byte buf[8];

	Common::File file;
	file.open(Common::Path(filename));
	file.seek(-24, SEEK_END);
	file.read(buf, 8);
	assert(memcmp(buf, SIG, 8) == 0);
	uint32 offset = file.size() - file.readUint32BE();
	uint32 pos = offset + file.readUint32BE();
	uint32 tableOfContentsSize = file.readUint32BE();
	file.seek(pos, SEEK_SET);
	byte *toc = new byte[tableOfContentsSize];
	file.read(toc, tableOfContentsSize);
	Common::hexdump(toc, tableOfContentsSize);

	delete[] toc;
	file.close();
}

} // End of namespace Yaga
