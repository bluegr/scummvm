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

#include "dune/intro.h"

namespace Dune {

void Intro::runIntro() {
	while (!_video->skipped()) {
		_music->playMusic(MORNING, false);
		_music->setFrameStop(3300);
		_video->playVideo(HNM_VIRGIN);
		reinitializeMidiMusic();
		_music->playMusic(CRYOMUS, false);
		_video->playVideo(HNM_CRYO);
		_video->playVideo(HNM_CRYO2);
		_video->playVideo(HNM_PRESENT);
		_video->playVideo(HNM_IRULAN);
		_video->playVideo(HNM_TITLE);
		_video->resetSkipped();
		_video->playVideo(HNM_CREDITS);
	}
	_video->resetSkipped();
}

void Intro::reinitializeMidiMusic() {
	_music->stopMusic();
	delete _music;
	_music = new MidiMusic(_vm);
}

Intro::Intro(DuneEngine *vm) : _vm(vm) {
	_video = new HnmPlayer(vm);
	_music = new MidiMusic(vm);
}

Intro::~Intro() {
	delete _video;
	delete _music;
}

} //End of namespace Dune

