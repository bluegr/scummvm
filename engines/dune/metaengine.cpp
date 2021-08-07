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

#include "backends/keymapper/action.h"
#include "backends/keymapper/keymapper.h"
#include "backends/keymapper/standard-actions.h"
#include "common/events.h"
#include "common/config-manager.h"
#include "common/savefile.h"
#include "common/serializer.h"
#include "common/system.h"
#include "common/translation.h"

#include "engines/advancedDetector.h"

class DuneMetaEngine : public AdvancedMetaEngine {
public:
	const char *getName() const override;
	Common::KeymapArray initKeymaps(const char *target) const override;

	Common::Error createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const override;
	bool hasFeature(MetaEngineFeature f) const override;

private:
	const char *cutscenesKeymapId = "dune-cutscenes";
	const char *mainKeymapId = "dune";
};

const char *DuneMetaEngine::getName() const {
	return "dune";
}

Common::KeymapArray DuneMetaEngine::initKeymaps(const char *target) const {
	Common::Keymap *cutscenesKeymap = new Common::Keymap(Common::Keymap::kKeymapTypeGame, cutscenesKeymapId, "Z-Vision - Cutscenes");

	Common::Action *act;

	act = new Common::Action(Common::kStandardActionSkip, _("Skip cutscene"));
	act->setCustomEngineActionEvent(Dune::kDuneActionSkipCutscene);
	act->addDefaultInputMapping("SPACE");
	act->addDefaultInputMapping("JOY_Y");
	cutscenesKeymap->addAction(act);

	
	Common::KeymapArray keymaps(1);
	keymaps[0] = cutscenesKeymap;

	return keymaps;
}

Common::Error DuneMetaEngine::createInstance(OSystem *syst, Engine **engine, const ADGameDescription *desc) const {
	*engine = new Dune::DuneEngine(syst, desc);
	return Common::kNoError;
}

bool DuneMetaEngine::hasFeature(MetaEngineFeature f) const {
	return 0;
}

#if PLUGIN_ENABLED_DYNAMIC(DUNE)
REGISTER_PLUGIN_DYNAMIC(DUNE, PLUGIN_TYPE_ENGINE, DuneMetaEngine);
#else
REGISTER_PLUGIN_STATIC(DUNE, PLUGIN_TYPE_ENGINE, DuneMetaEngine);
#endif
