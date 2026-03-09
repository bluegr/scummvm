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

/*
 * McMillan Installer archive reader.
 *
 * The YAGA engine games bundled Python 2.2.x scripts together with the
 * Python interpreter using the now-defunct McMillan Installer (predecessor
 * to PyInstaller).  This class reads the Table of Contents embedded at the
 * end of the game executable and exposes the bundled Python scripts and
 * pre-compiled bytecode modules.
 *
 * Archive footer layout (24 bytes at end of EXE):
 *   [0-7]   signature  { 0x4D,0x45,0x49,0x0C,0x0B,0x0A,0x0B,0x0E }
 *   [8-11]  archive size from EOF (big-endian uint32)
 *   [12-15] TOC offset from archive start (big-endian uint32)
 *   [16-19] TOC size in bytes (big-endian uint32)
 *   [20-23] (reserved)
 *
 * TOC entry layout (variable length, entries are contiguous):
 *   [0-3]   entry size, used to advance to next entry (big-endian uint32)
 *   [4-7]   data offset from archive start (big-endian uint32)
 *   [8-11]  data size in bytes (big-endian uint32)
 *   [12-15] uncompressed size (big-endian uint32)
 *   [16]    compression type  (2 = stored/uncompressed)
 *   [17]    entry type  ('z' = zip path, 'm' = marshalled module, 's' = script)
 *   [18+]   null-terminated entry name
 */

#ifndef YAGA_INSTALLER_H
#define YAGA_INSTALLER_H

#include "common/array.h"
#include "common/file.h"
#include "common/str.h"
#include "common/scummsys.h"

namespace Yaga {

/** Type tag of a McMillan TOC entry. */
enum InstallerEntryType {
	kInstallerEntryZipPath = 'z', // Embedded zip archive (bundled stdlib/packages)
	kInstallerEntryModule  = 'm', // Marshalled Python bytecode module (.pyc)
	kInstallerEntryScript  = 's'  // Python source script (main game logic)
};

/** One record from the McMillan installer Table of Contents. */
struct InstallerEntry {
	uint32 entrySize;        // Byte length of this whole TOC record
	uint32 dataOffset;       // Offset of entry data from archive start
	uint32 dataSize;         // Size of entry data in bytes
	uint32 uncompressedSize; // Uncompressed size (== dataSize when stored)
	byte   compressionType;  // Storage mode; 2 = stored (no compression)
	InstallerEntryType type; // Entry type tag, as defined by InstallerEntryType
	Common::String name;     // Module or script name
};

/**
 * Parses a McMillan Installer archive embedded inside a game executable.
 */
class Installer {
public:
	Installer();
	~Installer();

	/**
	 * Open an executable and parse its embedded McMillan archive.
	 * @param filename  Filename as known to ScummVM's SearchMan.
	 * @return true on success; false if the file cannot be opened or has
	 *         no valid McMillan archive.
	 */
	bool open(const Common::String &filename);

	/** Close the file handle and free all parsed TOC data. */
	void close();

	bool isOpen() const { return _file.isOpen(); }

	/** Byte offset of the archive block start within the EXE file. */
	uint32 getArchiveOffset() const { return _archiveOffset; }

	/** Path as passed to open(). */
	const Common::String &getArchivePath() const { return _archivePath; }

	/** All entries from the Table of Contents, in order. */
	const Common::Array<InstallerEntry> &getEntries() const { return _entries; }

	Common::String extractData(uint entry);

	/**
	 * Allocate a buffer and read the raw data for @p entry from the file.
	 *
	 * The returned buffer is always one byte larger than entry.dataSize and
	 * the extra byte is set to zero, so Python source scripts can be passed
	 * directly to PyRun_SimpleString().
	 *
	 * @return Heap-allocated buffer; caller must delete[].  Returns nullptr
	 *         on allocation failure or I/O error.
	 */
	Common::String extractData(const InstallerEntry &entry);

private:
	Common::File _file;
	Common::String _archivePath;
	uint32 _archiveOffset;
	Common::Array<InstallerEntry> _entries;

	/**
	 * Parse the raw TOC block and populate _entries.
	 *
	 * @param stream  Seekable stream containing the TOC data.
	 * @return true on success; false if the TOC is malformed.
	 */
	bool parseToc(Common::SeekableReadStream *stream);
};

} // End of namespace Yaga

#endif // YAGA_INSTALLER_H
