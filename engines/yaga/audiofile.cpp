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

#include "audio/audiostream.h"
#include "audio/decoders/mp3.h"
#include "audio/decoders/wave.h"
#include "common/compression/unzip.h"
#include "yaga/audiofile.h"

namespace Yaga {

AudioFile::AudioFile(const Common::String &filename, Audio::Mixer::SoundType soundType, Audio::Mixer *mixer) :
	_soundType(soundType), _mixer(mixer) {
	_archive = Common::makeZipArchive(Common::Path(filename));
}

AudioFile::~AudioFile() {
	stop();
	delete _archive;
}

void AudioFile::play(const Common::String &filePath) {
	Common::SeekableReadStream *talkiesStream = _archive->createReadStreamForMember(Common::Path(filePath));
	Audio::SeekableAudioStream *audioStream;

	if (filePath.hasSuffixIgnoreCase(".mp3"))
		audioStream = Audio::makeMP3Stream(talkiesStream, DisposeAfterUse::YES);
	else if (filePath.hasSuffixIgnoreCase(".wav"))
		audioStream = Audio::makeWAVStream(talkiesStream, DisposeAfterUse::YES);
	else
		error("Unsupported audio format: %s", filePath.c_str());

	_mixer->playStream(Audio::Mixer::kSFXSoundType, &_audioHandle, audioStream);
}

void AudioFile::stop() {
	_mixer->stopHandle(_audioHandle);
}

bool AudioFile::isPlaying() {
	return _mixer->isSoundHandleActive(_audioHandle);
}

} // End of namespace Yaga
