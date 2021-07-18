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

#ifndef DUNE_MUSIC_H
#define DUNE_MUSIC_H

#include "audio/mididrv.h"
#include "dune/dune.h"

namespace Dune {
class DuneEngine;

class MidiMusic : public MidiDriver_BASE {
public:
	MidiMusic(DuneEngine *vm);
	~MidiMusic() override;
	void setVolume(int volume);
	int getVolume()	const { return _masterVolume; }

	void playSong(Musics musicId, bool loop);
	void stopMusic();
	void setLoop(bool loop)		{ _isLooping = loop; }

	// MidiDriver_BASE interface implementation
	//NOP
	void send(uint32 b) override{}
	//NOP
	void metaEvent(byte type, byte *data, uint16 length) override {}

protected:
	void playMusic();

	MidiDriver *_driver;

	void onTimer();

	static void timerCallback(void *refCon) { ((MidiMusic *)refCon)->onTimer(); }

	bool _isUsingAdlib;
	bool _isUsingNativeMT32;

	bool _isPlaying;
	bool _isLooping;
	byte _masterVolume;

private:
	Musics _currentSong;
	DuneEngine *_vm;
};

} // End of namespace Dune

#endif
