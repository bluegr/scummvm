/* ScummVM - Scumm Interpreter
 * Copyright (C) 2001  Ludvig Strigeus
 * Copyright (C) 2001-2004 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$
 *
 */

#include "stdafx.h"

#include "base/engine.h"

#include "scumm/scumm.h"
#include "scumm/actor.h"
#include "scumm/sound.h"

#include "scumm/imuse.h"
#include "scumm/imuse_digi/dimuse.h"

#include "scumm/smush/smush_player.h"
#include "scumm/smush/chunk_type.h"
#include "scumm/smush/chunk.h"

#include "scumm/insane/insane.h"

// TODO (in no particular order):
// o Ben's velocity don't get zeroed after crash
// o TRS file support. Everything is in place, I just need to figure out function parameters
// o Code review/cleanup
// o DOS demo INSANE

namespace Scumm {

static const int actorAnimationData[21] = {20, 21, 22, 23, 24, 25, 26, 13, 14, 15, 16, 17, 
	18, 19, 6, 7, 8, 9, 10, 11, 12};


Insane::Insane(ScummEngine_v6 *scumm) {
	_vm = scumm;
	
	// Demo has different insane, so disable it now
	if (_vm->_features & GF_DEMO)
		return;

	initvars();

	readFileToMem("roadrash.rip", &_smush_roadrashRip);
	readFileToMem("roadrsh2.rip", &_smush_roadrsh2Rip);
	readFileToMem("roadrsh3.rip", &_smush_roadrsh3Rip);
	readFileToMem("goglpalt.rip", &_smush_goglpaltRip);
	readFileToMem("tovista1.flu", &_smush_tovista1Flu);
	readFileToMem("tovista2.flu", &_smush_tovista2Flu);
	readFileToMem("toranch.flu", &_smush_toranchFlu);
	readFileToMem("minedriv.flu", &_smush_minedrivFlu);
	readFileToMem("minefite.flu", &_smush_minefiteFlu);

	_smush_iconsNut = new NutRenderer(_vm);
	_smush_iconsNut->loadFont("icons.nut", _vm->getGameDataPath());
	_smush_icons2Nut = new NutRenderer(_vm);
	_smush_icons2Nut->loadFont("icons2.nut", _vm->getGameDataPath());
	_smush_bensgoggNut = new NutRenderer(_vm);
	_smush_bensgoggNut->loadFont("bensgogg.nut", _vm->getGameDataPath());
	_smush_bencutNut = new NutRenderer(_vm);
	_smush_bencutNut->loadFont("bencut.nut", _vm->getGameDataPath());

	// FIXME: implement
	// openManyResource(0, 4, "specfnt.nut", "titlfnt.nut", "techfnt.nut", "scummfnt.nut");
}

Insane::~Insane(void) {
	free(_smush_roadrashRip);
	free(_smush_roadrsh2Rip);
	free(_smush_roadrsh3Rip);
	free(_smush_goglpaltRip);
	free(_smush_tovista1Flu);
	free(_smush_tovista2Flu);
	free(_smush_toranchFlu);
	free(_smush_minedrivFlu);
	free(_smush_minefiteFlu);

	delete _smush_bencutNut;
	delete _smush_bensgoggNut;
	delete _smush_iconsNut;
	delete _smush_icons2Nut;
}

void Insane::setSmushParams(int speed) {
	_speed = speed;
}

void Insane::initvars(void) {
	int i, j;

	_speed = 12;
	_insaneIsRunning = false;

	_numberArray = 0;
	_emulateInterrupt = 0;
	_flag1d = 0;
	_objArray1Idx = 0;
	_objArray1Idx2 = 0;
	_objArray2Idx = 0;
	_objArray2Idx2 = 0;
	_currSceneId = 1;
	_timer6Id = 0;
	_timerSpriteId = 0;
	_temp2SceneId = 0;
	_tempSceneId = 0;
	_currEnemy = -1;
	_currScenePropIdx = 0;
	_currScenePropSubIdx = 0;
	_currTrsMsg = 0;
	_sceneData2Loaded = 0;
	_sceneData1Loaded = 0;
	_keyboardDisable = 0;
	_needSceneSwitch = false;
	_idx2Exceeded = 0;
	_memoryAllocatedNotOK = 0;
	_lastKey = 0;
	_tiresRustle = false;
	_keybOldDx = 0;
	_keybOldDy = 0;
	_velocityX = 0;
	_velocityY = 0;
	_keybX = 0;
	_keybY = 0;
	_firstBattle = false;
	_battleScene = true;
	_kickBenProgress = false;
	_weaponBenJustSwitched = false;
	_kickEnemyProgress = false;
	_weaponEnemyJustSwitched = false;
	_beenCheated = 0;
	_val11d = 0;
	_val32d = -1;
	_val51d = 0;
	_val52d = 0;
	_val53d = 0;
	_val54d = 0;
	_val55d = 0;
	_val56d = 0;
	_val57d = 0;
	_val109w = 0x40;
	_val10b = 3;
	_val110w = 0x100;
	_val111w = 0x20;
	_val112w = 0x20;
	_val113d = 1;
	_val115w = 0;
	_val116w = 0;
	_val119_ = false;
	_val120_ = false;
	_val121_ = false;
	_val122_ = false;
	_val123_ = false;
	_val124_ = false;
	_val211d = 0;
	_val212_ = 0;
	_val213d = 0;
	_val214d = -1;
	_val215d = 0;
	_smlayer_room = 0;
	_smlayer_room2 = 0;
	_isBenCut = 0;
	_smush_isPauseImuse = false;
	_continueFrame = 0;
	_continueFrame1 = 0;
	_counter1 = 0;
	_iactSceneId = 0;
	_iactSceneId2 = 0;

	for (i = 0; i < 9; i++)
		for (j = 0; j < 9; j++)
			_enHdlVar[i][j] = 0;

	for (i = 0; i < 0x80; i++)
		_iactBits[i] = 0;

	init_enemyStruct(EN_ROTT1, EN_ROTT1, 0, 0, 160, 0, INV_MACE, 90, "wr2_rott.san", 
					 26, 16, 17, 27, 11, 3);
	init_enemyStruct(EN_ROTT2, EN_ROTT2, 1, 0, 250, 0, INV_2X4, 90, "wr2_rott.san", 
					 28, 16, 17, 42, 11, 3);
	init_enemyStruct(EN_ROTT3, EN_ROTT3, 2, 0, 120, 0, INV_HAND, 90, "wr2_rott.san", 
					 15, 16, 17, 43, 11, 3);
	init_enemyStruct(EN_VULTF1, EN_VULTF1, 3, 0, 60, 0, INV_HAND, 91, "wr2_vltp.san", 
					 29, 33, 32, 37, 12, 4);
	init_enemyStruct(EN_VULTM1, EN_VULTM1, 4, 0, 100, 0, INV_CHAIN, 91, "wr2_vltc.san", 
					 30, 33, 32, 36, 12, 4);
	init_enemyStruct(EN_VULTF2, EN_VULTF2, 5, 0, 250, 0, INV_CHAINSAW, 91, "wr2_vlts.san", 
					 31, 33, 32, 35, 12, 4);
	init_enemyStruct(EN_VULTM2, EN_VULTM2, 6, 0, 900, 0, INV_BOOT, 91, "wr2_rott.san", 
					 34, 33, 32, 45, 16, 4);
	init_enemyStruct(EN_CAVEFISH, EN_CAVEFISH, 7, 0, 60, 0, INV_DUST, 92, "wr2_cave.san", 
					 39, 0, 0, 41, 13, 2);
	init_enemyStruct(EN_TORQUE, EN_TORQUE, 8, 0, 900, 0, INV_HAND, 93, "wr2_vltp.san", 
					 57, 0, 0, 37, 12, 1);

	init_fluConfStruct(1, 1, &_smush_minedrivFlu, "minedriv.san", 235, 1300);
	init_fluConfStruct(2, 1, &_smush_minedrivFlu, "minedriv.san", 355, 1300);
	init_fluConfStruct(3, 1, &_smush_minedrivFlu, "minedriv.san", 1255, 1300);
	init_fluConfStruct(4, 1, &_smush_minedrivFlu, "minedriv.san", 565, 1300);
	init_fluConfStruct(5, 1, &_smush_minedrivFlu, "minedriv.san", 1040, 1300);
	init_fluConfStruct(8, 1, &_smush_minedrivFlu, "minedriv.san", 1040, 1300);
	init_fluConfStruct(9, 1, &_smush_minedrivFlu, "minedriv.san", 655, 1300);
	init_fluConfStruct(10, 1, &_smush_minedrivFlu, "minedriv.san", 115, 1300);
	init_fluConfStruct(11, 1, &_smush_minedrivFlu, "minedriv.san", 315, 1300);
	init_fluConfStruct(12, 1, &_smush_minedrivFlu, "minedriv.san", 235, 1300);
	init_fluConfStruct(15, 6, &_smush_toranchFlu, "toranch.san", 115, 530);
	init_fluConfStruct(16, 5, &_smush_tovista2Flu, "tovista2.san", 235, 290);
	init_fluConfStruct(17, 4, &_smush_tovista1Flu, "tovista1.san", 175, 230);
	init_fluConfStruct(18, 4, &_smush_tovista1Flu, "tovista1.san", 175, 230);
	init_fluConfStruct(19, 6, &_smush_toranchFlu, "toranch.san", 115, 530);
	init_fluConfStruct(20, 6, &_smush_toranchFlu, "toranch.san", 115, 530);
  
	init_scenePropStruct(  0,  0, 0, 0, 0, 0, 0, 0, 0, 0, 1);
	init_scenePropStruct(  1,  0, 1, 128, 2001, 0, 0, 0, 0, 56, 2);
	init_scenePropStruct(  2,  0, 0, 125, 1002, 0, 0, 0, 0, 35, 3);
	init_scenePropStruct(  3,  0, 1, 129, 2002, 0, 0, 0, 0, 23, 4);
	init_scenePropStruct(  4,  0, 1, 130, 2003, 0, 0, 0, 0, 40, 5);
	init_scenePropStruct(  5,  0, 0, 126, 1005, 0, 0, 0, 0, 46, 6);
	init_scenePropStruct(  6,  0, 1, 131, 2004, 0, 0, 0, 0, 39, 7);
	init_scenePropStruct(  7,  0, 1, 132, 2005, 0, 0, 0, 0, 45, 8);
	init_scenePropStruct(  8,  0, 1, 133, 2006, 0, 0, 0, 0, 14, 9);
	init_scenePropStruct(  9,  0, 0, 127, 1009, 0, 0, 0, 0, 15, 10);
	init_scenePropStruct( 10,  0, 1, 134, 501, 0, 0, 0, 0, 25, 11);
	init_scenePropStruct( 11,  0, 1, 135, 502, 0, 0, 0, 0, 15, 0);
	init_scenePropStruct( 12,  1, -1, 0, 0, 0xFF, 0xFF, 0xFF, 0, 0, 1);
	init_scenePropStruct( 13,  1, 0, 291, 135, 0xFF, 0xFF, 0xFF, 0, 25, 0);
	init_scenePropStruct( 14,  2, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 15,  2, 1, 277, 17, 0xFC, 0, 0xFC, 0, 56, 2);
	init_scenePropStruct( 16,  2, 0, 288, 18, 0xFF, 0xFF, 0xFF, 0, 56, 3);
	init_scenePropStruct( 17,  2, 1, 278, 19, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 18,  3, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 19,  3, 1, 282, 23, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 20,  4, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 21,  4, 1, 283, 24, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 22,  5, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 23,  5, 1, 284, 25, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 24,  6, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 25,  6, 1, 285, 26, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 26,  7, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 27,  7, 1, 286, 27, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 28,  8, -1, 0, 0, 0xFC, 0, 0xFC, 0, 0, 1);
	init_scenePropStruct( 29,  8, 1, 287, 28, 0xFC, 0, 0xFC, 0, 56, 0);
	init_scenePropStruct( 30,  9, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 31,  9, 1, 261, 1, 0xFC, 0, 0, 0, 40, 2);
	init_scenePropStruct( 32,  9, 1, 262, 2, 0xFC, 0, 0, 0, 40, 3);
	init_scenePropStruct( 33,  9, 1, 263, 3, 0xFC, 0, 0, 0, 40, 0);
	init_scenePropStruct( 34, 10, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 35, 10, 1, 263, 3, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 36, 11, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 37, 11, 1, 264, 4, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 38, 12, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 39, 12, 1, 265, 5, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 40, 13, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 41, 13, 1, 266, 6, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 42, 14, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 43, 14, 1, 267, 7, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 44, 15, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 45, 15, 1, 268, 8, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 46, 16, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 47, 16, 1, 274, 14, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 48, 17, -1, 0, 0, 0xFC, 0, 0, 0, 0, 1);
	init_scenePropStruct( 49, 17, 1, 270, 10, 0xFC, 0, 0, 0, 30, 0);
	init_scenePropStruct( 50, 18, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 51, 18, 0, 289, 45, 0xFF, 0xFF, 0xFF, 0, 40, 2);
	init_scenePropStruct( 52, 18, 1, 177, 49, 0xFC, 0xFC, 0x54, 0, 40, 3);
	init_scenePropStruct( 53, 18, 1, 178, 50, 0xFC, 0xFC, 0x54, 0, 40, 4);
	init_scenePropStruct( 54, 18, 0, 290, 47, 0xFF, 0xFF, 0xFF, 0, 40, 0);
	init_scenePropStruct( 55, 19, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 56, 19, 1, 179, 51, 0xFC, 0xFC, 0x54, 0, 40, 0);
	init_scenePropStruct( 57, 20, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 58, 20, 1, 183, 55, 0xFC, 0xFC, 0x54, 0, 40, 0);
	init_scenePropStruct( 59, 21, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 60, 21, 1, 184, 56, 0xFC, 0xFC, 0x54, 0, 40, 0);
	init_scenePropStruct( 61, 22, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 62, 22, 1, 186, 58, 0xFC, 0xFC, 0x54, 0, 40, 0);
	init_scenePropStruct( 63, 23, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 64, 23, 1, 191, 63, 0xFC, 0xFC, 0x54, 0, 40, 0);
	init_scenePropStruct( 65, 24, -1, 0, 0, 0xFC, 0xFC, 0x54, 0, 0, 1);
	init_scenePropStruct( 66, 24, 1, 192, 64, 0xFC, 0xFC, 0x54, 0, 40, 0);
	init_scenePropStruct( 67, 25, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 68, 25, 1, 220, 93, 0xBC, 0x78, 0x48, 0, 40, 2);
	init_scenePropStruct( 69, 25, 1, 221, 94, 0xBC, 0x78, 0x48, 0, 40, 3);
	init_scenePropStruct( 70, 25, 1, 222, 95, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 71, 26, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 72, 26, 1, 223, 96, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 73, 27, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 74, 27, 1, 224, 97, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 75, 28, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 76, 28, 1, 225, 98, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 77, 29, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 78, 29, 1, 226, 99, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 79, 30, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 80, 30, 1, 228, 101, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 81, 31, -1, 0, 0, 0xBC, 0x78, 0x48, 0, 0, 1);
	init_scenePropStruct( 82, 31, 1, 229, 102, 0xBC, 0x78, 0x48, 0, 40, 0);
	init_scenePropStruct( 83, 32, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct( 84, 32, 1, 233, 106, 0xA8, 0xA8, 0xA8, 0, 40, 2);
	init_scenePropStruct( 85, 32, 1, 234, 107, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct( 86, 33, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct( 87, 33, 1, 241, 114, 0xA8, 0xA8, 0xA8, 0, 40, 2);
	init_scenePropStruct( 88, 33, 1, 242, 115, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct( 89, 34, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct( 90, 34, 1, 237, 110, 0xA8, 0xA8, 0xA8, 0, 40, 2);
	init_scenePropStruct( 91, 34, 1, 238, 111, 0xA8, 0xA8, 0xA8, 0, 40, 3);
	init_scenePropStruct( 92, 34, 1, 239, 112, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct( 93, 35, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct( 94, 35, 1, 258, 131, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct( 95, 36, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct( 96, 36, 1, 260, 133, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct( 97, 37, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct( 98, 37, 1, 252, 125, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct( 99, 38, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct(100, 38, 1, 254, 127, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct(101, 39, -1, 0, 0, 0xA8, 0xA8, 0xA8, 0, 0, 1);
	init_scenePropStruct(102, 39, 1, 236, 109, 0xA8, 0xA8, 0xA8, 0, 40, 0);
	init_scenePropStruct(103, 40, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(104, 40, 1, 174, 42, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(105, 41, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(106, 41, 1, 167, 36, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(107, 42, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(108, 42, 1, 160, 29, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(109, 43, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(110, 43, 1, 161, 30, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(111, 44, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(112, 44, 1, 163, 32, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(113, 45, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(114, 45, 1, 164, 33, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(115, 46, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(116, 46, 1, 170, 39, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(117, 47, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(118, 47, 1, 166, 35, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(119, 48, -1, 0, 0, 4, 0xBC, 0, 0, 0, 1);
	init_scenePropStruct(120, 48, 1, 175, 43, 4, 0xBC, 0, 0, 40, 0);
	init_scenePropStruct(121, 49, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(122, 49, 1, 203, 75, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(123, 50, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(124, 50, 1, 194, 66, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(125, 51, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(126, 51, 1, 195, 67, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(127, 52, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(128, 52, 1, 199, 71, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(129, 53, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(130, 53, 1, 205, 77, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(131, 54, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(132, 54, 1, 212, 85, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(133, 55, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(134, 55, 1, 201, 73, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(135, 56, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(136, 56, 1, 198, 70, 0x40, 0x40, 0xFC, 0, 40, 0);
	init_scenePropStruct(137, 57, -1, 0, 0, 0x40, 0x40, 0xFC, 0, 0, 1);
	init_scenePropStruct(138, 57, 0, 59, 134, 0xFF, 0xFF, 0xFF, 0, 30, 0);

	_actor[0].damage = 0;
	_actor[0].maxdamage = 80;
	_actor[0].field_8 = 1;
	_actor[0].frame = 0;
	_actor[0].tilt = 0;
	_actor[0].cursorX = 0;
	_actor[0].speed = 0;
	_actor[0].x = 160;
	_actor[0].y = 0;
	_actor[0].y1 = -1;
	_actor[0].x1 = -1;
	_actor[0].weaponClass = 2;
	_actor[0].animWeaponClass = 0;
	_actor[0].field_34 = 2;
	_actor[0].field_38 = 0;
	_actor[0].lost = 0;
	_actor[0].kicking = 0;
	_actor[0].field_44 = 0;
	_actor[0].field_48 = 0;
	_actor[0].defunct = 0;
	_actor[0].scenePropSubIdx = 0;
	_actor[0].field_54 = 0;
	_actor[0].runningSound = 0;
	_actor[0].weapon = INV_HAND;
	_actor[0].inventory[INV_CHAIN] = 0;
	_actor[0].inventory[INV_CHAINSAW] = 0;
	_actor[0].inventory[INV_MACE] = 0;
	_actor[0].inventory[INV_2X4] = 0;
	_actor[0].inventory[INV_WRENCH] = 1;
	_actor[0].inventory[INV_BOOT] = 1;
	_actor[0].inventory[INV_HAND] = 1;
	_actor[0].inventory[INV_DUST] = 0;
	_actor[0].probability = 5;
	_actor[0].enemyHandler = EN_BEN;
	init_actStruct(0, 0, 11, 1, 1, 0, 0, 0);
	init_actStruct(0, 1, 12, 1, 1, 0, 0, 0);
	init_actStruct(0, 2, 1,  1, 1, 0, 0, 0);
	init_actStruct(0, 3, 1,  1, 1, 0, 0, 0);

	_actor[1].damage = 0;
	_actor[1].maxdamage = -1;
	_actor[1].field_8 = 1;
	_actor[1].frame = 0;
	_actor[1].tilt = 0;
	_actor[1].cursorX = 0;
	_actor[1].speed = 0;
	_actor[1].x = 160;
	_actor[1].y = 0;
	_actor[1].y1 = -1;
	_actor[1].x1 = -1;
	_actor[1].weaponClass = 2;
	_actor[1].animWeaponClass = 0;
	_actor[1].field_34 = 0;
	_actor[1].field_38 = 0;
	_actor[1].lost = 0;
	_actor[1].kicking = 0;
	_actor[1].field_44 = 0;
	_actor[1].field_48 = 0;
	_actor[1].defunct = 0;
	_actor[1].scenePropSubIdx = 0;
	_actor[1].field_54 = 0;
	_actor[1].runningSound = 0;
	_actor[1].weapon = INV_HAND;
	_actor[1].inventory[INV_CHAIN] = 0;
	_actor[1].inventory[INV_CHAINSAW] = 0;
	_actor[1].inventory[INV_MACE] = 1;
	_actor[1].inventory[INV_2X4] = 0;
	_actor[1].inventory[INV_WRENCH] = 0;
	_actor[1].inventory[INV_BOOT] = 0;
	_actor[1].inventory[INV_HAND] = 0;
	_actor[1].inventory[INV_DUST] = 0;
	_actor[1].probability = 5;
	_actor[1].enemyHandler = -1;

	init_actStruct(1, 0, 14, 1, 1, 0, 0, 0);
	init_actStruct(1, 1, 15, 1, 1, 0, 0, 0);
	init_actStruct(1, 2, 13, 1, 1, 0, 0, 0);
	init_actStruct(1, 3, 13, 1, 1, 0, 0, 0);

	for (i = 0; i < 9; i++)
		for (j = 0; j < 10; j++)
			_enemyState[i][j] = 0;
}

void Insane::init_actStruct(int actornum, int actnum, int32 actorval, byte state, 
								  int32 room, int32 animTilt, int32 tilt, int32 frame) {
	_actor[actornum].act[actnum].actor = actorval;
	_actor[actornum].act[actnum].state = state;
	_actor[actornum].act[actnum].room = room;
	_actor[actornum].act[actnum].animTilt = animTilt;
	_actor[actornum].act[actnum].tilt = tilt;
	_actor[actornum].act[actnum].frame = frame;
}

void Insane::init_enemyStruct(int n, int32 handler, int32 initializer,
								   int32 field_8, int32 maxdamage, int32 field_10,
								   int32 weapon, int32 sound, const char *filename,
								   int32 costume4, int32 costume6, int32 costume5,
								   int16 costumevar, int32 maxframe, int32 field_34) {
	assert(strlen(filename) < 20);

	_enemy[n].handler = handler;
	_enemy[n].initializer = initializer;
	_enemy[n].field_8 = field_8;
	_enemy[n].maxdamage = maxdamage;
	_enemy[n].field_10 = field_10;
	_enemy[n].weapon = weapon;
	_enemy[n].sound = sound;
	strncpy(_enemy[n].filename, filename, 20);
	_enemy[n].costume4 = costume4;
	_enemy[n].costume6 = costume6;
	_enemy[n].costume5 = costume5;
	_enemy[n].costumevar = costumevar;
	_enemy[n].maxframe = maxframe;
	_enemy[n].field_34 = field_34;
}

void Insane::init_fluConfStruct(int n, int sceneId, byte **fluPtr, 
					const char *filenamePtr, int startFrame, int numFrames) {
	_fluConf[n].sceneId = sceneId;
	_fluConf[n].fluPtr = fluPtr;
	_fluConf[n].filenamePtr = filenamePtr;
	_fluConf[n].startFrame = startFrame;
	_fluConf[n].numFrames = numFrames;
}

void Insane::init_scenePropStruct(int32 n, int32 n1, int32 actornum, int32 sound, int32 trsId,
					  byte r, byte g, byte b, int32 counter, int32 maxCounter,
					  int32 index) {
	_sceneProp[n].actor = actornum; // main actor number, -1 if not applicable
	_sceneProp[n].sound = sound;
	_sceneProp[n].trsId = trsId;
	_sceneProp[n].r = r;
	_sceneProp[n].g = g;
	_sceneProp[n].b = b;
	_sceneProp[n].counter = counter;
	_sceneProp[n].maxCounter = maxCounter;
	_sceneProp[n].index = index;
}

void Insane::setBenAnimation(int32 actornum, int anim) {
	if (anim <= 12)
		smlayer_setActorFacing(actornum, 1, 
			  actorAnimationData[_actor[actornum].weaponClass * 7 + anim - 6], 180);
}

void Insane::setEnemyAnimation(int32 actornum, int anim) {
	int d = 0;

	if (_currEnemy == EN_VULTM1)
		d = 14;

	if (anim <= 12)
		smlayer_setActorFacing(actornum, 1, 
			  actorAnimationData[_actor[actornum].weaponClass * 7 + anim - 6 + d], 180);
}

int32 Insane::processMouse(void) {
	int32 buttons = 0;

	_enemyState[EN_BEN][0] = _vm->_mouse.x;
	_enemyState[EN_BEN][1] = _vm->_mouse.y;

	buttons = _vm->VAR(_vm->VAR_LEFTBTN_HOLD) ? 1 : 0;
	buttons |= _vm->VAR(_vm->VAR_RIGHTBTN_HOLD) ? 2 : 0;

	return buttons;
}

int32 Insane::processKeyboard(void) {
	int32 retval = 0;
	int dx = 0, dy = 0;
	int tmpx, tmpy;

	if (_vm->getKeyState(0x14f) || _vm->getKeyState(0x14b) || _vm->getKeyState(0x147))
		dx--;

	if (_vm->getKeyState(0x151) || _vm->getKeyState(0x14d) || _vm->getKeyState(0x149))
		dx++;

	if (_vm->getKeyState(0x147) || _vm->getKeyState(0x148) || _vm->getKeyState(0x149))
		dy--;

	if (_vm->getKeyState(0x14f) || _vm->getKeyState(0x150) || _vm->getKeyState(0x151))
		dy++;

	if (dx == _keybOldDx)
		_velocityX += 4;
	else
		_velocityX = 3;

	if (dy == _keybOldDy)
		_velocityY += 4;
	else
		_velocityY = 2;

	_keybOldDx = dx;
	_keybOldDy = dy;

	if (_velocityX > 48)
		_velocityX = 48;

	if (_velocityY > 32)
		_velocityY = 32;

	_keybX += dx * _velocityX;
	_keybY += dy * _velocityY;

	tmpx = _keybX / 4;
	tmpy = _keybY / 4;

	_keybX -= tmpx * 4;
	_keybY -= tmpy * 4;

	if (tmpx || tmpy) {
		_enemyState[EN_BEN][0] += tmpx;
		_enemyState[EN_BEN][1] += tmpy;
	}

	if (_vm->getKeyState(0x0d))
		retval |= 1;

	if (_vm->getKeyState(0x09))
		retval |= 2;

	return retval;
}

void Insane::readFileToMem(const char *name, byte **buf) {
	File *in;
	uint32 len;

	in = new File();
	in->open(name, _vm->getGameDataPath());
	len = in->size();
	*buf = (byte *)malloc(len);
	in->read(*buf, len);
	in->close();
}

void Insane::startVideo(const char *filename, int num, int argC, int frameRate, 
						 int doMainLoop, byte *fluPtr, int32 numFrames) {

	// Demo has different insane, so disable it now
	if (_vm->_features & GF_DEMO)
		return;

	_smush_curFrame = 0;
	_smush_isSanFileSetup = 0;
	_smush_setupsan4 = 0;
	_smush_smushState = 0;
	_smush_setupsan1 = 0;
	_smush_setupsan17 = 0;


	if (fluPtr) {
		smush_setupSanWithFlu(filename, 0, -1, -1, 0, fluPtr, numFrames);
	} else {
		smush_setupSanFromStart(filename, 0, -1, -1, 0);
	}

	_player->play(filename, _vm->getGameDataPath());
}

int Insane::smlayer_mainLoop(void) {
	// FIXME: implement
	warning("stub Insane::smlayer_mainLoop");
	mainLoop();
	return 1;
}

void Insane::smush_proc39(void) {
	// FIXME: implement
	warning("stub Insane::smush_proc39");
}

void Insane::smush_proc40(void) {
	// FIXME: implement
	warning("stub Insane::smush_proc40");
}

void Insane::smush_proc41(void) {
	// FIXME: implement
	warning("stub Insane::smush_proc41");
}

void Insane::smush_warpMouse(int x, int y, int buttons) {
	_vm->_system->warp_mouse(x, y);
}

void Insane::putActors(void) {
	smlayer_putActor(0, 2, _actor[0].x, _actor[0].y1, _smlayer_room);
	smlayer_putActor(0, 0, _actor[0].x, _actor[0].y1, _smlayer_room);
	smlayer_putActor(0, 1, _actor[0].x, _actor[0].y1, _smlayer_room);
	smlayer_putActor(1, 2, _actor[0].x, _actor[0].y1, _smlayer_room);
	smlayer_putActor(1, 0, _actor[0].x, _actor[0].y1, _smlayer_room);
	smlayer_putActor(1, 1, _actor[0].x, _actor[0].y1, _smlayer_room);
}

void Insane::readState(void) {
	_actor[0].inventory[INV_CHAIN] = readArray(50) != 0;
	_actor[0].inventory[INV_CHAINSAW] = readArray(51) != 0;
	_actor[0].inventory[INV_MACE] = readArray(52) != 0;
	_actor[0].inventory[INV_2X4] = readArray(53) != 0;
	_actor[0].inventory[INV_WRENCH] = readArray(54) != 0;
	_actor[0].inventory[INV_DUST] = readArray(55) != 0;
	_actor[0].inventory[INV_HAND] = 1; // Boot
	_actor[0].inventory[INV_BOOT] = 1; // Hand

	_smlayer_room = readArray(320);
	_smlayer_room2 = readArray(321);
	_val55d = readArray(322);
	_val53d = readArray(323);
	_val57d = readArray(324);
	_val52d = readArray(325);
	_val56d = readArray(326);
	_val54d = readArray(327);
	_val51d = readArray(328);
	_enemy[EN_TORQUE].field_8 = readArray(337);
	_enemy[EN_ROTT1].field_8 = readArray(329);
	_enemy[EN_ROTT2].field_8 = readArray(330);
	_enemy[EN_ROTT3].field_8 = readArray(331);
	_enemy[EN_VULTF1].field_8 = readArray(332);
	_enemy[EN_VULTM1].field_8 = readArray(333);
	_enemy[EN_VULTF2].field_8 = readArray(334);
	_enemy[EN_VULTM2].field_8 = readArray(335);
	_enemy[EN_CAVEFISH].field_8 = readArray(336);
	_enemy[EN_VULTM2].field_10 = readArray(340);
	_enemy[EN_CAVEFISH].field_10 = readArray(56);
	_enemy[EN_VULTF2].field_10 = readArray(339);
}

void Insane::setTrsFile(int file) {
	// FIXME: we don't need it
}

void Insane::resetTrsFilePtr(void) {
	// FIXME: we don't need it
}

void Insane::setupValues(void) {
	_actor[0].x = 160;
	_actor[0].y = 200;
	_actor[0].tilt = 0;
	_actor[0].field_8 = 1;
	_actor[0].frame = 0;
	_actor[0].act[2].state = 1;
	_actor[0].act[0].state = 1;
	_actor[0].act[1].state = 0;
	_actor[0].act[2].room = 1;
	_actor[0].act[1].room = 0;
	_actor[0].act[0].room = 0;
	_actor[0].cursorX = 0;
	_actor[0].lost = 0;
	_currEnemy = -1;
	_val32d = -1;
	smush_warpMouse(160, 100, -1);
}

// FIXME: it seems that in ScummVM it may be unused
void Insane::mainLoop(void) {
	int32 resid;

	_val11d++;

	while (!idx2Compare()) {
		if(!(resid = idx2Tweak()))
			continue;
	  
		_vm->ensureResourceLoaded(rtSound, resid);
		_vm->setResourceCounter(rtSound, resid, 1);
	}
	_vm->increaseResourceCounter();
	
	while (!idx1Compare()) {
		resid = idx1Tweak();
		_vm->ensureResourceLoaded(rtCostume, resid);
		_vm->setResourceCounter(rtCostume, resid, 1);
		// smlayer_lock (rtCostume, resid);
	}
	
	if (loadSceneData(_currSceneId, 0, 2)) {
		_sceneData1Loaded = 0;
		_objArray2[0] = 0;
		return;
	}
	_sceneData1Loaded = 1;
	if (idx1Compare() || idx2Compare()) {
		_objArray2[0] = 0;
		return;
	}
	_objArray2[0]++;
	if (_objArray2[0] <= 5) {
		_objArray2[0] = 0;
		return;
	}
	
	smush_setToFinish();
	loadSceneData(_currSceneId, 1, 2);
	_memoryAllocatedNotOK = 1;
}

bool Insane::idx1Compare(void) {
	return _objArray1Idx == _objArray1Idx2;
}

bool Insane::idx2Compare(void) {
	return _objArray2Idx == _objArray2Idx2;
}

int32 Insane::idx1Tweak(void) {
	_objArray1Idx++;
	if (_objArray1Idx >= 100)
		_objArray1Idx = 0;
	
	return _objArray1[_objArray1Idx];
}

int32 Insane::idx2Tweak(void) {
	if (!_idx2Exceeded)
		if (_objArray2Idx >= _objArray2Idx2)
			return false;
	
	_objArray2Idx++;
	if (_objArray2Idx >= 100) {
		_idx2Exceeded = 0;
		_objArray2Idx = 0;
	}
	return _objArray2[_objArray2Idx];
}

void Insane::smush_setToFinish(void) {
	debug(0, "Video is set to finish");
	_vm->_videoFinished = 1;
}

// smlayer_stopSound
void Insane::smlayer_stopSound(int idx) {
	_vm->_sound->stopSound(readArray(idx));
}

void Insane::switchSceneIfNeeded(void) {
	if (_needSceneSwitch && !_smush_isSanFileSetup) {
		putActors();
		stopSceneSounds(_currSceneId);
		_tempSceneId = _currSceneId;
		_currSceneId = _temp2SceneId;
		_needSceneSwitch = false;
		loadSceneData(_temp2SceneId, 0, 1);

		if(loadSceneData(_temp2SceneId, 0, 2)) {
			setSceneCostumes(_temp2SceneId);
			_sceneData2Loaded = 0;
			_sceneData1Loaded = 0;
			return;
		}

		_sceneData2Loaded = 1;
		if (_temp2SceneId == 13 || _temp2SceneId == 3)
			_isBenCut = 1;
	}

	if (_sceneData2Loaded && !_sceneData1Loaded) {
		setSceneCostumes(_currSceneId);
		_sceneData2Loaded = 0;
	}
}

void Insane::prepareScenePropScene(int32 scenePropNum, bool arg_4, bool arg_8) {
	static const int scenePropIdx[58] = {0,  12,  14,  18,  20,  22,  24,  26,  28,  30,  34,
	 36,  38,  40,  42,  44,  46,  48,  50,  55,  57,  59,  61,  63,  65,  67,  71,
	 73,  75,  77,  79,  81,  83,  85,  89,  93,  95,  97,  99, 101, 103, 105, 107,
	 109, 111, 113, 115, 117, 119, 121, 123, 125, 127, 129, 131, 133, 135, 137};

	int tmp, idx = scenePropIdx[scenePropNum];

	if (!loadScenePropSounds(idx))
		return;

	_actor[0].defunct = arg_4;
	_actor[1].defunct = arg_8;
	_currScenePropIdx = idx;
	_sceneProp[idx + 1].counter = 0;
	_currScenePropSubIdx = 1;
	if (_sceneProp[idx + 1].trsId)
		_currTrsMsg = handleTrsTag(_trsFilePtr, _sceneProp[idx + 1].trsId);
	else
		_currTrsMsg = 0;

	tmp = _sceneProp[idx + 1].actor;
	if (tmp != -1) {
		_actor[tmp].field_54 = 1;
		_actor[tmp].act[3].state = 117;
		_actor[tmp].scenePropSubIdx = _currScenePropSubIdx;
	}
}

int Insane::smush_changeState(int state) {
	if (state == 2) {
		if (_smush_smushState == 0)
			_smush_smushState = 1;
		else
			_smush_smushState = 0;
	} else if (state == 4) {
		if (_smush_smushState)
			_smush_smushState = 3;
	} else if (state != 5) {
		_smush_smushState = state;
	}
	return _smush_smushState;
}

void Insane::queueSceneSwitch(int32 sceneId, byte *fluPtr, const char *filename, 
							  int32 arg_C, int32 arg_10, int32 startFrame, int32 numFrames) {
	int32 tmp;
	
	debug(0, "queueSceneSwitch(%d, *, %s, %d, %d, %d, %d)", sceneId, filename, arg_C, arg_10,
		  startFrame, numFrames);
	if (_needSceneSwitch || _sceneData1Loaded)
		return;
	
	if (fluPtr) {
		tmp = ((int)startFrame/30+1)*30;
		if (tmp >= numFrames)
			tmp = 0;
	
		smush_setupSanWithFlu(filename, arg_C|32, -1, -1, 0, fluPtr, tmp);
	} else {
		smush_setupSanFromStart(filename, arg_C|32, -1, -1, 0);
	}
	_needSceneSwitch = true;
	_temp2SceneId = sceneId;
}

void Insane::smush_rewindCurrentSan(int arg_0, int arg_4, int arg_8) {
	debug(0, "smush_rewindCurrentSan(%d, %d, %d)", arg_0, arg_4, arg_8);
	_smush_setupsan2 = arg_0;
	
	smush_setupSanFile(0, 8, 0);
	_smush_isSanFileSetup = 1;
	smush_setFrameSteps(arg_4, arg_8);

	_smush_curFrame = 0; // HACK
}

int32 Insane::weaponMaxRange(int32 actornum) {
	static int map[8] = {135, 125, 130, 125, 120, 104, 104, 104};

	if (_actor[actornum].weapon == -1)
		return 104;
	
	return map[_actor[actornum].weapon];
}

int32 Insane::weaponMinRange(int32 actornum) {
	static int map[8] = {80, 40, 80, 40, 80, 80, 40, 50};

	if (_actor[actornum].weapon == -1)
		return 40;
	
	return map[_actor[actornum].weapon];
}

int32 Insane::weaponDamage(int32 actornum) {
	static int map[8] = {20, 300, 25, 40, 15, 10, 10, 5};

	if (_actor[actornum].weapon == -1)
		return 10;
	
	return map[_actor[actornum].weapon];
}

void Insane::reinitActors(void) {
	smlayer_setActorCostume(0, 2, readArray(12));
	smlayer_setActorCostume(0, 0, readArray(14));
	smlayer_setActorCostume(0, 1, readArray(13));
	smlayer_setActorLayer(0, 1, 1);
	smlayer_setActorLayer(0, 2, 5);
	smlayer_setActorLayer(0, 0, 10);
	_actor[0].weapon = INV_HAND;
	_actor[0].weaponClass = 2;
	_actor[0].animWeaponClass = 0;
	_actor[0].field_34 = 2;
	_actor[0].field_38 = 0;
	_actor[0].tilt = 0;
	_actor[0].field_8 = 1;
	_actor[0].act[2].state = 1;
	_actor[0].act[2].animTilt = 1;
	_actor[0].act[0].state = 0;
	_actor[0].act[1].state = 1;
	_actor[0].act[2].room = 1;
	_actor[0].act[1].room = 1;
	_actor[0].act[0].room = 1;
	_actor[0].cursorX = 0;
}

int Insane::calcTilt(int speed) {
	const int tilt[7] = {-5, -4, -2, 0, 2, 4, 5};
	if (speed + 3 > 6)
		return 0;
	
	return tilt[speed + 3];
}

bool Insane::actor1StateFlags(int state) {
	// This is compressed table. It contains indexes where state
	// changes. I.e. 0-33: true, 34-38: false 39-72: true, etc.
	const int spans[] = {0, 34, 39, 73, 89, 90, 92, 93, 99, 100, 117};
	bool retvalue = 0;
	unsigned int i;
	
	for (i = 0; i < sizeof(spans); i++) {
		retvalue = !retvalue;
		if (spans[i] <= state)
			break;
	}
	return retvalue;
}

void Insane::escapeKeyHandler(void) {
	struct fluConf *flu;

	//if (!_ptrMainLoop) { } // We don't need it
 	// Demo has different insane, so disable it now
	if (!_insaneIsRunning || _vm->_features & GF_DEMO) {
		smush_setToFinish();
		return;
	}

	if (_needSceneSwitch || _keyboardDisable)
		return;

	debug(0, "scene: %d", _currSceneId);
	switch (_currSceneId) {
	case 1:
		queueSceneSwitch(1, _smush_minedrivFlu, "minedriv.san", 64, 0, _continueFrame1, 1300);
		writeArray(9, 0);
		break;
	case 18:
		queueSceneSwitch(1, _smush_minedrivFlu, "minedriv.san", 64, 0, _continueFrame1, 1300);
		writeArray(9, 1);
		break;
	case 2:
		flu = &_fluConf[14 + _iactSceneId2];
		queueSceneSwitch(flu->sceneId, *flu->fluPtr, flu->filenamePtr, 64, 0, 
						 flu->startFrame, flu->numFrames);
		break;
	case 3:
		queueSceneSwitch(1, _smush_minedrivFlu, "minedriv.san", 64, 0, _continueFrame1, 1300);
		break;
	case 4:
		if (_needSceneSwitch)
			return;
		
		if (readArray(6)) {
			if (readArray(4)) {
				queueSceneSwitch(14, 0, "hitdust2.san", 64, 0, 0, 0);
			} else {
				queueSceneSwitch(14, 0, "hitdust4.san", 64, 0, 0, 0);
			}
		} else {
			if (readArray(4)) {
				queueSceneSwitch(14, 0, "hitdust1.san", 64, 0, 0, 0);
			} else {
				queueSceneSwitch(14, 0, "hitdust3.san", 64, 0, 0, 0);
			}
		}
		break;
	case 5:
		if (readArray(4)) {
			if (_needSceneSwitch)
				return;
			queueSceneSwitch(15, 0, "vistthru.san", 64, 0, 0, 0);
		} else {
			writeArray(1, _val53d);
			smush_setToFinish();
		}
		break;
	case 6:
		if (readArray(4)) {
			if (_needSceneSwitch)
				return;
			queueSceneSwitch(15, 0, "chasthru.san", 64, 0, 0, 0);
		} else {
			if (readArray(5)) {
				writeArray(1, _val57d);
				smush_setToFinish();
			} else {
				writeArray(4, 1);
				queueSceneSwitch(15, 0, "chasout.san", 64, 0, 0, 0);
			}
		}
		break;
	case 8:
		flu = &_fluConf[7 + _iactSceneId2];
		queueSceneSwitch(flu->sceneId, *flu->fluPtr, flu->filenamePtr, 64, 0, 
						 flu->startFrame, flu->numFrames);
		break;
	case 7:
		flu = &_fluConf[0 + _iactSceneId2];
		queueSceneSwitch(flu->sceneId, *flu->fluPtr, flu->filenamePtr, 64, 0, 
						 flu->startFrame, flu->numFrames);
		break;
	case 23:
		_actor[0].damage = 0;
		queueSceneSwitch(21, 0, "rottfite.san", 64, 0, 0, 0);
		break;
	case 9:
		_actor[0].damage = 0;
		queueSceneSwitch(1, _smush_minedrivFlu, "minedriv.san", 64, 0, _continueFrame, 1300);
		break;
	case 10:
		_actor[0].damage = 0;
		queueSceneSwitch(1, _smush_minedrivFlu, "minedriv.san", 64, 0, _continueFrame1, 1300);
		break;
	case 13:
		queueSceneSwitch(1, _smush_minedrivFlu, "minedriv.san", 64, 0, _continueFrame, 1300);
		break;
	case 24:
		queueSceneSwitch(21, 0, "rottfite.san", 64, 0, 0, 0);
		break;
	case 16:
		writeArray(4, 0);
		writeArray(5, 1);
		writeArray(1, _val56d);
		writeArray(3, _val55d);
		smush_setToFinish();
		break;
	case 15:
		switch (_tempSceneId) {
		case 5:
			queueSceneSwitch(6, 0, "toranch.san", 64, 0, 0, 530);
			break;
		case 6:
			queueSceneSwitch(4, 0, "tovista1.san", 64, 0, 0, 230);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

bool Insane::actor0StateFlags1(int state) {
	const int spans[] = {0, 2, 34, 35, 39, 69, 98, 100, 117};
	bool retvalue = 0;
	unsigned int i;
	
	for (i = 0; i < sizeof(spans); i++) {
		retvalue = !retvalue;
		if (spans[i] <= state)
			break;
	}
	return retvalue;
}

bool Insane::actor0StateFlags2(int state) {
	const int spans[] = {0, 10, 14, 34, 39, 73, 75, 79, 81, 90, 93, 94,
		 98, 100, 117, 133, 136, 153, 158, 200, 202, 209, 212, 213, 217,
		 219, 236, 256, 259, 272, 277, 311, 312, 315, 317, 328, 331, 332,
		 336, 338, 355, 379, 382, 391, 396, 440, 441, 447, 450, 451, 455,
		 457, 474, 502, 505, 510, 515, 549, 553, 566, 569, 570, 574, 576,
		 593, 601, 604, 629, 634, 680, 682, 685, 688, 689, 693, 695, 712,
		 716, 718, 748, 753, 787, 788, 804, 807, 808, 812, 814, 831, 863,
		 866, 867, 872, 920, 922, 923, 926, 927, 931, 933, 950};
	bool retvalue = 0;
	unsigned int i;
	
	for (i = 0; i < sizeof(spans); i++) {
		retvalue = !retvalue;
		if (spans[i] <= state)
			break;
	}
	return retvalue;
}

// smlayer_loadSound1 && smlayer_loadSound2
int Insane::smlayer_loadSound(int id, int flag, int phase) {
	int resid;
	
	if (phase == 1) {
		if (_idx2Exceeded != 0)
			if (_objArray2Idx >= _objArray2Idx2)
				return 0;
	}
	resid = readArray(id);
	
	if (!resid && phase == 2)
		return 0;
	
	if (phase == 2)
		_vm->ensureResourceLoaded(rtSound, resid);
	
	_vm->setResourceCounter(rtSound, resid, 1);
	
	if (phase == 1) {
		_objArray2Idx2++;
		_objArray2[_objArray2Idx2] = id;
		if (_objArray2Idx2 >= 100) {
			_idx2Exceeded = 1;
			_objArray2Idx2 = 0;
		}
	}
	return resid;
}

void Insane::IMUSE_shutVolume(void) {
	debug(0, "***************************************************");
	_vm->_imuse->pause(true);
}

void Insane::IMUSE_restoreVolume(void) {
	debug(0, "***************************************************");
	_vm->_imuse->pause(false);
}

// smlayer_loadCostume1 && smlayer_loadCostume2
int Insane::smlayer_loadCostume(int id, int phase) {
	int resid;
	resid = readArray(id);
	
	if (!resid)
		return 0;
	
	_vm->ensureResourceLoaded(rtCostume, resid);
	_vm->setResourceCounter(rtCostume, resid, 1);
	
	// smlayer_lock(rtCostume, resid); // FIXME
	
	if (phase == 1) {
		_objArray1Idx2++;
		_objArray1[_objArray1Idx2] = id;
		if (_objArray1Idx2 == 100)
			_objArray1Idx2 = 0;
	}
	
	return resid;
}

void Insane::smlayer_setActorCostume(int actornum, int actnum, int costume) {
	Actor *a = _vm->derefActor(_actor[actornum].act[actnum].actor, "smlayer_setActorCostume");
	a->setActorCostume(costume);
	a->setDirection(180);
	a->startAnimActor(1);
	_actor[actornum].act[actnum].frame = 0;
}

void Insane::smlayer_putActor(int actornum, int actnum, int x, int y, byte room) {
	Actor *a = _vm->derefActor(_actor[actornum].act[actnum].actor, "smlayer_putActor");
	a->putActor(x, y, room);
}

void Insane::smlayer_setActorLayer(int actornum, int actnum, int layer) {
	Actor *a = _vm->derefActor(_actor[actornum].act[actnum].actor, "smlayer_setActorLayer");
	a->layer = layer;
}

void Insane::smlayer_setFluPalette(byte *pal, int shut_flag) {
	// FIXME. We can't run it without SmushPlayer object
	//	  if (shut_flag)
	//		// FIXME: shut colors and make picture appear smoothly
	//		SmushPlayer::setPalette(pal);
	//	  else
	_player->setPalette(pal);
}

bool Insane::smlayer_isSoundRunning(int32 sound) {
	return _vm->_sound->isSoundRunning(readArray(sound)) != 0;
}

bool Insane::smlayer_startSfx(int32 sound) {
	if (smlayer_loadSound(sound, 0, 2)) {
		_vm->_imuseDigital->startSfx(readArray(sound));
		return true;
	} else
		return false;
}

bool Insane::smlayer_startVoice(int32 sound) {
	if (smlayer_loadSound(sound, 0, 2)) {
		_vm->_imuseDigital->startSfx(readArray(sound));
		return true;
	} else
		return false;
}

void Insane::smlayer_soundSetPan(int32 soundid, int32 pan) {
	_vm->_imuseDigital->parseScriptCmds(12, soundid, 0x700, pan, 0, 0, 0, 0);
}

void Insane::smlayer_soundSetPriority(int32 sound, int32 priority) {
}

void Insane::smlayer_drawSomething(byte *renderBitmap, int32 codecparam, 
			   int32 x, int32 y, int32 arg_10, NutRenderer *nutfile, 
			   int32 c, int32 arg_1C, int32 arg_20) {
	nutfile->drawFrame(renderBitmap, c, x, y);
}

void Insane::smlayer_overrideDrawActorAt(byte *arg_0, byte arg_4, byte arg_8) {
	// FIXME: doublecheck

	// noop in current implementation
}

void Insane::smlayer_showStatusMsg(int32 arg_0, byte *renderBitmap, int32 codecparam, 
					   int32 x, int32 y, int32 arg_14, int32 arg_18, 
					   int32 arg_1C, const char *formatString, char *str) {
	// FIXME: implement
	// SmushPlayer::handleTextResource does the thing
	warning("stub Insane::smlayer_showStatusMsg(...)");
}

void Insane::procSKIP(Chunk &b) {
	_player->checkBlock(b, TYPE_SKIP, 4);
	int16 par1, par2;

	par1 = b.getWord();
	par2 = b.getWord();

	if (!par2) {
		if (isBitSet(par1))
			_player->_skipNext = true;

		return;
	}

	if (isBitSet(par1) != isBitSet(par2))
		_player->_skipNext = true;
}

bool Insane::isBitSet(int n) {
	if (n >= 0x80)
		return false;

	return (_iactBits[n / 8] & (0x80 >> (n % 8))) != 0;
}

void Insane::setBit(int n) {
	if (n >= 0x80)
		return;

	_iactBits[n / 8] |= 0x80 >> (n % 8);
}

void Insane::clearBit(int n) {
	if (n >= 0x80)
		return;

	_iactBits[n / 8] &= ~(0x80 >> (n % 8));
}

void Insane::smlayer_setActorFacing(int actornum, int actnum, int frame, int direction) {
	if (_actor[actornum].act[actnum].room) {
		Actor *a = _vm->derefActor(_actor[actornum].act[actnum].actor, "smlayer_setActorFacing");
		a->setDirection(direction);
		a->startAnimActor(frame);
		_actor[actornum].act[actnum].frame = 0;
	}
}

char *Insane::handleTrsTag(int32 trsFilePtr, int32 trsId) {
	// FIXME: implement
	warning("stub Insane::handleTrsTag(0, %d)", trsId);

	return 0;
}

bool Insane::smush_eitherNotStartNewFrame(void) {
	if (_smush_setupsan17)
		return false;
	
	if (_smush_isSanFileSetup) {
		if (_smush_frameStep < 0)
			return false;
	
		if (_smush_curFrame > _smush_frameStep + _smush_frameNum2)
			return true;
		else
			return false;
	} else {
		if (_smush_frameNum1 < 0)
			return false;
	
		if (_smush_curFrame >= _smush_frameNum1) {
			_smush_frameNum1 = -1;
			return false;
		} else
			return true;
	}
}

int32 Insane::getLastKey(bool arg_0) {
	return _vm->_lastKeyHit;
}

bool Insane::smlayer_actorNeedRedraw(int actornum, int actnum) {
	Actor *a = _vm->derefActor(_actor[actornum].act[actnum].actor, "smlayer_actorNeedRedraw");

	return a->needRedraw;
}

void Insane::smush_setPaletteValue(int where, int r, int g, int b) {
	// FIXME: implement
	warning("stub Insane::smlayer_setPaletteValue(%d, %d, %d, %d)", where, r, g, b);
}

int32 Insane::readArray (int item) {
	return _vm->readArray(_numberArray, 0, item);
}

void Insane::writeArray(int item, int value) {
	_vm->writeArray(_numberArray, 0, item, value);
}

void Insane::smush_setupSanWithFlu(const char *filename, int32 setupsan2, int32 step1, 
								   int32 step2, int32 setupsan1, byte *fluPtr, 
								   int32 numFrames) {
	byte *tmp = fluPtr;
	int32 offset;
	
	debug(0, "smush_setupSanWithFlu(%s, %d, %d, %d, %d, %lx, %d)", filename, setupsan2,
		  step1, step2, setupsan1, fluPtr, numFrames);

	_smush_setupsan1 = setupsan1;
	
	/* skip FLUP marker */
	if (READ_BE_UINT32(fluPtr) == 'FLUP')
		tmp += 8;

	_smush_setupsan2 = setupsan2;
	
	if (tmp[2] <= 1) {
		/* 0x300 -- palette, 0x8 -- header */
		offset = READ_LE_UINT32(tmp + 0x308 + numFrames*4);
		smush_setupSanFile(filename, offset, numFrames);
		memcpy(_smush_earlyFluContents, tmp+2, 0x306);
		_smush_earlyFluContents[0x30e] = 0;
		_smush_earlyFluContents[0x30f] = 0;
		_smush_earlyFluContents[0x310] = 0;
		_smush_earlyFluContents[0x311] = 0;
		_smush_earlyFluContents[0x306] = 0;
		_smush_earlyFluContents[0x307] = 0;
	} else {
		offset = READ_LE_UINT32(tmp + 0x31c + numFrames*4);
		smush_setupSanFile(filename, offset, numFrames);
		memcpy(_smush_earlyFluContents, tmp+2, 0x31a);
	}
	_smush_isSanFileSetup = 1;
	_smush_setupsan4 = 1;
	_smush_curFrame = numFrames;
	smush_setFrameSteps(step1, step2);
}

void Insane::smush_setupSanFromStart(const char *filename, int32 setupsan2, int32 step1, 
									 int32 step2, int32 setupsan1) {
	_smush_setupsan1 = setupsan1;
	_smush_setupsan2 = setupsan2;
	smush_setupSanFile(filename, 8, 0);
	_smush_isSanFileSetup = 1;
	smush_setFrameSteps(step1, step2);
}

void Insane::smush_setFrameSteps(int32 step1, int32 step2) {
	_smush_frameNum2 = _smush_curFrame;
	_smush_frameNum1 = step2;
	_smush_frameStep = step1;
}

	void Insane::smush_setupSanFile(const char *filename, int32 offset, int32 contFrame) {
	debug(0, "smush_setupSanFile(%s, %x)", filename, offset);

	_player->seekSan(filename, _vm->getGameDataPath(), offset, contFrame);

	_vm->_imuseDigital->pause(false);
}

}
