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

#include "common/debug.h"
#include "common/endian.h"
#include "common/substream.h"
#include "common/textconsole.h"

#include "yaga/installer.h"

namespace Yaga {

// 8-byte magic that identifies a McMillan Installer archive footer.
static const byte kMcMillanSig[8] = { 0x4D, 0x45, 0x49, 0x0C, 0x0B, 0x0A, 0x0B, 0x0E };

// Total size of the signature.
static const int kSigSize = 8;

// Total size of the archive footer appended to the executable.
static const int kFooterSize = 24;

// Offset of the entry name field within a TOC entry.
static const int kEntryNameOffset = 18;

Installer::Installer() : _archiveOffset(0) {
}

Installer::~Installer() {
	close();
}

bool Installer::open(const Common::String &filename) {
	_archivePath = filename;

	if (!_file.open(Common::Path(filename))) {
		warning("Installer: cannot open '%s'", filename.c_str());
		return false;
	}

	if (_file.size() < kFooterSize) {
		warning("Installer: '%s' is too small to contain a McMillan archive", filename.c_str());
		_file.close();
		return false;
	}

	// Read the 24-byte footer that McMillan Installer appends to the EXE.
	_file.seek(-kFooterSize, SEEK_END);

	byte sig[kSigSize];
	_file.read(sig, kSigSize);

	// Validate the 8-byte magic signature.
	if (memcmp(sig, kMcMillanSig, kSigSize) != 0) {
		warning("Installer: '%s' has no McMillan archive signature", filename.c_str());
		_file.close();
		return false;
	}

	// footer[8..11]  = archive size counted from the end of the file
	// footer[12..15] = TOC offset relative to the archive start
	// footer[16..19] = TOC size in bytes
	uint32 archiveSizeFromEnd = _file.readUint32BE();
	uint32 tocRelOffset       = _file.readUint32BE();
	uint32 tocSize            = _file.readUint32BE();

	_archiveOffset = (uint32)_file.size() - archiveSizeFromEnd;

	uint32 tocAbsOffset = _archiveOffset + tocRelOffset;
	_file.seek(tocAbsOffset, SEEK_SET);

	Common::SeekableSubReadStream tocStream(&_file, tocAbsOffset, tocAbsOffset + tocSize);

	bool ok = parseToc(&tocStream);

	if (!ok) {
		_file.close();
		return false;
	}

	debug(1, "Installer: opened '%s': archive offset 0x%08X, %u TOC entries",
	      filename.c_str(), _archiveOffset, _entries.size());
	return true;
}

void Installer::close() {
	_file.close();
	_entries.clear();
	_archiveOffset = 0;
}

bool Installer::parseToc(Common::SeekableReadStream *stream) {
	while (!stream->eos()) {
		uint32 entrySize = stream->readUint32BE();
		// Check for EOS again, to avoid reading junk bytes
		// at the end of the file
		if (stream->eos())
			break;

		if (entrySize < (uint32)(kEntryNameOffset + 1)) {
			warning("Installer: TOC entry too small (%u bytes)", entrySize);
			return false;
		}

		InstallerEntry entry;
		entry.entrySize        = entrySize;
		entry.dataOffset       = stream->readUint32BE();
		entry.dataSize         = stream->readUint32BE();
		entry.uncompressedSize = stream->readUint32BE();
		entry.compressionType  = stream->readByte();

		byte type = stream->readByte();
		if (type != kInstallerEntryZipPath &&
			type != kInstallerEntryModule &&
			type != kInstallerEntryScript) {
			warning("Installer: unknown TOC entry type '%c' (0x%02X)", type, type);
			return false;
		}

		entry.type = (InstallerEntryType)type;
		entry.name = stream->readString('\0', entrySize - kEntryNameOffset);

		_entries.push_back(entry);

		debug(2, "Installer TOC: type='%c' offset=0x%08X size=%u name='%s'",
		      type, entry.dataOffset, entry.dataSize, entry.name.c_str());
	}

	return true;
}

Common::String Installer::extractData(uint entry) {
	if (entry >= _entries.size()) {
		warning("Installer: invalid entry index %u", entry);
		return nullptr;
	}
	return extractData(_entries[entry]);
}

Common::String Installer::extractData(const InstallerEntry &entry) {
	// Only stored (uncompressed) entries are supported.
	if (entry.compressionType != 2) {
		warning("Installer: compressed entries (type %u) are not supported",
		        entry.compressionType);
		return nullptr;
	}

	_file.seek(_archiveOffset + entry.dataOffset, SEEK_SET);
	return _file.readString('\0', entry.dataSize);
}

} // End of namespace Yaga
