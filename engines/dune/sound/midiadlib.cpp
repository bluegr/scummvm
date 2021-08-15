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

#include "dune/sound/midiadlib.h"
#include "dune/dune.h"

#include "common/debug.h"

namespace Dune {

AdLibMidiDriver::AdLibMidiDriver(DuneEngine *vm) : _vm(vm) {
	_buf_size = 2048;
	_bits = 16;
	_nChannels = 2;
	_frequency = 44100;
	_reader = nullptr;
	_opl = nullptr;
	open();
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
	0, 1, 2, 8, 9, 10, 16, 17, 18
};

const uint16_t AdLibMidiDriver::FNum[HERAD_NUM_NOTES] = {
	343, 364, 385, 408, 433, 459, 486, 515, 546, 579, 614, 650
};

const uint8_t AdLibMidiDriver::fine_bend[HERAD_NUM_NOTES + 1] = {
	19, 21, 21, 23, 25, 26, 27, 29, 31, 33, 35, 36, 37};

const uint8_t AdLibMidiDriver::coarse_bend[10] = {
	0, 5, 10, 15, 20,
	0, 6, 12, 18, 24
};

bool AdLibMidiDriver::update() {
	_wTime = _wTime - 256;
	if (_wTime < 0) {
		_wTime = _wTime + _wSpeed;
		processEvents();
	}
	return !_songEnd;
}

bool AdLibMidiDriver::isPlaying() {
	return _isPlaying;
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
				_tracks[i].pos += 2; //HeradV1 specific (DUNE uses Herad V1)
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

void AdLibMidiDriver::load(Common::SeekableReadStream *reader) {
	open();
	_reader = reader;
	long size = _reader->size();
	// Read entire file into memory
	uint8_t *data = new uint8_t[size];
	_reader->read(data, size);
	_fileCompressionType = HERAD_COMP_NONE;
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
	for (int i = 0; i < _nInstruments; i++) {
		memcpy(_instruments[i].data, data + offset + i * HERAD_INST_SIZE, HERAD_INST_SIZE);
	}
	delete[] data;
	rewind(0);
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
			//HeradV1 specific (DUNE uses Herad V1)
			par = (_tracks[t].data[_tracks[t].pos++]);
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
		if (_tracks[i].pos >= _tracks[i].size) {
			_songEnd = true;
			continue;
		}
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
	_channels[ch].note = note;
	_channels[ch].keyon = true;
	_channels[ch].bend = HERAD_BEND_CENTER;
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
	//HeradV1 specific (DUNE uses Herad V1)
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
	//TODO: remove this. We do not use this code.
	_adlibTimerProc = timerProc;
	_adlibTimerParam = timerParam;
}

void AdLibMidiDriver::onTimer() {
	if (_frameStop > -1 && _frameCount >= _frameStop) {
		return;
	}
	_isPlaying = update();
	_frameCount++;
}

void AdLibMidiDriver::setVolume(uint32 volume) {
	for (int i = 0; i < _midiNumberOfChannels; ++i)
		adlibSetChannelVolume(i, volume * 64 / 256 + 64);
}

void AdLibMidiDriver::adlibWrite(uint8 port, uint8 value) {
	debug("Reg.: 0x%X Value: 0x%X", port, value);
	_opl->writeReg(port, value);
}

void AdLibMidiDriver::adlibSetChannelVolume(int channel, uint8 volume) {
	_adlibChannelsVolumeTable[channel] = volume;
}
} // namespace Dune
