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
/*
 * Adplug - Replayer for many OPL2/OPL3 audio file formats.
 * Copyright (C) 1999 - 2008 Simon Peter <dn.tlp@gmx.net>, et al.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * herad.cpp - Herbulot AdLib Player by Stas'M <binarymaster@mail.ru>
 *
 * Thanks goes to co-workers:
 *   -=CHE@TER=- (SQX decompression)
 *   SynaMax (general documentation, reverse-engineering, testing)
 *   Jepael (timer code sample, DOS driver shell)
 *   Daniël van de Burgt "thatdutchguy" (pitch slides code sample)
 *
 * REFERENCES:
 * http://www.vgmpf.com/Wiki/index.php/HERAD
 *
 * TODO:
 * - Fix strange AGD sound
 * - Fix splash sound in Gorbi (at 0:23)
 * - Fix hiss sound in NewSan (at beginning)
 */

#include <cstring>

#include "dune/midiadlib.h"
#include "dune/dune.h"

#include "common/debug.h"
#include "common/endian.h"

namespace Dune {
AdLibMidiDriver::AdLibMidiDriver(DuneEngine *vm) : _vm(vm) {
	_buf_size = 2048;
	_bits = 16;
	_nChannels = 2;
	_frequency = 44100;
	_reader = nullptr;
	_opl = nullptr;
	open();
	adlibSetupCard();
	for (int i = 0; i < 11; ++i) {
		_adlibChannelsVolume[i] = 0;
		adlibSetNoteVolume(i, 0);
		adlibTurnNoteOff(i);
	}
}

AdLibMidiDriver::~AdLibMidiDriver() {
	if (_opl) {
		close();
	}
	if (_tracks) {
		for (int i = 0; i < _nTracks; i++) {
			if (_tracks[i].data)
				delete[] _tracks[i].data;
		}
		delete[] _tracks;
	}
	if (_channels)
		delete[] _channels;
	if (_instruments)
		delete[] _instruments;
}

const uint8_t AdLibMidiDriver::slot_offset[HERAD_NUM_VOICES] = {
	0, 1, 2, 8, 9, 10, 16, 17, 18};
const uint16_t AdLibMidiDriver::FNum[HERAD_NUM_NOTES] = {
	343, 364, 385, 408, 433, 459, 486, 515, 546, 579, 614, 650};
const uint8_t AdLibMidiDriver::fine_bend[HERAD_NUM_NOTES + 1] = {
	19, 21, 21, 23, 25, 26, 27, 29, 31, 33, 35, 36, 37};
const uint8_t AdLibMidiDriver::coarse_bend[10] = {
	0, 5, 10, 15, 20,
	0, 6, 12, 18, 24};

bool AdLibMidiDriver::update() {
	_wTime = _wTime - 256;
	if (_wTime < 0) {
		_wTime = _wTime + _wSpeed;
		processEvents();
	}
	return !_songEnd;
}

void AdLibMidiDriver::rewind(int subsong) {
	uint32_t j;
	_wTime = 0;
	_songEnd = false;

	_current_tick_position = -1; // there's always 1 excess tick at start
	_total_tick_count = 0;
	_loop_pos = -1;
	_loop_times = 1;

	for (int i = 0; i < _nTracks; i++) {
		_tracks[i].pos = 0;
		j = 0;
		while (_tracks[i].pos < _tracks[i].size) {
			j += getTicks(i);
			switch (_tracks[i].data[_tracks[i].pos++] & 0xF0) {
			case 0x80: // Note Off
				_tracks[i].pos += (_isHeradV2 ? 1 : 2);
				break;
			case 0x90: // Note On
			case 0xA0: // Unused
			case 0xB0: // Unused
				_tracks[i].pos += 2;
				break;
			case 0xC0: // Program Change
			case 0xD0: // Aftertouch
			case 0xE0: // Pitch Bend
				_tracks[i].pos++;
				break;
			default:
				_tracks[i].pos = _tracks[i].size;
				break;
			}
		}
		if (j > _total_tick_count)
			_total_tick_count = j;
		_tracks[i].pos = 0;
		_tracks[i].counter = 0;
		_tracks[i].ticks = 0;
		_channels[i].program = 0;
		_channels[i].playprog = 0;
		_channels[i].note = 0;
		_channels[i].keyon = false;
		_channels[i].bend = HERAD_BEND_CENTER;
		_channels[i].slide_dur = 0;
	}
	if (_isHeradV2) {
		if (!_wLoopStart || _wLoopCount)
			_wLoopStart = 1; // if loop not specified, start from beginning
		if (!_wLoopEnd || _wLoopCount)
			_wLoopEnd = getpatterns() + 1; // till the end
		if (_wLoopCount)
			_wLoopCount = 0; // repeats forever
	}

	_opl->init();
	_opl->write(1, 32);   // Enable Waveform Select
	_opl->write(0xBD, 0); // Disable Percussion Mode
	_opl->write(8, 64);   // Enable Note-Sel
	if (isAgd) {
		enableOPL3();
	}
}

void AdLibMidiDriver::enableOPL3() {
	//_opl->setchip(1);
	//_opl->setchip(0);
	if (_oplType == OPL::Config::OplType::kOpl3 && _opl && _isOplInitialized) {
		return;
	}
	if (_opl) {
		close();
	}
	_oplType = OPL::Config::OplType::kOpl3;
	open();
	_opl->write(5, 1); // Enable OPL3
	_opl->write(4, 0); // Disable 4OP Mode
}

void AdLibMidiDriver::enableDualOPL2() {
	if (_oplType == OPL::Config::OplType::kDualOpl2 && _opl && _isOplInitialized) {
		return;
	}
	if (_opl) {
		close();
	}
	_oplType = OPL::Config::OplType::kDualOpl2;
	open();
}

void AdLibMidiDriver::enableSingleOPL2() {
	if (_oplType == OPL::Config::OplType::kOpl2 && _opl && _isOplInitialized) {
		return;
	}
	if (_opl) {
		close();
	}
	_oplType = OPL::Config::OplType::kOpl2;
	open();
}

Common::String AdLibMidiDriver::gettype() {
	char scomp[12 + 1] = "";
	if (_fileCompressionType > HERAD_COMP_NONE) {
		debug(scomp, ", %s packed", (_fileCompressionType == HERAD_COMP_HSQ ? "HSQ" : "SQX"));
	}
	char type[40 + 1];
	debug(type, "HERAD System %s (version %d%s)", (isAgd ? "AGD" : "SDB"), (_isHeradV2 ? 2 : 1), scomp);
	return Common::String(type);
}

void AdLibMidiDriver::load(Common::SeekableReadStream *reader) {
	open();
	_reader = reader;
	long size = _reader->size();
	// Read entire file into memory
	uint8_t *data = new uint8_t[size];
	_reader->read(data, size);
	// Detect compression
	if (isSQX(data)) {
		_fileCompressionType = HERAD_COMP_SQX;
		uint8_t *out = new uint8_t[HERAD_MAX_SIZE];
		memset(out, 0, HERAD_MAX_SIZE);
		size = SQX_decompress(data, size, out);
		delete[] data;
		data = new uint8_t[size];
		memcpy(data, out, size);
		delete[] out;
	} else {
		_fileCompressionType = HERAD_COMP_NONE;
	}
	// Process file header
	uint16_t offset;
	if (size < HERAD_HEAD_SIZE) {
		delete[] data;
	}
	if (size < *(uint16_t *)data) {
		delete[] data;
	}
	_nInstruments = (size - *(uint16_t *)data) / HERAD_INST_SIZE;
	if (_nInstruments == 0) {
		delete[] data;
	}
	offset = *(uint16_t *)(data + 2);
	if (offset != 0x32 && offset != 0x52) {
		delete[] data;
	}
	isAgd = offset == 0x52;
	_wLoopStart = *(uint16_t *)(data + 0x2C);
	_wLoopEnd = *(uint16_t *)(data + 0x2E);
	_wLoopCount = *(uint16_t *)(data + 0x30);
	_wSpeed = *(uint16_t *)(data + 0x32);
	if (_wSpeed == 0) {
		delete[] data;
	}
	_nTracks = 0;
	for (int i = 0; i < HERAD_MAX_TRACKS; i++) {
		if (*(uint16_t *)(data + 2 + i * 2) == 0)
			break;
		_nTracks++;
	}
	_tracks = new herad_trk[_nTracks];
	_channels = new herad_chn[_nTracks];
	for (int i = 0; i < _nTracks; i++) {
		offset = *(uint16_t *)(data + 2 + i * 2) + 2;
		uint16_t next = (i < HERAD_MAX_TRACKS - 1 ? *(uint16_t *)(data + 2 + (i + 1) * 2) + 2 : *(uint16_t *)data);
		if (next <= 2)
			next = *(uint16_t *)data;

		_tracks[i].size = next - offset;
		_tracks[i].data = new uint8_t[_tracks[i].size];
		memcpy(_tracks[i].data, data + offset, _tracks[i].size);
	}
	_instruments = new herad_inst[_nInstruments];
	offset = *(uint16_t *)data;
	_isHeradV2 = true;
	for (int i = 0; i < _nInstruments; i++) {
		memcpy(_instruments[i].data, data + offset + i * HERAD_INST_SIZE, HERAD_INST_SIZE);
		if (_isHeradV2 && _instruments[i].param.mode == HERAD_INSTMODE_SDB1)
			_isHeradV2 = false;
	}
	delete[] data;
	rewind(0);
}

bool AdLibMidiDriver::isSQX(uint8_t *data) {
	// data[0] - word OutbufInit
	// data[1]
	// data[2] - byte SQX flag #1
	// data[3] - byte SQX flag #2
	// data[4] - byte SQX flag #3
	// data[5] - byte CntOffPart
	if (data[2] > 2 || data[3] > 2 || data[4] > 2) {
		return false;
	}
	if (data[5] == 0 || data[5] > 15) {
		return false;
	}
	return true;
}

uint16_t AdLibMidiDriver::SQX_decompress(uint8_t *data, int size, uint8_t *out) {
	int16_t offset;
	uint16_t count;
	uint8_t *src = data;
	uint8_t *dst = out;
	bool done = false;

	std::memcpy(dst, src, sizeof(uint16_t));
	src += 6;
	uint16_t queue = 1;
	uint8_t bit, bit_p;
	while (true) {
		bit = queue & 1;
		queue >>= 1;
		if (queue == 0) {
			queue = u16_unaligned(src);
			src += 2;
			bit_p = bit;
			bit = queue & 1;
			queue >>= 1;
			if (bit_p)
				queue |= 0x8000;
		}
		if (bit == 0) {
			switch (data[2]) {
			case 0:
				*dst++ = *src++;
				break;
			case 1:
				count = 0;
				bit = queue & 1;
				queue >>= 1;
				if (queue == 0) {
					queue = u16_unaligned(src);
					src += 2;
					bit_p = bit;
					bit = queue & 1;
					queue >>= 1;
					if (bit_p)
						queue |= 0x8000;
					count = bit;
					bit = queue & 1;
					queue >>= 1;
				} else {
					count = bit;
					bit = queue & 1;
					queue >>= 1;
					if (queue == 0) {
						queue = u16_unaligned(src);
						src += 2;
						bit_p = bit;
						bit = queue & 1;
						queue >>= 1;
						if (bit_p)
							queue |= 0x8000;
					}
				}
				count = (count << 1) | bit;
				offset = *(uint8_t *)src;
				offset -= 256;
				src++;
				count += 2;
				while (count--) {
					*dst = *(dst + offset);
					dst++;
				}
				break;
			case 2:
				count = u16_unaligned(src);
				offset = (count >> data[5]) - (1 << (16 - data[5]));
				count &= (1 << data[5]) - 1;
				src += 2;
				if (!count) {
					count = *(uint8_t *)src;
					src++;
				}
				if (!count) {
					done = true;
					break;
				}
				count += 2;
				while (count--) {
					*dst = *(dst + offset);
					dst++;
				}
				break;
			}
			if (done)
				break;
			continue;
		} else {
			bit = queue & 1;
			queue >>= 1;
			if (queue == 0) {
				queue = u16_unaligned(src);
				src += 2;
				bit_p = bit;
				bit = queue & 1;
				queue >>= 1;
				if (bit_p)
					queue |= 0x8000;
			}
			if (bit == 0) {
				switch (data[3]) {
				case 0:
					*dst++ = *src++;
					break;
				case 1:
					count = 0;
					bit = queue & 1;
					queue >>= 1;
					if (queue == 0) {
						queue = u16_unaligned(src);
						src += 2;
						bit_p = bit;
						bit = queue & 1;
						queue >>= 1;
						if (bit_p)
							queue |= 0x8000;
						count = bit;
						bit = queue & 1;
						queue >>= 1;
					} else {
						count = bit;
						bit = queue & 1;
						queue >>= 1;
						if (queue == 0) {
							queue = u16_unaligned(src);
							src += 2;
							bit_p = bit;
							bit = queue & 1;
							queue >>= 1;
							if (bit_p)
								queue |= 0x8000;
						}
					}
					count = (count << 1) | bit;
					offset = *(uint8_t *)src;
					offset -= 256;
					src++;
					count += 2;
					while (count--) {
						*dst = *(dst + offset);
						dst++;
					}
					break;
				case 2:
					count = u16_unaligned(src);
					offset = (count >> data[5]) - (1 << (16 - data[5]));
					count &= (1 << data[5]) - 1;
					src += 2;
					if (!count) {
						count = *(uint8_t *)src;
						src++;
					}
					if (!count) {
						done = true;
						break;
					}
					count += 2;
					while (count--) {
						*dst = *(dst + offset);
						dst++;
					}
					break;
				}
				if (done)
					break;
				continue;
			} else {
				switch (data[4]) {
				case 0:
					*dst++ = *src++;
					break;
				case 1:
					count = 0;
					bit = queue & 1;
					queue >>= 1;
					if (queue == 0) {
						queue = u16_unaligned(src);
						src += 2;
						bit_p = bit;
						bit = queue & 1;
						queue >>= 1;
						if (bit_p)
							queue |= 0x8000;
						count = bit;
						bit = queue & 1;
						queue >>= 1;
					} else {
						count = bit;
						bit = queue & 1;
						queue >>= 1;
						if (queue == 0) {
							queue = u16_unaligned(src);
							src += 2;
							bit_p = bit;
							bit = queue & 1;
							queue >>= 1;
							if (bit_p)
								queue |= 0x8000;
						}
					}
					count = (count << 1) | bit;
					offset = *(uint8_t *)src;
					offset -= 256;
					src++;
					count += 2;
					while (count--) {
						*dst = *(dst + offset);
						dst++;
					}
					break;
				case 2:
					count = u16_unaligned(src);
					offset = (count >> data[5]) - (1 << (16 - data[5]));
					count &= (1 << data[5]) - 1;
					src += 2;
					if (!count) {
						count = *(uint8_t *)src;
						src++;
					}
					if (!count) {
						done = true;
						break;
					}
					count += 2;
					while (count--) {
						*dst = *(dst + offset);
						dst++;
					}
					break;
				}
				if (done)
					break;
				continue;
			}
		}
	}
	return dst - out;
}

void AdLibMidiDriver::frame() {
	_isPlaying = update();
}

uint32_t AdLibMidiDriver::getTicks(uint8_t t) {
	uint32_t result = 0;
	do {
		result <<= 7;
		result |= _tracks[t].data[_tracks[t].pos] & 0x7F;
	} while (_tracks[t].data[_tracks[t].pos++] & 0x80 && _tracks[t].pos < _tracks[t].size);
	return result;
}
void AdLibMidiDriver::executeCommand(uint8_t t) {
	uint8_t status, note, par;

	if (t >= _nTracks)
		return;

	if (t >= (isAgd ? HERAD_NUM_VOICES * 2 : HERAD_NUM_VOICES)) {
		_tracks[t].pos = _tracks[t].size;
		return;
	}

	// execute MIDI command
	status = _tracks[t].data[_tracks[t].pos++];
	if (status == 0xFF) {
		_tracks[t].pos = _tracks[t].size;
	} else {
		switch (status & 0xF0) {
		case 0x80: // Note Off
			note = _tracks[t].data[_tracks[t].pos++];
			par = (_isHeradV2 ? 0 : _tracks[t].data[_tracks[t].pos++]);
			ev_noteOff(t, note, par);
			break;
		case 0x90: // Note On
			note = _tracks[t].data[_tracks[t].pos++];
			par = _tracks[t].data[_tracks[t].pos++];
			ev_noteOn(t, note, par);
			break;
		case 0xA0: // Unused
		case 0xB0: // Unused
			_tracks[t].pos += 2;
			break;
		case 0xC0: // Program Change
			par = _tracks[t].data[_tracks[t].pos++];
			ev_programChange(t, par);
			break;
		case 0xD0: // Aftertouch
			par = _tracks[t].data[_tracks[t].pos++];
			ev_aftertouch(t, par);
			break;
		case 0xE0: // Pitch Bend
			par = _tracks[t].data[_tracks[t].pos++];
			ev_pitchBend(t, par);
			break;
		default:
			_tracks[t].pos = _tracks[t].size;
			break;
		}
	}
}
void AdLibMidiDriver::processEvents() {
	uint8_t i;
	_songEnd = true;

	if (_wLoopStart && _wLoopEnd && (_current_tick_position + 1) % HERAD_MEASURE_TICKS == 0 && (_current_tick_position + 1) / HERAD_MEASURE_TICKS + 1 == _wLoopStart) {
		_loop_pos = _current_tick_position;
		for (i = 0; i < _nTracks; i++) {
			_loop_data[i].counter = _tracks[i].counter;
			_loop_data[i].ticks = _tracks[i].ticks;
			_loop_data[i].pos = _tracks[i].pos;
		}
	}
	for (i = 0; i < _nTracks; i++) {
		if (_channels[i].slide_dur > 0 && _channels[i].keyon)
			macroSlide(i);
		if (_tracks[i].pos >= _tracks[i].size)
			continue;
		_songEnd = false; // track is not finished
		if (!_tracks[i].counter) {
			bool first = _tracks[i].pos == 0;
			_tracks[i].ticks = getTicks(i);
			if (first && _tracks[i].ticks)
				_tracks[i].ticks++; // workaround to synchronize tracks (there's always 1 excess tick at start)
		}
		if (++_tracks[i].counter >= _tracks[i].ticks) {
			_tracks[i].counter = 0;
			while (_tracks[i].pos < _tracks[i].size) {
				executeCommand(i);
				if (_tracks[i].pos >= _tracks[i].size) {
					break;
				} else if (!_tracks[i].data[_tracks[i].pos]) // if next delay is zero
				{
					_tracks[i].pos++;
				} else
					break;
			}
		} else if (_tracks[i].ticks >= 0x8000) {
			_tracks[i].pos = _tracks[i].size;
			_tracks[i].counter = _tracks[i].ticks;
		}
	}
	if (!_songEnd)
		_current_tick_position++;
	if (_wLoopStart && _wLoopEnd && (_current_tick_position == _total_tick_count || (_current_tick_position % HERAD_MEASURE_TICKS == 0 && _current_tick_position / HERAD_MEASURE_TICKS + 1 == _wLoopEnd))) {
		if (_isLooping) {
			if (!_wLoopCount)
				_songEnd = true;
			else if (_songEnd && _loop_times < _wLoopCount)
				_songEnd = false;

			if (!_wLoopCount || _loop_times < _wLoopCount) {
				_current_tick_position = _loop_pos;
				for (i = 0; i < _nTracks; i++) {
					_tracks[i].counter = _loop_data[i].counter;
					_tracks[i].ticks = _loop_data[i].ticks;
					_tracks[i].pos = _loop_data[i].pos;
				}
				if (_wLoopCount)
					_loop_times++;
			}
		}
	}
}
void AdLibMidiDriver::ev_noteOn(uint8_t ch, uint8_t note, uint8_t vel) {
	int8_t macro;

	if (_channels[ch].keyon) {
		// turn off last active note
		_channels[ch].keyon = false;
		playNote(ch, _channels[ch].note, HERAD_NOTE_OFF);
	}
	if (_isHeradV2 && _instruments[_channels[ch].program].param.mode == HERAD_INSTMODE_KMAP) {
		// keymap is used
		int8_t mp = note - (_instruments[_channels[ch].program].keymap.offset + 24);
		if (mp < 0 || mp >= HERAD_INST_SIZE - 4)
			return; // if not in range, skip note
		_channels[ch].playprog = _instruments[_channels[ch].program].keymap.index[mp];
		changeProgram(ch, _channels[ch].playprog);
	}
	_channels[ch].note = note;
	_channels[ch].keyon = true;
	_channels[ch].bend = HERAD_BEND_CENTER;
	if (_isHeradV2 && _instruments[_channels[ch].playprog].param.mode == HERAD_INSTMODE_KMAP)
		return; // single keymapped instrument can't be keymap (avoid recursion)
	playNote(ch, note, HERAD_NOTE_ON);
	macro = _instruments[_channels[ch].playprog].param.mc_mod_out_vel;
	if (macro != 0)
		macroModOutput(ch, _channels[ch].playprog, macro, vel);
	macro = _instruments[_channels[ch].playprog].param.mc_car_out_vel;
	if (macro != 0)
		macroCarOutput(ch, _channels[ch].playprog, macro, vel);
	macro = _instruments[_channels[ch].playprog].param.mc_fb_vel;
	if (macro != 0)
		macroFeedback(ch, _channels[ch].playprog, macro, vel);
}
void AdLibMidiDriver::ev_noteOff(uint8_t ch, uint8_t note, uint8_t vel) {
	if (note != _channels[ch].note || !_channels[ch].keyon)
		return;
	_channels[ch].keyon = false;
	playNote(ch, note, HERAD_NOTE_OFF);
}
void AdLibMidiDriver::ev_programChange(uint8_t ch, uint8_t prog) {
	if (prog >= _nInstruments) // out of index
		return;
	_channels[ch].program = prog;
	_channels[ch].playprog = prog;
	changeProgram(ch, prog);
}
void AdLibMidiDriver::ev_aftertouch(uint8_t ch, uint8_t vel) {
	int8_t macro;

	if (_isHeradV2) // version 2 ignores this event
		return;
	macro = _instruments[_channels[ch].playprog].param.mc_mod_out_at;
	if (macro != 0)
		macroModOutput(ch, _channels[ch].playprog, macro, vel);
	macro = _instruments[_channels[ch].playprog].param.mc_car_out_at;
	if (macro != 0 && _instruments[_channels[ch].playprog].param.mc_car_out_vel != 0)
		macroCarOutput(ch, _channels[ch].playprog, macro, vel);
	macro = _instruments[_channels[ch].playprog].param.mc_fb_at;
	if (macro != 0)
		macroFeedback(ch, _channels[ch].playprog, macro, vel);
}
void AdLibMidiDriver::ev_pitchBend(uint8_t ch, uint8_t bend) {
	_channels[ch].bend = bend;
	if (_channels[ch].keyon) // update pitch
		playNote(ch, _channels[ch].note, HERAD_NOTE_UPDATE);
}
void AdLibMidiDriver::playNote(uint8_t c, uint8_t note, uint8_t state) {
	if (_instruments[_channels[c].playprog].param.mc_transpose != 0)
		macroTranspose(&note, _channels[c].playprog);
	note = (note - 24) & 0xFF;
	if (state != HERAD_NOTE_UPDATE && note >= 0x60)
		note = 0; // clip too low/high notes
	int8_t oct = note / HERAD_NUM_NOTES;
	int8_t key = note % HERAD_NUM_NOTES;
	if (state != HERAD_NOTE_UPDATE && _instruments[_channels[c].playprog].param.mc_slide_dur) {
		_channels[c].slide_dur = (state == HERAD_NOTE_ON ? _instruments[_channels[c].playprog].param.mc_slide_dur : 0);
	}
	uint8_t bend = _channels[c].bend;
	int16_t amount, detune = 0;
	uint8_t amount_lo, amount_hi;
	if (!(_instruments[_channels[c].playprog].param.mc_slide_coarse & 1)) { // fine tune
		if (bend - HERAD_BEND_CENTER < 0) {                   // slide down
			amount = HERAD_BEND_CENTER - bend;
			amount_lo = (amount >> 5);
			amount_hi = (amount << 3) & 0xFF;
			key -= amount_lo;

			if (key < 0) {
				key += HERAD_NUM_NOTES;
				oct--;
			}
			if (oct < 0) {
				key = 0;
				oct = 0;
			}
			detune = -1 * ((fine_bend[key] * amount_hi) >> 8);
		} else { // slide up
			amount = bend - HERAD_BEND_CENTER;
			amount_lo = (amount >> 5);
			amount_hi = (amount << 3) & 0xFF;
			key += amount_lo;

			if (key >= HERAD_NUM_NOTES) {
				key -= HERAD_NUM_NOTES;
				oct++;
			}
			detune = (fine_bend[key + 1] * amount_hi) >> 8;
		}
	} else { // coarse tune
		uint8_t offset;
		if (bend - HERAD_BEND_CENTER < 0) { // slide down
			amount = HERAD_BEND_CENTER - bend;
			key -= amount / 5;

			if (key < 0) {
				key += HERAD_NUM_NOTES;
				oct--;
			}
			if (oct < 0) {
				key = 0;
				oct = 0;
			}
			offset = (amount % 5) + (key >= 6 ? 5 : 0);
			detune = -1 * coarse_bend[offset];
		} else { // slide up
			amount = bend - HERAD_BEND_CENTER;
			key += amount / 5;

			if (key >= HERAD_NUM_NOTES) {
				key -= HERAD_NUM_NOTES;
				oct++;
			}
			offset = (amount % 5) + (key >= 6 ? 5 : 0);
			detune = coarse_bend[offset];
		}
	}
	setFreq(c, oct, FNum[key] + detune, state != HERAD_NOTE_OFF);
}
void AdLibMidiDriver::setFreq(uint8_t c, uint8_t oct, uint16_t freq, bool on) {
	uint8_t reg, val;

	if (c >= HERAD_NUM_VOICES)
		enableDualOPL2();

	reg = 0xA0 + (c % HERAD_NUM_VOICES);
	val = freq & 0xFF;
	adlibWrite(reg, val);
	reg = 0xB0 + (c % HERAD_NUM_VOICES);
	val = ((freq >> 8) & 3) |
		  ((oct & 7) << 2) |
		  ((on ? 1 : 0) << 5);
	adlibWrite(reg, val);

	/*if (c >= HERAD_NUM_VOICES)
		enableSingleOPL2();*/
}
void AdLibMidiDriver::changeProgram(uint8_t c, uint8_t i) {
	uint8_t reg, val;

	if (_isHeradV2 && _instruments[i].param.mode == HERAD_INSTMODE_KMAP)
		return;

	/*if (c >= HERAD_NUM_VOICES)
		enableDualOPL2();*/

	// Amp Mod / Vibrato / EG type / Key Scaling / Multiple
	reg = 0x20 + slot_offset[c % HERAD_NUM_VOICES];
	val = (_instruments[i].param.mod_mul & 15) |
		  ((_instruments[i].param.mod_ksr & 1) << 4) |
		  ((_instruments[i].param.mod_eg > 0 ? 1 : 0) << 5) |
		  ((_instruments[i].param.mod_vib & 1) << 6) |
		  ((_instruments[i].param.mod_am & 1) << 7);
	adlibWrite(reg, val);
	reg += 3;
	val = (_instruments[i].param.car_mul & 15) |
		  ((_instruments[i].param.car_ksr & 1) << 4) |
		  ((_instruments[i].param.car_eg > 0 ? 1 : 0) << 5) |
		  ((_instruments[i].param.car_vib & 1) << 6) |
		  ((_instruments[i].param.car_am & 1) << 7);
	adlibWrite(reg, val);

	// Key scaling level / Output level
	reg = 0x40 + slot_offset[c % HERAD_NUM_VOICES];
	val = (_instruments[i].param.mod_out & 63) |
		  ((_instruments[i].param.mod_ksl & 3) << 6);
	adlibWrite(reg, val);
	reg += 3;
	val = (_instruments[i].param.car_out & 63) |
		  ((_instruments[i].param.car_ksl & 3) << 6);
	adlibWrite(reg, val);

	// Attack Rate / Decay Rate
	reg = 0x60 + slot_offset[c % HERAD_NUM_VOICES];
	val = (_instruments[i].param.mod_D & 15) |
		  ((_instruments[i].param.mod_A & 15) << 4);
	adlibWrite(reg, val);
	reg += 3;
	val = (_instruments[i].param.car_D & 15) |
		  ((_instruments[i].param.car_A & 15) << 4);
	adlibWrite(reg, val);

	// Sustain Level / Release Rate
	reg = 0x80 + slot_offset[c % HERAD_NUM_VOICES];
	val = (_instruments[i].param.mod_R & 15) |
		  ((_instruments[i].param.mod_S & 15) << 4);
	adlibWrite(reg, val);
	reg += 3;
	val = (_instruments[i].param.car_R & 15) |
		  ((_instruments[i].param.car_S & 15) << 4);
	adlibWrite(reg, val);

	// Panning / Feedback strength / Connection type
	reg = 0xC0 + (c % HERAD_NUM_VOICES);
	val = (_instruments[i].param.con > 0 ? 0 : 1) |
		  ((_instruments[i].param.feedback & 7) << 1) |
		  ((isAgd ? (_instruments[i].param.pan == 0 || _instruments[i].param.pan > 3 ? 3 : _instruments[i].param.pan) : 0) << 4);
	adlibWrite(reg, val);

	// Wave Select
	reg = 0xE0 + slot_offset[c % HERAD_NUM_VOICES];
	val = _instruments[i].param.mod_wave & (isAgd ? 7 : 3);
	adlibWrite(reg, val);
	reg += 3;
	val = _instruments[i].param.car_wave & (isAgd ? 7 : 3);
	adlibWrite(reg, val);

	/*if (c >= HERAD_NUM_VOICES)
		enableSingleOPL2();*/
}
void AdLibMidiDriver::send(uint32 b) {
	int channel = b & 15;
	int cmd = (b >> 4) & 7;
	int param1 = (b >> 8) & 255;
	int param2 = (b >> 16) & 255;
	switch (cmd) {
	case 0:
		adlibTurnNoteOff(channel);
		break;
	case 1:
		handleMidiEvent0x90_NoteOn(channel, param1, param2);
		break;
	case 3:
		break;
	case 5:
		adlibSetNoteVolume(channel, param1);
		_adlibChannelsVolume[channel] = param1;
		break;
	case 6:
		adlibSetPitchBend(channel, param1 | (param2 << 7));
		break;
	default:
		debug("Unhandled cmd %d channel %d (0x%X)", cmd, channel, b);
		break;
	}
}

void AdLibMidiDriver::macroModOutput(uint8_t c, uint8_t i, int8_t sens, uint8_t level) {
	uint8_t reg, val;
	uint16_t output;

	if (sens < -4 || sens > 4)
		return;

	if (sens < 0) {
		output = (level >> (sens + 4) > 63 ? 63 : level >> (sens + 4));
	} else {
		output = ((0x80 - level) >> (4 - sens) > 63 ? 63 : (0x80 - level) >> (4 - sens));
	}
	output += _instruments[i].param.mod_out;
	if (output > 63)
		output = 63;

	/*if (c >= HERAD_NUM_VOICES)
		enableDualOPL2();*/

	// Key scaling level / Output level
	reg = 0x40 + slot_offset[c % HERAD_NUM_VOICES];
	val = (output & 63) | ((_instruments[i].param.mod_ksl & 3) << 6);
	adlibWrite(reg, val);

	/*if (c >= HERAD_NUM_VOICES)
		enableSingleOPL2();*/
}
void AdLibMidiDriver::macroCarOutput(uint8_t c, uint8_t i, int8_t sens, uint8_t level) {
	uint8_t reg, val;
	uint16_t output;

	if (sens < -4 || sens > 4)
		return;

	if (sens < 0) {
		output = (level >> (sens + 4) > 63 ? 63 : level >> (sens + 4));
	} else {
		output = ((0x80 - level) >> (4 - sens) > 63 ? 63 : (0x80 - level) >> (4 - sens));
	}
	output += _instruments[i].param.car_out;
	if (output > 63)
		output = 63;

	/*if (c >= HERAD_NUM_VOICES)
		enableDualOPL2();*/

	// Key scaling level / Output level
	reg = 0x43 + slot_offset[c % HERAD_NUM_VOICES];
	val = (output & 63) | ((_instruments[i].param.car_ksl & 3) << 6);
	adlibWrite(reg, val);

	/*if (c >= HERAD_NUM_VOICES)
		enableSingleOPL2();*/
}
void AdLibMidiDriver::macroFeedback(uint8_t c, uint8_t i, int8_t sens, uint8_t level) {
	uint8_t reg, val;
	uint8_t feedback;

	if (sens < -6 || sens > 6)
		return;

	if (sens < 0) {
		feedback = (level >> (sens + 7) > 7 ? 7 : level >> (sens + 7));
	} else {
		feedback = ((0x80 - level) >> (7 - sens) > 7 ? 7 : (0x80 - level) >> (7 - sens));
	}
	feedback += _instruments[i].param.feedback;
	if (feedback > 7)
		feedback = 7;

	/*if (c >= HERAD_NUM_VOICES)
		enableDualOPL2();*/

	// Panning / Feedback strength / Connection type
	reg = 0xC0 + (c % HERAD_NUM_VOICES);
	val = (_instruments[i].param.con > 0 ? 0 : 1) | ((feedback & 7) << 1) |
		((isAgd ? (_instruments[i].param.pan == 0 || _instruments[i].param.pan > 3 ? 3 : _instruments[i].param.pan) : 0) << 4);
	adlibWrite(reg, val);

	/*if (c >= HERAD_NUM_VOICES)
		enableSingleOPL2();*/
}
void AdLibMidiDriver::macroTranspose(uint8_t *note, uint8_t i) {
	uint8_t tran = _instruments[i].param.mc_transpose;
	uint8_t diff = (tran - 0x31) & 0xFF;
	if (_isHeradV2 && diff < 0x60)
		*note = (diff + 0x18) & 0xFF;
	else
		*note = (*note + tran) & 0xFF;
}
void AdLibMidiDriver::macroSlide(uint8_t c) {
	if (!_channels[c].slide_dur)
		return;

	_channels[c].slide_dur--;
	_channels[c].bend += _instruments[_channels[c].playprog].param.mc_slide_range;

	if (!(_channels[c].note & 0x7F))
		return;
	playNote(c, _channels[c].note, HERAD_NOTE_UPDATE);
}

void AdLibMidiDriver::close() {
	delete _opl;
	_isOplInitialized = false;
}

int AdLibMidiDriver::open() {
	if (_isOplInitialized) {
		return 0;
	}
	_isOplInitialized = true;
	_opl = OPL::Config::create(_oplType);
	if (!_opl || !_opl->init())
		error("Failed to create OPL");
	return 0;
}

void AdLibMidiDriver::play(bool loop) {
	_isLooping = loop;
	_opl->start(new Common::Functor0Mem<void, AdLibMidiDriver>(this, &AdLibMidiDriver::onTimer), 235);
}
void AdLibMidiDriver::setTimerCallback(void *timerParam, Common::TimerManager::TimerProc timerProc) {
	_adlibTimerProc = timerProc;
	_adlibTimerParam = timerParam;
}

void AdLibMidiDriver::onTimer() {
	if (_adlibTimerProc)
		(*_adlibTimerProc)(_adlibTimerParam);
	frame();
}

void AdLibMidiDriver::setVolume(uint32 volume) {
	for (int i = 0; i < _midiNumberOfChannels; ++i)
		adlibSetChannelVolume(i, volume * 64 / 256 + 64);
}

void AdLibMidiDriver::metaEvent(byte type, byte *data, uint16 length) {
	int event = 0;
	if (length > 4 && READ_BE_UINT32(data) == 0x3F00) {
		event = data[4];
		switch (event) {
		case 1:
			if (length == 34) {
				handleSequencerSpecificMetaEvent1(data[5], data + 6);
				return;
			}
			break;
		case 2:
			if (length == 6) {
				handleSequencerSpecificMetaEvent2(data[5]);
				return;
			}
			break;
		case 3:
			if (length == 6) {
				handleSequencerSpecificMetaEvent3(data[5]);
				return;
			}
			break;
		default:
			break;
		}
	}
	warning("Unhandled meta event %d len %d", event, length);
}

void AdLibMidiDriver::handleSequencerSpecificMetaEvent1(int channel, const uint8 *data) {
	for (int i = 0; i < 28; ++i) {
		_adlibMetaSequenceData[i] = data[i];
	}
	if (_midiNumberOfChannels > channel) {
		const uint8 *p;
		if (_adlibRhythmEnabled) {
			p = &_adlibChannelsKeyScalingTable2[channel * 2];
		} else {
			p = &_adlibChannelsKeyScalingTable1[channel * 2];
		}
		adlibSetupChannel(p[0], _adlibMetaSequenceData, _adlibMetaSequenceData[26]);
		if (p[1] != 255) {
			adlibSetupChannel(p[1], _adlibMetaSequenceData + 13, _adlibMetaSequenceData[27]);
		}
	}
}

void AdLibMidiDriver::handleSequencerSpecificMetaEvent2(uint8 value) {
	_adlibRhythmEnabled = value;
	_midiNumberOfChannels = _adlibRhythmEnabled ? 11 : 9;
	adlibSetAmpVibratoRhythm();
}

void AdLibMidiDriver::handleSequencerSpecificMetaEvent3(uint8 value) {
	adlibSetNoteMul(value);
}

void AdLibMidiDriver::handleMidiEvent0x90_NoteOn(int channel, int param1, int param2) { // note, volume
	if (param2 == 0) {
		adlibTurnNoteOff(channel);
		_adlibChannelsVolume[channel] = param2;
	} else {
		adlibSetNoteVolume(channel, param2);
		_adlibChannelsVolume[channel] = param2;
		adlibTurnNoteOff(channel);
		adlibTurnNoteOn(channel, param1);
	}
}

void AdLibMidiDriver::adlibWrite(uint8 port, uint8 value) {
	_opl->writeReg(port, value);
}

void AdLibMidiDriver::adlibSetupCard() {
	for (int i = 1; i <= 0xF5; ++i) {
		adlibWrite(i, 0);
	}
	adlibWrite(4, 6);
	for (int i = 0; i < 9; ++i) {
		_midiChannelsNote2Table[i] = 8192;
		_midiChannelsOctTable[i] = 0;
		_midiChannelsNote1Table[i] = 0;
		_midiChannelsFreqTable[i] = 0;
	}
	memset(_adlibChannelsLevelKeyScalingTable, 127, 11);
	memset(_adlibChannelsVolumeTable, 128, 11);
	adlibSetupChannels(0);
	adlibResetAmpVibratoRhythm(0, 0, 0);
	adlibSetNoteMul(1);
	adlibSetWaveformSelect(1);
}

void AdLibMidiDriver::adlibSetupChannels(int fl) {
	if (fl != 0) {
		_midiChannelsNote1Table[8] = 24;
		_midiChannelsNote2Table[8] = 8192;
		adlibPlayNote(8);
		_midiChannelsNote1Table[7] = 31;
		_midiChannelsNote2Table[7] = 8192;
		adlibPlayNote(7);
	}
	_adlibRhythmEnabled = fl;
	_midiNumberOfChannels = fl ? 11 : 9;
	_adlibVibratoRhythm = 0;
	_adlibAMDepthEq48 = 0;
	_adlibVibratoDepthEq14 = 0;
	_adlibKeyboardSplitOn = 0;
	adlibResetChannels();
	adlibSetAmpVibratoRhythm();
}

void AdLibMidiDriver::adlibResetAmpVibratoRhythm(int am, int vib, int kso) {
	_adlibAMDepthEq48 = am;
	_adlibVibratoDepthEq14 = vib;
	_adlibKeyboardSplitOn = kso;
	adlibSetAmpVibratoRhythm();
	adlibSetCSMKeyboardSplit();
}

void AdLibMidiDriver::adlibResetChannels() {
	for (int i = 0; i < 18; ++i) {
		adlibSetupChannelFromSequence(i, _adlibChannelsNoFeedback[i] ? _adlibInitSequenceData2 : _adlibInitSequenceData1, 0);
	}
	if (_adlibRhythmEnabled) {
		adlibSetupChannelFromSequence(12, _adlibInitSequenceData3, 0);
		adlibSetupChannelFromSequence(15, _adlibInitSequenceData4, 0);
		adlibSetupChannelFromSequence(16, _adlibInitSequenceData5, 0);
		adlibSetupChannelFromSequence(14, _adlibInitSequenceData6, 0);
		adlibSetupChannelFromSequence(17, _adlibInitSequenceData7, 0);
		adlibSetupChannelFromSequence(13, _adlibInitSequenceData8, 0);
	}
}

void AdLibMidiDriver::adlibSetAmpVibratoRhythm() {
	uint8 value = 0;
	if (_adlibAMDepthEq48) {
		value |= 0x80;
	}
	if (_adlibVibratoDepthEq14) {
		value |= 0x40;
	}
	if (_adlibRhythmEnabled) {
		value |= 0x20;
	}
	adlibWrite(0xBD, value | _adlibVibratoRhythm);
}

void AdLibMidiDriver::adlibSetCSMKeyboardSplit() {
	uint8 value = _adlibKeyboardSplitOn ? 0x40 : 0;
	adlibWrite(8, value);
}

void AdLibMidiDriver::adlibSetNoteMul(int mul) {
	if (mul > 12) {
		mul = 12;
	} else if (mul < 1) {
		mul = 1;
	}
	_adlibNoteMul = mul;
}

void AdLibMidiDriver::adlibSetWaveformSelect(int fl) {
	_adlibWaveformSelect = fl ? 0x20 : 0;
	for (int i = 0; i < 18; ++i) {
		adlibWrite(0xE0 + _adlibChannelsMappingTable1[i], 0);
	}
	adlibWrite(1, _adlibWaveformSelect);
}

void AdLibMidiDriver::adlibSetPitchBend(int channel, int range) {
	if ((_adlibRhythmEnabled && channel <= 6) || channel < 9) {
		if (range > 16383) {
			range = 16383;
		}
		_midiChannelsNote2Table[channel] = range;
		adlibPlayNote(channel);
	}
}

void AdLibMidiDriver::adlibPlayNote(int channel) {
	_midiChannelsFreqTable[channel] = adlibPlayNoteHelper(channel, _midiChannelsNote1Table[channel], _midiChannelsNote2Table[channel], _midiChannelsOctTable[channel]);
}

uint8 AdLibMidiDriver::adlibPlayNoteHelper(int channel, int note1, int note2, int oct) {
	int n = ((note2 * _midiChannelsNoteTable[channel]) >> 8) - 8192;
	if (n != 0) {
		n >>= 5;
		n *= _adlibNoteMul;
	}
	n += (note1 << 8) + 8;
	n >>= 4;
	if (n < 0) {
		n = 0;
	} else if (n > 1535) {
		n = 1535;
	}
	int index = (((n >> 4) % 12) << 4) | (n & 0xF);
	int f = _midiNoteFreqTable[index];
	int o = (n >> 4) / 12 - 1;
	if (f < 0) {
		++o;
	}
	if (o < 0) {
		++o;
		f >>= 1;
	}
	adlibWrite(0xA0 + channel, f & 0xFF);
	int value = ((f >> 8) & 3) | (o << 2) | oct;
	adlibWrite(0xB0 + channel, value);
	return value;
}

void AdLibMidiDriver::adlibTurnNoteOff(int channel) {
	if ((_adlibRhythmEnabled && channel <= 6) || channel < 9) {
		_midiChannelsOctTable[channel] = 0;
		_midiChannelsFreqTable[channel] &= ~0x20;
		adlibWrite(0xB0 + channel, _midiChannelsFreqTable[channel]);
	} else if (_adlibRhythmEnabled && channel <= 10) {
		_adlibVibratoRhythm &= ~(1 << (4 - (channel - 6)));
		adlibSetAmpVibratoRhythm();
	}
}

void AdLibMidiDriver::adlibTurnNoteOn(int channel, int note) {
	note -= 12;
	if (note < 0) {
		note = 0;
	}
	if ((_adlibRhythmEnabled && channel <= 6) || channel < 9) {
		_midiChannelsNote1Table[channel] = note;
		_midiChannelsOctTable[channel] = 0x20;
		adlibPlayNote(channel);
	} else if (_adlibRhythmEnabled && channel <= 10) {
		if (channel == 6) {
			_midiChannelsNote1Table[6] = note;
			adlibPlayNote(channel);
		} else if (channel == 8 && _midiChannelsNote1Table[8] == note) {
			_midiChannelsNote1Table[8] = note;
			_midiChannelsNote1Table[7] = note + 7;
			adlibPlayNote(8);
			adlibPlayNote(7);
		}
		_adlibVibratoRhythm = 1 << (4 - (channel - 6));
		adlibSetAmpVibratoRhythm();
	}
}

void AdLibMidiDriver::adlibSetupChannelFromSequence(int channel, const uint8 *src, int fl) {
	for (int i = 0; i < 13; ++i) {
		_adlibSetupChannelSequence2[i] = src[i];
	}
	adlibSetupChannel(channel, _adlibSetupChannelSequence2, fl);
}

void AdLibMidiDriver::adlibSetupChannel(int channel, const uint16 *src, int fl) {
	for (int i = 0; i < 13; ++i) {
		_adlibSetupChannelSequence1[14 * channel + i] = src[i];
	}
	_adlibSetupChannelSequence1[14 * channel + 13] = fl & 3;
	adlibSetupChannelHelper(channel);
}

void AdLibMidiDriver::adlibSetNoteVolume(int channel, int volume) {
	if (_midiNumberOfChannels > channel) {
		if (volume > 127) {
			volume = 127;
		}
		_adlibChannelsLevelKeyScalingTable[channel] = volume;
		const uint8 *p;
		if (_adlibRhythmEnabled) {
			p = &_adlibChannelsKeyScalingTable2[channel * 2];
		} else {
			p = &_adlibChannelsKeyScalingTable1[channel * 2];
		}
		adlibSetChannel0x40(p[0]);
		if (p[1] != 255) {
			adlibSetChannel0x40(p[1]);
		}
	}
}

void AdLibMidiDriver::adlibSetChannelVolume(int channel, uint8 volume) {
	if (channel < (_adlibRhythmEnabled ? 11 : 9))
		_adlibChannelsVolumeTable[channel] = volume;
}

void AdLibMidiDriver::adlibSetupChannelHelper(int channel) {
	adlibSetAmpVibratoRhythm();
	adlibSetCSMKeyboardSplit();
	adlibSetChannel0x40(channel);
	adlibSetChannel0xC0(channel);
	adlibSetChannel0x60(channel);
	adlibSetChannel0x80(channel);
	adlibSetChannel0x20(channel);
	adlibSetChannel0xE0(channel);
}

void AdLibMidiDriver::adlibSetChannel0x40(int channel) {
	int index, value, fl;

	if (_adlibRhythmEnabled) {
		index = _adlibChannelsMappingTable3[channel];
	} else {
		index = _adlibChannelsMappingTable2[channel];
	}
	value = 63 - (_adlibSetupChannelSequence1[channel * 14 + 8] & 63);
	fl = 0;
	if (_adlibRhythmEnabled && index > 6) {
		fl = -1;
	}
	if (_adlibChannelsNoFeedback[channel] || _adlibSetupChannelSequence1[channel * 14 + 12] == 0 || fl != 0) {
		value = ((_adlibChannelsLevelKeyScalingTable[index] * value) + 64) >> 7;
	}
	value = (_adlibChannelsVolumeTable[index] * value * 2) >> 8;
	if (value > 63) {
		value = 63;
	}
	value = 63 - value;
	value |= _adlibSetupChannelSequence1[channel * 14] << 6;
	adlibWrite(0x40 + _adlibChannelsMappingTable1[channel], value);
}

void AdLibMidiDriver::adlibSetChannel0xC0(int channel) {
	if (_adlibChannelsNoFeedback[channel] == 0) {
		const uint8 *p = &_adlibSetupChannelSequence1[channel * 14];
		uint8 value = p[2] << 1;
		if (p[12] == 0) {
			value |= 1;
		}
		adlibWrite(0xC0 + _adlibChannelsMappingTable2[channel], value);
	}
}

void AdLibMidiDriver::adlibSetChannel0x60(int channel) {
	const uint8 *p = &_adlibSetupChannelSequence1[channel * 14];
	uint8 value = (p[3] << 4) | (p[6] & 15);
	adlibWrite(0x60 + _adlibChannelsMappingTable1[channel], value);
}

void AdLibMidiDriver::adlibSetChannel0x80(int channel) {
	const uint8 *p = &_adlibSetupChannelSequence1[channel * 14];
	uint8 value = (p[4] << 4) | (p[7] & 15);
	adlibWrite(0x80 + _adlibChannelsMappingTable1[channel], value);
}

void AdLibMidiDriver::adlibSetChannel0x20(int channel) {
	const uint8 *p = &_adlibSetupChannelSequence1[channel * 14];
	uint8 value = p[1] & 15;
	if (p[9]) {
		value |= 0x80;
	}
	if (p[10]) {
		value |= 0x40;
	}
	if (p[5]) {
		value |= 0x20;
	}
	if (p[11]) {
		value |= 0x10;
	}
	adlibWrite(0x20 + _adlibChannelsMappingTable1[channel], value);
}

void AdLibMidiDriver::adlibSetChannel0xE0(int channel) {
	uint8 value = 0;
	if (_adlibWaveformSelect) {
		const uint8 *p = &_adlibSetupChannelSequence1[channel * 14];
		value = p[13] & 3;
	}
	adlibWrite(0xE0 + _adlibChannelsMappingTable1[channel], value);
}

const uint8 AdLibMidiDriver::_adlibChannelsMappingTable1[] = {
	0, 1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 16, 17, 18, 19, 20, 21};

const uint8 AdLibMidiDriver::_adlibChannelsNoFeedback[] = {
	0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1};

const uint8 AdLibMidiDriver::_adlibChannelsMappingTable2[] = {
	0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5, 6, 7, 8, 6, 7, 8};

const uint8 AdLibMidiDriver::_adlibChannelsMappingTable3[] = {
	0, 1, 2, 0, 1, 2, 3, 4, 5, 3, 4, 5, 6, 10, 8, 6, 7, 9};

const uint8 AdLibMidiDriver::_adlibChannelsKeyScalingTable1[] = {
	0, 3, 1, 4, 2, 5, 6, 9, 7, 10, 8, 11, 12, 15, 13, 16, 14, 17};

const uint8 AdLibMidiDriver::_adlibChannelsKeyScalingTable2[] = {
	0, 3, 1, 4, 2, 5, 6, 9, 7, 10, 8, 11, 12, 15, 16, 255, 14, 255, 17, 255, 13, 255};

const uint8 AdLibMidiDriver::_adlibInitSequenceData1[] = {
	1, 1, 3, 15, 5, 0, 1, 3, 15, 0, 0, 0, 1, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData2[] = {
	0, 1, 1, 15, 7, 0, 2, 4, 0, 0, 0, 1, 0, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData3[] = {
	0, 0, 0, 10, 4, 0, 8, 12, 11, 0, 0, 0, 1, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData4[] = {
	0, 0, 0, 13, 4, 0, 6, 15, 0, 0, 0, 0, 1, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData5[] = {
	0, 12, 0, 15, 11, 0, 8, 5, 0, 0, 0, 0, 0, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData6[] = {
	0, 4, 0, 15, 11, 0, 7, 5, 0, 0, 0, 0, 0, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData7[] = {
	0, 1, 0, 15, 11, 0, 5, 5, 0, 0, 0, 0, 0, 0};

const uint8 AdLibMidiDriver::_adlibInitSequenceData8[] = {
	0, 1, 0, 15, 11, 0, 7, 5, 0, 0, 0, 0, 0, 0};

const int16 AdLibMidiDriver::_midiChannelsNoteTable[] = {
	256, 256, 256, 256, 256, 256, 256, 256, 256, 256, 256};

const int16 AdLibMidiDriver::_midiNoteFreqTable[] = {
	690, 692, 695, 697, 700, 702, 705, 707, 710, 713, 715, 718,
	720, 723, 726, 728, 731, 733, 736, 739, 741, 744, 747, 749,
	752, 755, 758, 760, 763, 766, 769, 771, 774, 777, 780, 783,
	785, 788, 791, 794, 797, 800, 803, 806, 809, 811, 814, 817,
	820, 823, 826, 829, 832, 835, 838, 841, 844, 847, 850, 854,
	857, 860, 863, 866, 869, 872, 875, 879, 882, 885, 888, 891,
	895, 898, 901, 904, 908, 911, 914, 917, 921, 924, 927, 931,
	934, 937, 941, 944, 948, 951, 955, 958, 961, 965, 968, 972,
	975, 979, 983, 986, 990, 993, 997, 1000, 1004, 1008, 1011, 1015,
	1019, 1022, -511, -509, -507, -505, -504, -502, -500, -498, -496, -494,
	-492, -490, -488, -486, -484, -482, -480, -479, -477, -475, -473, -471,
	-469, -467, -465, -463, -460, -458, -456, -454, -452, -450, -448, -446,
	-444, -442, -440, -438, -436, -433, -431, -429, -427, -425, -423, -420,
	-418, -416, -414, -412, -409, -407, -405, -403, -401, -398, -396, -394,
	-391, -389, -387, -385, -382, -380, -378, -375, -373, -371, -368, -366,
	-363, -361, -359, -356, -354, -351, -349, -347, -344, -342, -339, -337};

} // namespace Dune
