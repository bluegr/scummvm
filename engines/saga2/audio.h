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
 * aint32 with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * Based on the original sources
 *   Faery Tale II -- The Halls of the Dead
 *   (c) 1993-1996 The Wyrmkeep Entertainment Co.
 */

#ifndef SAGA2_AUDIO_H
#define SAGA2_AUDIO_H

#include "audio/mixer.h"

namespace Saga2 {

class Music;
class hResContext;

#define QUEUES_EXTERNAL_ALLOCATION 1

// TODO: FIXME. STUB
typedef int HDIGDRIVER;
typedef int HTIMER;
typedef int HMDIDRIVER;

inline void audioFatal(char *msg) {
	error("Sound error %s", msg);
}

typedef int8 Volume;
typedef Point32 sampleLocation;

#define Here Point32(0,0)

typedef int8 Volume;

enum VolumeTarget {
	kVolSfx,
	kVolVoice,
	kVolMusic
};

enum volumeMode {
	volumeSetTo = 0L,   // absolute mode
	volumeUpDown       // relative mode
};

struct SoundInstance {
	soundSegment seg;
	bool loop;
	sampleLocation loc;
};

class audioInterface {
private:
	soundSegment looping;           // ID of music currently playing
	uint32 _currentSpeech;

	Music *_music;

public:
	Audio::SoundHandle _speechSoundHandle;
	Audio::SoundHandle _sfxSoundHandle;
	Audio::SoundHandle _bgmSoundHandle;
	Audio::SoundHandle _clickSoundHandle;
	Audio::SoundHandle _loopSoundHandle;

	Common::List<SoundInstance> _speechQueue;
	Common::Queue<SoundInstance> _sfxQueue;

	SoundInstance _loopSound;
	SoundInstance _musicSound;

	Audio::Mixer *_mixer;

public:
	// ctor, dtor, initialization
	audioInterface();
	~audioInterface();

	// init, cleanup
	void initAudioInterface(hResContext *musicContext);
	void cleanupAudioInterface(void);

	// timer calls
	void suspendGameClock(void);
	void resumeGameClock(void);

	// event loop calls
	bool playFlag(void);
	void playMe(void);

	// music calls
	void playMusic(soundSegment s, int16 loopFactor = 1, sampleLocation where = Here);
	void stopMusic(void);

	// sound calls
	void queueSound(soundSegment s, int16 loopFactor = 1, sampleLocation where = Here);

	// loop calls
	void queueLoop(soundSegment s, int16 loopFactor = 0, sampleLocation where = Here);
	void stopLoop(void);
	void setLoopPosition(sampleLocation newLoc);
	soundSegment currentLoop(void) {
		return looping;    // ID of music currently playing
	}

	// voice calls
	void queueVoice(soundSegment s, sampleLocation where = Here);
	void queueVoice(soundSegment s[], sampleLocation where = Here);
	void stopVoice(void);
	bool talking(void);
	bool saying(soundSegment s);

	// volume and enabled calls
	bool active(void);

	Volume getVolume(VolumeTarget src);
	void suspend(void);
	void resume(void);
};

} // end of namespace Saga2

#endif
