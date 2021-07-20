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

#include "dune/music.h"
#include "dune/dune.h"
#include "dune/midiadlib.h"

namespace Dune {

MidiMusic::MidiMusic(DuneEngine *vm)
	: _isPlaying(false), _isLooping(false),
	  _masterVolume(192)
	{
	_vm = vm;
	MidiDriver::DeviceHandle dev = MidiDriver::detectDevice(MDT_MIDI | MDT_ADLIB | MDT_PREFER_MT32);
	_isUsingAdlib = (MidiDriver::getMusicType(dev) == MT_ADLIB);
	_isUsingNativeMT32 = ((MidiDriver::getMusicType(dev) == MT_MT32) || ConfMan.getBool("native_mt32"));

	if (_isUsingAdlib) {
		_driver = new AdLibMidiDriver(_vm);
	} else {
		_driver = MidiDriver::createMidi(dev);
		if (_isUsingNativeMT32) {
			_driver->property(MidiDriver::PROP_CHANNEL_MASK, 0x03FE);
		}
	}
	assert(_driver);

	int ret = _driver->open();
	assert(ret == 0);
	_driver->setTimerCallback(this, &timerCallback);

	if (_isUsingNativeMT32)
		_driver->sendMT32Reset();
	else
		_driver->sendGMReset();
}

MidiMusic::~MidiMusic() {
	_driver->setTimerCallback(0, 0);
	delete _driver;
}

void MidiMusic::onTimer() {
	//NOP, fow now...?
}


//TODO: Support AGD (which means usage of the OPL3 by the AdLib midi driver), and M32 (MT-32) files variants.
//TOOD: Fix how it sounds compared to DOSBox
void MidiMusic::playSong(Musics musicId, bool loop) {
	const char *hsqMusicFilenames[] = {
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
	};

	const char *filename = hsqMusicFilenames[musicId];
	setLoop(loop);
	Common::SeekableReadStream *r = _vm->openResource(filename);
	if (_isUsingAdlib) {
		static_cast<AdLibMidiDriver *>(_driver)->load(r);
	}
	_currentSong = musicId;
	playMusic();
}

void MidiMusic::playMusic() {
	stopMusic();
	if (_isUsingAdlib) {
		static_cast<AdLibMidiDriver *>(_driver)-> play(_isLooping);
	}
	_isPlaying = true;
}

void MidiMusic::stopMusic() {
	_isPlaying = false;
	if (_isUsingAdlib) {
		//TODO: implement it.
	}
}


void MidiMusic::setVolume(int volume) {
	volume = CLIP(volume, 0, 255);

	if (_masterVolume == volume)
		return;

	_masterVolume = volume;

	if (_isUsingAdlib)
		static_cast<AdLibMidiDriver *>(_driver)->setVolume(volume);
}

} // End of namespace Dune
