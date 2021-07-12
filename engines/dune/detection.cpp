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


#include "dune/dune.h"

#include "common/config-manager.h"
#include "common/system.h"
#include "common/savefile.h"
#include "common/serializer.h"
#include "common/translation.h"

#include "engines/advancedDetector.h"

namespace Dune {

static const PlainGameDescriptor duneGames[] = {
	{"dune", "Dune"},
	{0, 0}
};

static const ADGameDescription gameDescriptions[] = {
	// CD version
	{
		"dune",
		"CD",
		{
			{"dune.dat", 0, "f096565944ab48cf4cb6cbf389384e6f", 397794384},
			{0,0,0,0}
		},
		Common::EN_ANY,
		Common::kPlatformDOS,
		ADGF_CD,
		GUIO0()
	},

	AD_TABLE_END_MARKER
};

} // End of namespace Dune

class DuneMetaEngineDetection : public AdvancedMetaEngineDetection {
public:
	DuneMetaEngineDetection();

	const char *getEngineId() const override;
	const char *getName() const override;
	const char *getOriginalCopyright() const override;
};

DuneMetaEngineDetection::DuneMetaEngineDetection()
	: AdvancedMetaEngineDetection(
		Dune::gameDescriptions,
		sizeof(Dune::gameDescriptions[0]),
		Dune::duneGames) {}

const char *DuneMetaEngineDetection::getEngineId() const {
	return "dune";
}

const char *DuneMetaEngineDetection::getName() const {
	return "Dune";
}

const char *DuneMetaEngineDetection::getOriginalCopyright() const {
	return "Dune (C) 1992 Cryo Interactive Entertainment";
}

REGISTER_PLUGIN_STATIC(DUNE_DETECTION, PLUGIN_TYPE_ENGINE_DETECTION, DuneMetaEngineDetection);
