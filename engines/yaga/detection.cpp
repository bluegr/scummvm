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

#include "base/plugins.h"
#include "common/config-manager.h"
#include "common/file.h"
#include "common/md5.h"
#include "common/str-array.h"
#include "common/translation.h"
#include "common/util.h"
#include "yaga/detection.h"
#include "yaga/detection_tables.h"

static const char *const directoryGlobs[] = {
	"MaxFiles",
	"Pajama Sam LRS",
	"Putt-Putt PBS",
	0
};

const DebugChannelDef YagaMetaEngineDetection::debugFlagList[] = {
	{ Yaga::kDebugGraphics, "Graphics", "Graphics debug level" },
	{ Yaga::kDebugPath, "Path", "Pathfinding debug level" },
	{ Yaga::kDebugFilePath, "FilePath", "File path debug level" },
	{ Yaga::kDebugScan, "Scan", "Scan for unrecognised games" },
	{ Yaga::kDebugScript, "Script", "Enable debug script dump" },
	DEBUG_CHANNEL_END
};

YagaMetaEngineDetection::YagaMetaEngineDetection() : AdvancedMetaEngineDetection(
	Yaga::gameDescriptions, Yaga::yagaGames) {
	_maxScanDepth = 2;
	_directoryGlobs = directoryGlobs;
	_guiOptions = GUIO2(GUIO_NOMIDI, GAMEOPTION_ORIGINAL_SAVELOAD);		
}

REGISTER_PLUGIN_STATIC(YAGA_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, YagaMetaEngineDetection);
