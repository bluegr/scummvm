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

#include "common/scummsys.h"

#include "common/system.h"
#include "graphics/pixelformat.h"

#include "zvision/zvision.h"
#include "zvision/cursor_manager.h"


namespace ZVision {

const char *CursorManager::_cursorNames[] = { "active", "arrow", "backward", "downarrow", "forward", "handpt", "handpu", "hdown", "hleft",
                                              "hright", "hup", "idle", "leftarrow", "rightarrow", "suggest_surround", "suggest_tilt", "turnaround", "zuparrow" };

const char *CursorManager::_zgiCursorFileNames[] = { "g0gbc011.zcr", "g0gac001.zcr", "g0gac021.zcr", "g0gac031.zcr", "g0gac041.zcr", "g0gac051.zcr", "g0gac061.zcr", "g0gac071.zcr", "g0gac081.zcr",
                                                     "g0gac091.zcr", "g0gac101.zcr", "g0gac011.zcr", "g0gac111.zcr", "g0gac121.zcr", "g0gac131.zcr", "g0gac141.zcr", "g0gac151.zcr", "g0gac161.zcr" };

const char *CursorManager::_zNemCursorFileNames[] = { "00act", "arrow", "back", "down", "forw", "handpt", "handpu", "hdown", "hleft",
                                                      "hright", "hup", "00idle", "left", "right", "ssurr", "stilt", "turn", "up" };


CursorManager::CursorManager(ZVision *engine, const Graphics::PixelFormat *pixelFormat)
		: _engine(engine),
		  _pixelFormat(pixelFormat),
		  _cursorIsPushed(false),
		  _currentCursor("idle") {
	// WARNING: The index 11 is hardcoded. If you change the order of _cursorNames/_zgiCursorFileNames/_zNemCursorFileNames, you HAVE to change the index accordingly
	if (_engine->getGameId() == ZorkNemesis) {
		Common::String name(Common::String::format("%sa.zcr", _zNemCursorFileNames[11]));
		_idleCursor = ZorkCursor(name);
	} else if (_engine->getGameId() == ZorkGrandInquisitor) {
		_idleCursor = ZorkCursor(_zgiCursorFileNames[11]);
	}
}

void CursorManager::initialize() {
	_engine->_system->setMouseCursor(_idleCursor.getSurface(), _idleCursor.getWidth(), _idleCursor.getHeight(), _idleCursor.getHotspotX(), _idleCursor.getHotspotY(), _idleCursor.getKeyColor(), false, _pixelFormat);
	_engine->_system->showMouse(true);
}

void CursorManager::changeCursor(Common::String &cursorName, bool pushed) {
	_cursorIsPushed = pushed;

	if (cursorName == "idle" && !pushed) {
		revertToIdle();
		return;
	}

	// WARNING: The range of this loop is hardcoded to the length of _cursorNames
	for (int i = 0; i < 18; i++) {
		if (_engine->getGameId() == ZorkNemesis) {
			if (_cursorNames[i] == cursorName) {
				// ZNem uses a/b at the end of the file to signify not pushed/pushed respectively
				Common::String pushedFlag = pushed ? "b" : "a";
				Common::String name = Common::String::format("%s%s.zcr", _zNemCursorFileNames[i], pushedFlag);

				changeCursor(ZorkCursor(name));
				return;
			}
		} else if (_engine->getGameId() == ZorkGrandInquisitor) {
			if (_cursorNames[i] == cursorName) {
				if (!pushed) {
					changeCursor(ZorkCursor(_zgiCursorFileNames[i]));
				} else {
					// ZGI flips not pushed/pushed between a/c and b/d
					// It flips the 4th character of the name
					char buffer[25];
					strcpy(buffer, _zgiCursorFileNames[i]);
					buffer[3] += 2;
				}
				return;
			}
		}
	}

	// If we get here, something went wrong
	warning("No cursor found for identifier %s", cursorName.c_str());
}

void CursorManager::changeCursor(const ZorkCursor &cursor) {
	_engine->_system->setMouseCursor(cursor.getSurface(), cursor.getWidth(), cursor.getHeight(), cursor.getHotspotX(), cursor.getHotspotY(), cursor.getKeyColor(), false, _pixelFormat);
}

void CursorManager::cursorDown(bool pushed) {
	if (_cursorIsPushed == pushed)
		return;

	_cursorIsPushed = pushed;
	changeCursor(_currentCursor, pushed);
}

void CursorManager::revertToIdle() {
	_currentCursor = "idle";
	_engine->_system->setMouseCursor(_idleCursor.getSurface(), _idleCursor.getWidth(), _idleCursor.getHeight(), _idleCursor.getHotspotX(), _idleCursor.getHotspotY(), _idleCursor.getKeyColor(), false, _pixelFormat);
}

} // End of namespace ZVision
