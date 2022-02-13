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

#include "common/config-manager.h"
#include "common/events.h"
#include "common/util.h"

#include "dune/sound/music.h"
#include "dune/dune.h"
#include "dune/sound/midiadlib.h"
#include "audio/mixer.h"

namespace Dune {

MidiMusic::MidiMusic(DuneEngine *vm)
	: _masterVolume(192)
	{
	_vm = vm;
	MidiDriver::DeviceHandle dev = MidiDriver::detectDevice(MDT_MIDI | MDT_ADLIB | MDT_PREFER_MT32);
	_adlib = false; //(MidiDriver::getMusicType(dev) == MT_ADLIB);
	_nativeMT32 = true; //((MidiDriver::getMusicType(dev) == MT_MT32) || ConfMan.getBool("native_mt32"));

	if (_adlib) {
		_driver = new AdLibMidiDriver(_vm);
	} else {
		_driver = MidiDriver::createMidi(dev);
		if (_nativeMT32) {
			_driver->property(MidiDriver::PROP_CHANNEL_MASK, 0x03FE);
		}
	}
	assert(_driver);

	int ret = _driver->open();
	assert(ret == 0);
	_driver->setTimerCallback(this, &timerCallback);

	if (_nativeMT32)
		_driver->sendMT32Reset();
	else
		_driver->sendGMReset();
}

MidiMusic::~MidiMusic() {
	_driver->setTimerCallback(0, 0);
	delete _driver;
}

void MidiMusic::onTimer() {
	//NOP, for now...?
}


//TODO: Support AGD, and M32 (MT-32) files variants.
void MidiMusic::playMusic(Musics musicId, bool loop) {
	const char *musicFilenames[] = {
		"ARRAKIS.HSQ",  //  1
		"BAGDAD.HSQ",   //  2
		"CRYOMUS.HSQ",  //  3 (CD only)
		"MORNING.HSQ",  //  4
		"SEKENCE.HSQ",  //  5
		"SIETCHM.HSQ",  //  6
		"WARSONG.HSQ",  //  7
		"WATER.HSQ",    //  8
		"WORMINTR.HSQ", //  9
		"WORMSUIT.HSQ"  //  10
		"ARRAKIS.M32",  //  1
		"BAGDAD.M32",   //  2
		"CRYOMUS.M32",  //  3 (CD only)
		"MORNING.M32",  //  4
		"SEKENCE.M32",  //  5
		"SIETCHM.M32",  //  6
		"WARSONG.M32",  //  7
		"WATER.M32",    //  8
		"WORMINTR.M32", //  9
		"WORMSUIT.M32"  //  10
		"ARRAKIS.AGD",  //  1
		"BAGDAD.AGD",   //  2
		"CRYOMUS.AGD",  //  3 (CD only)
		"MORNING.AGD",  //  4
		"SEKENCE.AGD",  //  5
		"SIETCHM.AGD",  //  6
		"WARSONG.AGD",  //  7
		"WATER.AGD",    //  8
		"WORMINTR.AGD", //  9
		"WORMSUIT.AGD"  //  10
	};

	int music = 0;

	if (!_adlib) {
		music = (int)musicId + 10;
	}

	const char *filename = musicFilenames[musicId];
	setLoop(loop);
	Common::SeekableReadStream *r = _vm->openMember(filename);
	if (_adlib) {
		static_cast<AdLibMidiDriver *>(_driver)->load(r);
	}
	_currentSong = musicId;
	playMusic();
}

void MidiMusic::playMusic() {
	if (_adlib) {
		static_cast<AdLibMidiDriver *>(_driver)->play(_isLooping);
	}
}

void MidiMusic::stopMusic() {
	if (_adlib) {
		static_cast<AdLibMidiDriver *>(_driver)->stopMusic();
	}
}

void MidiMusic::setFrameStop(int frameStop) {
	if (_adlib)
		static_cast<AdLibMidiDriver *>(_driver)->setFrameStop(frameStop);
}

bool MidiMusic::isPlaying() {
	if (_adlib) {
		return static_cast<AdLibMidiDriver *>(_driver)->isPlaying();
	}
	return false;
}


void MidiMusic::setVolume(int volume) {
	volume = CLIP(volume, 0, 255);

	if (_masterVolume == volume)
		return;

	_masterVolume = volume;

	if (_adlib)
		static_cast<AdLibMidiDriver *>(_driver)->setVolume(volume);
}

} // End of namespace Dune
