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

#include "dune/music.h"

#include "dune/dune.h"

#include "common/debug.h"

namespace Dune {
AgdPlayer::AgdPlayer(DuneEngine *vm) : _vm(vm) {
	_reader = nullptr;
}

AgdPlayer::~AgdPlayer() {
	if (track) {
		for (int i = 0; i < nTracks; i++) {
			if (track[i].data)
				delete[] track[i].data;
		}
		delete[] track;
	}
	if (chn)
		delete[] chn;
	if (inst)
		delete[] inst;
}

const uint8_t AgdPlayer::slot_offset[HERAD_NUM_VOICES] = {
	0, 1, 2, 8, 9, 10, 16, 17, 18};
const uint16_t AgdPlayer::FNum[HERAD_NUM_NOTES] = {
	343, 364, 385, 408, 433, 459, 486, 515, 546, 579, 614, 650};
const uint8_t AgdPlayer::fine_bend[HERAD_NUM_NOTES + 1] = {
	19, 21, 21, 23, 25, 26, 27, 29, 31, 33, 35, 36, 37};
const uint8_t AgdPlayer::coarse_bend[10] = {
	0, 5, 10, 15, 20,
	0, 6, 12, 18, 24};

bool AgdPlayer::update() {
	wTime = wTime - 256;
	if (wTime < 0) {
		wTime = wTime + wSpeed;
		processEvents();
	}
	return !songend;
}

void AgdPlayer::rewind(int subsong) {
	uint32_t j;
	wTime = 0;
	songend = false;

	ticks_pos = -1; // there's always 1 excess tick at start
	total_ticks = 0;
	loop_pos = -1;
	loop_times = 1;

	for (int i = 0; i < nTracks; i++) {
		track[i].pos = 0;
		j = 0;
		while (track[i].pos < track[i].size) {
			j += GetTicks(i);
			switch (track[i].data[track[i].pos++] & 0xF0) {
			case 0x80: // Note Off
				track[i].pos += (v2 ? 1 : 2);
				break;
			case 0x90: // Note On
			case 0xA0: // Unused
			case 0xB0: // Unused
				track[i].pos += 2;
				break;
			case 0xC0: // Program Change
			case 0xD0: // Aftertouch
			case 0xE0: // Pitch Bend
				track[i].pos++;
				break;
			default:
				track[i].pos = track[i].size;
				break;
			}
		}
		if (j > total_ticks)
			total_ticks = j;
		track[i].pos = 0;
		track[i].counter = 0;
		track[i].ticks = 0;
		chn[i].program = 0;
		chn[i].playprog = 0;
		chn[i].note = 0;
		chn[i].keyon = false;
		chn[i].bend = HERAD_BEND_CENTER;
		chn[i].slide_dur = 0;
	}
	if (v2) {
		if (!wLoopStart || wLoopCount)
			wLoopStart = 1; // if loop not specified, start from beginning
		if (!wLoopEnd || wLoopCount)
			wLoopEnd = getpatterns() + 1; // till the end
		if (wLoopCount)
			wLoopCount = 0; // repeats forever
	}

	//opl->init();
	//opl->write(1, 32);   // Enable Waveform Select
	//opl->write(0xBD, 0); // Disable Percussion Mode
	//opl->write(8, 64);   // Enable Note-Sel
	//if (AGD) {
	//	opl->setchip(1);
	//	opl->write(5, 1); // Enable OPL3
	//	opl->write(4, 0); // Disable 4OP Mode
	//	opl->setchip(0);
	//}
}

Common::String AgdPlayer::gettype() {
	char scomp[12 + 1] = "";
	if (comp > HERAD_COMP_NONE) {
		debug(scomp, ", %s packed", (comp == HERAD_COMP_HSQ ? "HSQ" : "SQX"));
	}
	char type[40 + 1];
	debug(type, "HERAD System %s (version %d%s)", (AGD ? "AGD" : "SDB"), (v2 ? 2 : 1), scomp);
	return Common::String(type);
}

void AgdPlayer::load(Common::SeekableReadStream *reader) {
	_reader = reader;
}

uint32_t AgdPlayer::GetTicks(uint8_t t) {
	uint32_t result = 0;
	do {
		result <<= 7;
		result |= track[t].data[track[t].pos] & 0x7F;
	} while (track[t].data[track[t].pos++] & 0x80 && track[t].pos < track[t].size);
	return result;
}
void AgdPlayer::executeCommand(uint8_t t) {
	uint8_t status, note, par;

	if (t >= nTracks)
		return;

	if (t >= (AGD ? HERAD_NUM_VOICES * 2 : HERAD_NUM_VOICES)) {
		track[t].pos = track[t].size;
		return;
	}

	// execute MIDI command
	status = track[t].data[track[t].pos++];
	if (status == 0xFF) {
		track[t].pos = track[t].size;
	} else {
		switch (status & 0xF0) {
		case 0x80: // Note Off
			note = track[t].data[track[t].pos++];
			par = (v2 ? 0 : track[t].data[track[t].pos++]);
			ev_noteOff(t, note, par);
			break;
		case 0x90: // Note On
			note = track[t].data[track[t].pos++];
			par = track[t].data[track[t].pos++];
			ev_noteOn(t, note, par);
			break;
		case 0xA0: // Unused
		case 0xB0: // Unused
			track[t].pos += 2;
			break;
		case 0xC0: // Program Change
			par = track[t].data[track[t].pos++];
			ev_programChange(t, par);
			break;
		case 0xD0: // Aftertouch
			par = track[t].data[track[t].pos++];
			ev_aftertouch(t, par);
			break;
		case 0xE0: // Pitch Bend
			par = track[t].data[track[t].pos++];
			ev_pitchBend(t, par);
			break;
		default:
			track[t].pos = track[t].size;
			break;
		}
	}
}
void AgdPlayer::processEvents() {
	uint8_t i;
	songend = true;

	if (wLoopStart && wLoopEnd && (ticks_pos + 1) % HERAD_MEASURE_TICKS == 0 && (ticks_pos + 1) / HERAD_MEASURE_TICKS + 1 == wLoopStart) {
		loop_pos = ticks_pos;
		for (i = 0; i < nTracks; i++) {
			loop_data[i].counter = track[i].counter;
			loop_data[i].ticks = track[i].ticks;
			loop_data[i].pos = track[i].pos;
		}
	}
	for (i = 0; i < nTracks; i++) {
		if (chn[i].slide_dur > 0 && chn[i].keyon)
			macroSlide(i);
		if (track[i].pos >= track[i].size)
			continue;
		songend = false; // track is not finished
		if (!track[i].counter) {
			bool first = track[i].pos == 0;
			track[i].ticks = GetTicks(i);
			if (first && track[i].ticks)
				track[i].ticks++; // workaround to synchronize tracks (there's always 1 excess tick at start)
		}
		if (++track[i].counter >= track[i].ticks) {
			track[i].counter = 0;
			while (track[i].pos < track[i].size) {
				executeCommand(i);
				if (track[i].pos >= track[i].size) {
					break;
				} else if (!track[i].data[track[i].pos]) // if next delay is zero
				{
					track[i].pos++;
				} else
					break;
			}
		} else if (track[i].ticks >= 0x8000) {
			track[i].pos = track[i].size;
			track[i].counter = track[i].ticks;
		}
	}
	if (!songend)
		ticks_pos++;
	if (wLoopStart && wLoopEnd && (ticks_pos == total_ticks || (ticks_pos % HERAD_MEASURE_TICKS == 0 && ticks_pos / HERAD_MEASURE_TICKS + 1 == wLoopEnd))) {
#ifdef HERAD_USE_LOOPING
		if (!wLoopCount)
			songend = true;
		else if (songend && loop_times < wLoopCount)
			songend = false;

		if (!wLoopCount || loop_times < wLoopCount) {
			ticks_pos = loop_pos;
			for (i = 0; i < nTracks; i++) {
				track[i].counter = loop_data[i].counter;
				track[i].ticks = loop_data[i].ticks;
				track[i].pos = loop_data[i].pos;
			}
			if (wLoopCount)
				loop_times++;
		}
#endif
	}
}
void AgdPlayer::ev_noteOn(uint8_t ch, uint8_t note, uint8_t vel) {
	int8_t macro;

	if (chn[ch].keyon) {
		// turn off last active note
		chn[ch].keyon = false;
		playNote(ch, chn[ch].note, HERAD_NOTE_OFF);
	}
	if (v2 && inst[chn[ch].program].param.mode == HERAD_INSTMODE_KMAP) {
		// keymap is used
		int8_t mp = note - (inst[chn[ch].program].keymap.offset + 24);
		if (mp < 0 || mp >= HERAD_INST_SIZE - 4)
			return; // if not in range, skip note
		chn[ch].playprog = inst[chn[ch].program].keymap.index[mp];
		changeProgram(ch, chn[ch].playprog);
	}
	chn[ch].note = note;
	chn[ch].keyon = true;
	chn[ch].bend = HERAD_BEND_CENTER;
	if (v2 && inst[chn[ch].playprog].param.mode == HERAD_INSTMODE_KMAP)
		return; // single keymapped instrument can't be keymap (avoid recursion)
	playNote(ch, note, HERAD_NOTE_ON);
	macro = inst[chn[ch].playprog].param.mc_mod_out_vel;
	if (macro != 0)
		macroModOutput(ch, chn[ch].playprog, macro, vel);
	macro = inst[chn[ch].playprog].param.mc_car_out_vel;
	if (macro != 0)
		macroCarOutput(ch, chn[ch].playprog, macro, vel);
	macro = inst[chn[ch].playprog].param.mc_fb_vel;
	if (macro != 0)
		macroFeedback(ch, chn[ch].playprog, macro, vel);
}
void AgdPlayer::ev_noteOff(uint8_t ch, uint8_t note, uint8_t vel) {
	if (note != chn[ch].note || !chn[ch].keyon)
		return;
	chn[ch].keyon = false;
	playNote(ch, note, HERAD_NOTE_OFF);
}
void AgdPlayer::ev_programChange(uint8_t ch, uint8_t prog) {
	if (prog >= nInsts) // out of index
		return;
	chn[ch].program = prog;
	chn[ch].playprog = prog;
	changeProgram(ch, prog);
}
void AgdPlayer::ev_aftertouch(uint8_t ch, uint8_t vel) {
	int8_t macro;

	if (v2) // version 2 ignores this event
		return;
	macro = inst[chn[ch].playprog].param.mc_mod_out_at;
	if (macro != 0)
		macroModOutput(ch, chn[ch].playprog, macro, vel);
	macro = inst[chn[ch].playprog].param.mc_car_out_at;
	if (macro != 0 && inst[chn[ch].playprog].param.mc_car_out_vel != 0)
		macroCarOutput(ch, chn[ch].playprog, macro, vel);
	macro = inst[chn[ch].playprog].param.mc_fb_at;
	if (macro != 0)
		macroFeedback(ch, chn[ch].playprog, macro, vel);
}
void AgdPlayer::ev_pitchBend(uint8_t ch, uint8_t bend) {
	chn[ch].bend = bend;
	if (chn[ch].keyon) // update pitch
		playNote(ch, chn[ch].note, HERAD_NOTE_UPDATE);
}
void AgdPlayer::playNote(uint8_t c, uint8_t note, uint8_t state) {
	if (inst[chn[c].playprog].param.mc_transpose != 0)
		macroTranspose(&note, chn[c].playprog);
	note = (note - 24) & 0xFF;
	if (state != HERAD_NOTE_UPDATE && note >= 0x60)
		note = 0; // clip too low/high notes
	int8_t oct = note / HERAD_NUM_NOTES;
	int8_t key = note % HERAD_NUM_NOTES;
	if (state != HERAD_NOTE_UPDATE && inst[chn[c].playprog].param.mc_slide_dur) {
		chn[c].slide_dur = (state == HERAD_NOTE_ON ? inst[chn[c].playprog].param.mc_slide_dur : 0);
	}
	uint8_t bend = chn[c].bend;
	int16_t amount, detune = 0;
	uint8_t amount_lo, amount_hi;
	if (!(inst[chn[c].playprog].param.mc_slide_coarse & 1)) { // fine tune
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
void AgdPlayer::setFreq(uint8_t c, uint8_t oct, uint16_t freq, bool on) {
	uint8_t reg, val;

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(1);

	reg = 0xA0 + (c % HERAD_NUM_VOICES);
	val = freq & 0xFF;
	//opl->write(reg, val);
	reg = 0xB0 + (c % HERAD_NUM_VOICES);
	val = ((freq >> 8) & 3) |
		  ((oct & 7) << 2) |
		  ((on ? 1 : 0) << 5);
	//opl->write(reg, val);

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(0);
}
void AgdPlayer::changeProgram(uint8_t c, uint8_t i) {
	uint8_t reg, val;

	if (v2 && inst[i].param.mode == HERAD_INSTMODE_KMAP)
		return;

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(1);

	// Amp Mod / Vibrato / EG type / Key Scaling / Multiple
	reg = 0x20 + slot_offset[c % HERAD_NUM_VOICES];
	val = (inst[i].param.mod_mul & 15) |
		  ((inst[i].param.mod_ksr & 1) << 4) |
		  ((inst[i].param.mod_eg > 0 ? 1 : 0) << 5) |
		  ((inst[i].param.mod_vib & 1) << 6) |
		  ((inst[i].param.mod_am & 1) << 7);
	//opl->write(reg, val);
	reg += 3;
	val = (inst[i].param.car_mul & 15) |
		  ((inst[i].param.car_ksr & 1) << 4) |
		  ((inst[i].param.car_eg > 0 ? 1 : 0) << 5) |
		  ((inst[i].param.car_vib & 1) << 6) |
		  ((inst[i].param.car_am & 1) << 7);
	//opl->write(reg, val);

	// Key scaling level / Output level
	reg = 0x40 + slot_offset[c % HERAD_NUM_VOICES];
	val = (inst[i].param.mod_out & 63) |
		  ((inst[i].param.mod_ksl & 3) << 6);
	//opl->write(reg, val);
	reg += 3;
	val = (inst[i].param.car_out & 63) |
		  ((inst[i].param.car_ksl & 3) << 6);
	//opl->write(reg, val);

	// Attack Rate / Decay Rate
	reg = 0x60 + slot_offset[c % HERAD_NUM_VOICES];
	val = (inst[i].param.mod_D & 15) |
		  ((inst[i].param.mod_A & 15) << 4);
	//opl->write(reg, val);
	reg += 3;
	val = (inst[i].param.car_D & 15) |
		  ((inst[i].param.car_A & 15) << 4);
	//opl->write(reg, val);

	// Sustain Level / Release Rate
	reg = 0x80 + slot_offset[c % HERAD_NUM_VOICES];
	val = (inst[i].param.mod_R & 15) |
		  ((inst[i].param.mod_S & 15) << 4);
	//opl->write(reg, val);
	reg += 3;
	val = (inst[i].param.car_R & 15) |
		  ((inst[i].param.car_S & 15) << 4);
	//opl->write(reg, val);

	// Panning / Feedback strength / Connection type
	reg = 0xC0 + (c % HERAD_NUM_VOICES);
	val = (inst[i].param.con > 0 ? 0 : 1) |
		  ((inst[i].param.feedback & 7) << 1) |
		  ((AGD ? (inst[i].param.pan == 0 || inst[i].param.pan > 3 ? 3 : inst[i].param.pan) : 0) << 4);
	//opl->write(reg, val);

	// Wave Select
	reg = 0xE0 + slot_offset[c % HERAD_NUM_VOICES];
	val = inst[i].param.mod_wave & (AGD ? 7 : 3);
	//opl->write(reg, val);
	reg += 3;
	val = inst[i].param.car_wave & (AGD ? 7 : 3);
	//opl->write(reg, val);

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(0);
}
void AgdPlayer::macroModOutput(uint8_t c, uint8_t i, int8_t sens, uint8_t level) {
	uint8_t reg, val;
	uint16_t output;

	if (sens < -4 || sens > 4)
		return;

	if (sens < 0) {
		output = (level >> (sens + 4) > 63 ? 63 : level >> (sens + 4));
	} else {
		output = ((0x80 - level) >> (4 - sens) > 63 ? 63 : (0x80 - level) >> (4 - sens));
	}
	output += inst[i].param.mod_out;
	if (output > 63)
		output = 63;

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(1);

	// Key scaling level / Output level
	reg = 0x40 + slot_offset[c % HERAD_NUM_VOICES];
	val = (output & 63) |
		  ((inst[i].param.mod_ksl & 3) << 6);
	//opl->write(reg, val);

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(0);
}
void AgdPlayer::macroCarOutput(uint8_t c, uint8_t i, int8_t sens, uint8_t level) {
	uint8_t reg, val;
	uint16_t output;

	if (sens < -4 || sens > 4)
		return;

	if (sens < 0) {
		output = (level >> (sens + 4) > 63 ? 63 : level >> (sens + 4));
	} else {
		output = ((0x80 - level) >> (4 - sens) > 63 ? 63 : (0x80 - level) >> (4 - sens));
	}
	output += inst[i].param.car_out;
	if (output > 63)
		output = 63;

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(1);

	// Key scaling level / Output level
	reg = 0x43 + slot_offset[c % HERAD_NUM_VOICES];
	val = (output & 63) |
		  ((inst[i].param.car_ksl & 3) << 6);
	//opl->write(reg, val);

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(0);
}
void AgdPlayer::macroFeedback(uint8_t c, uint8_t i, int8_t sens, uint8_t level) {
	uint8_t reg, val;
	uint8_t feedback;

	if (sens < -6 || sens > 6)
		return;

	if (sens < 0) {
		feedback = (level >> (sens + 7) > 7 ? 7 : level >> (sens + 7));
	} else {
		feedback = ((0x80 - level) >> (7 - sens) > 7 ? 7 : (0x80 - level) >> (7 - sens));
	}
	feedback += inst[i].param.feedback;
	if (feedback > 7)
		feedback = 7;

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(1);

	// Panning / Feedback strength / Connection type
	reg = 0xC0 + (c % HERAD_NUM_VOICES);
	val = (inst[i].param.con > 0 ? 0 : 1) |
		  ((feedback & 7) << 1) |
		  ((AGD ? (inst[i].param.pan == 0 || inst[i].param.pan > 3 ? 3 : inst[i].param.pan) : 0) << 4);
	//opl->write(reg, val);

	//if (c >= HERAD_NUM_VOICES)
	//opl->setchip(0);
}
void AgdPlayer::macroTranspose(uint8_t *note, uint8_t i) {
	uint8_t tran = inst[i].param.mc_transpose;
	uint8_t diff = (tran - 0x31) & 0xFF;
	if (v2 && diff < 0x60)
		*note = (diff + 0x18) & 0xFF;
	else
		*note = (*note + tran) & 0xFF;
}
void AgdPlayer::macroSlide(uint8_t c) {
	if (!chn[c].slide_dur)
		return;

	chn[c].slide_dur--;
	chn[c].bend += inst[chn[c].playprog].param.mc_slide_range;

	if (!(chn[c].note & 0x7F))
		return;
	playNote(c, chn[c].note, HERAD_NOTE_UPDATE);
}
} // namespace Dune
