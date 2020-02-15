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

#include "ultima/ultima8/misc/pent_include.h"
#include "ultima/ultima8/gumps/pentagram_menu_gump.h"
#include "ultima/ultima8/graphics/render_surface.h"
#include "ultima/ultima8/ultima8.h"
#include "ultima/ultima8/gumps/widgets/game_widget.h"
#include "ultima/ultima8/conf/setting_manager.h"
#include "ultima/ultima8/filesys/file_system.h"
#include "ultima/ultima8/filesys/idata_source.h"
#include "ultima/ultima8/graphics/texture_png.h"
#include "ultima/ultima8/gumps/u8_save_gump.h"
#include "ultima/ultima8/gumps/gump_notify_process.h"
#include "ultima/ultima8/kernel/kernel.h"

namespace Ultima {
namespace Ultima8 {

DEFINE_RUNTIME_CLASSTYPE_CODE(PentagramMenuGump, ModalGump)

PentagramMenuGump::PentagramMenuGump(int x, int y, int width, int height) :
	ModalGump(x, y, width, height) {
	_gameScrollPos = 0;
	_gameScrollTarget = 0;
	_gameScrollLastDelta = 0;
	_titleImage = 0;
	_navbarImage = 0;
	_coversImage = 0;
	_flagsImage = 0;
}

PentagramMenuGump::~PentagramMenuGump() {
	delete _titleImage;
	delete _navbarImage;
	delete _coversImage;
	delete _flagsImage;
}

void PentagramMenuGump::InitGump(Gump *newparent, bool take_focus) {
	ModalGump::InitGump(newparent, take_focus);

	GameWidget *g;
	int y = 50;

	Std::vector<istring> games;
	// TODO: listGames() should probably be in CoreApp
	games = SettingManager::get_instance()->listGames();
	unsigned int gameIndex = 0;
	for (unsigned int i = 0; i < games.size(); ++i) {
		istring gameName = games[i];

		if (gameName == "pentagram") continue;
		if (!Ultima8Engine::get_instance()->getGameInfo(gameName)) continue;

		g = new GameWidget(150, y, gameName);
		g->InitGump(this, false);
		g->SetIndex(gameIndex++);
		y += 114;
	}

	_gameCount = gameIndex;

	IDataSource *ds = FileSystem::get_instance()->ReadFile("@data/title.png");
	_titleImage = Texture::Create(ds, "title.png");
	delete ds;

	ds = FileSystem::get_instance()->ReadFile("@data/navbar.png");
	_navbarImage = Texture::Create(ds, "navbar.png");
	delete ds;

	ds = FileSystem::get_instance()->ReadFile("@data/covers.png");
	_coversImage = Texture::Create(ds, "covers.png");
	delete ds;

	ds = FileSystem::get_instance()->ReadFile("@data/flags.png");
	_flagsImage = Texture::Create(ds, "flags.png");
	delete ds;
}

void PentagramMenuGump::PaintThis(RenderSurface *surf, int32 lerp_factor, bool /*scaled*/) {
	int w = _dims.w, h = _dims.h;
#if 1
	// CHECKME: fast enough?
	for (int i = 0; i < h; i += 4) {
		unsigned int r = (140 * i) / h;
		unsigned int gb = (21 * i) / h;
		uint32 col = 0xFF000000 + (r << 16) + (gb << 8) + gb;
		surf->Fill32(col, 0, i, w, 4);
	}
#else
	surf->Fill32(0xFF440A0A, 0, 0, w, h);
#endif

//	surf->Fill32(0xFFDCB95C, 18, 0, 90, 400);
	surf->Blit(_navbarImage, 0, 0, _navbarImage->width, _navbarImage->height, 9, 0);

//	surf->Fill32(0xFFC11515, 200, 6, 340, 36);
	surf->Blit(_titleImage, 0, 0, _titleImage->width, _titleImage->height, 200, 6);
}

void PentagramMenuGump::PaintChildren(RenderSurface *surf, int32 lerp_factor, bool scaled) {
	// Iterate all children
	Std::list<Gump *>::iterator it = _children.begin();
	Std::list<Gump *>::iterator end = _children.end();

	Rect game_clip_rect(0, 45, SCREEN_WIDTH, _dims.h - 58);
	Rect cur_clip_rect;
	surf->GetClippingRect(cur_clip_rect);


	while (it != end) {
		Gump *g = *it;

		if (g->IsOfType<GameWidget>()) {
			surf->SetClippingRect(game_clip_rect);
			g->Move(150, 50 + 114 * g->GetIndex() + _gameScrollPos);
		}

		g->Paint(surf, lerp_factor, scaled);

		surf->SetClippingRect(cur_clip_rect);

		++it;
	}
}


void PentagramMenuGump::ChildNotify(Gump *child, uint32 message) {
	if (child->IsOfType<GameWidget>()) {

		GameWidget *gw = p_dynamic_cast<GameWidget *>(child);
		istring gamename = gw->getGameName();

		switch (message) {
		case GameWidget::GAME_PLAY:
			Ultima8Engine::get_instance()->changeGame(gamename);
			break;
		case GameWidget::GAME_LOAD: {
			GameInfo *info = Ultima8Engine::get_instance()->getGameInfo(gamename);
			if (info && info->type == GameInfo::GAME_U8) {
				Ultima8Engine::get_instance()->menuInitMinimal(gamename);
				Gump *gump = U8SaveGump::showLoadSaveGump(0, false);
				if (gump) {
					HideGump();
					gump->CreateNotifier();

					PentagramMenuCallbackProcess *p = new PentagramMenuCallbackProcess(getObjId(), gamename);

					Kernel::get_instance()->addProcess(p);
					p->waitFor(gump->GetNotifyProcess());
				} else {
					Ultima8Engine::get_instance()->menuInitMinimal("pentagram");
				}
			} else {
				Ultima8Engine::get_instance()->Error("Load Savegame not yet implemented");
			}
		}
		break;
		case GameWidget::GAME_SETTINGS:
			Ultima8Engine::get_instance()->Error("Settings not yet implemented");
			break;
		case GameWidget::GAME_REMOVE:
			Ultima8Engine::get_instance()->Error("Remove not yet implemented");
			break;
		}
	}
}

void PentagramMenuGump::run() {
	int oldpos = _gameScrollPos;
	ModalGump::run();

	if (_gameScrollPos != _gameScrollTarget) {
		int diff = _gameScrollTarget - _gameScrollPos;
		if (diff < 20 && diff > -20) {
			_gameScrollPos = _gameScrollTarget;
		} else {
			_gameScrollPos += diff / 3;
		}
	}

	_gameScrollLastDelta = _gameScrollPos - oldpos;
}


bool PentagramMenuGump::OnKeyDown(int key, int mod) {
	int delta = 0;

	if (key == Common::KEYCODE_DOWN) {
		delta = -114;
	} else if (key == Common::KEYCODE_UP) {
		delta = 114;
	}

	if (delta && _gameCount > 3) {
		_gameScrollTarget += delta;

		if (_gameScrollTarget > 0)
			_gameScrollTarget = 0;
		if (_gameScrollTarget < -114 * (_gameCount - 3))
			_gameScrollTarget = -114 * (_gameCount - 3);

		return true;
	}

	return false;
}

void PentagramMenuGump::ProcessCallback(Std::string gamename, int message) {
	if (message != 0) {
		SettingManager *settingman = SettingManager::get_instance();
		settingman->set("lastSave", message != 1 ? message : -1);
		Ultima8Engine::get_instance()->changeGame(gamename);
	}

	UnhideGump();
	Ultima8Engine::get_instance()->menuInitMinimal("pentagram");
}

} // End of namespace Ultima8
} // End of namespace Ultima
