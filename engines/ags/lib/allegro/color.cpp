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

#include "ags/lib/allegro/color.h"
#include "common/textconsole.h"
#include "common/system.h"
#include "graphics/palette.h"

namespace AGS3 {

int _rgb_r_shift_15 = 0;
int _rgb_g_shift_15 = 0;
int _rgb_b_shift_15 = 0;
int _rgb_r_shift_16 = 0;
int _rgb_g_shift_16 = 0;
int _rgb_b_shift_16 = 0;
int _rgb_r_shift_24 = 0;
int _rgb_g_shift_24 = 0;
int _rgb_b_shift_24 = 0;
int _rgb_r_shift_32 = 0;
int _rgb_g_shift_32 = 0;
int _rgb_b_shift_32 = 0;
int _rgb_a_shift_32 = 0;

AL_ARRAY(int, _rgb_scale_5);
AL_ARRAY(int, _rgb_scale_6);

PALETTE _current_palette;

int bestfit_color(const PALETTE pal, int r, int g, int b) {
	error("TODO: bestfit_color");
}

void set_color(int idx, const RGB *p) {
	_current_palette[idx] = *p;
	g_system->getPaletteManager()->setPalette((const byte *)p, idx, 1);
}

void set_palette(const PALETTE p) {
	for (int idx = 0; idx < PAL_SIZE; ++idx)
		_current_palette[idx] = p[idx];
	g_system->getPaletteManager()->setPalette((const byte *)p, 0, PAL_SIZE);
}

void set_palette_range(const PALETTE p, int from, int to, int retracesync) {
	byte palette[256 * 3];
	byte *destP = palette;
	for (int i = 0; i < 256; ++i, destP += 3) {
		_current_palette[i] = *p;
		destP[0] = p->r;
		destP[1] = p->g;
		destP[2] = p->b;
	}

	g_system->getPaletteManager()->setPalette(&palette[from], from, to - from + 1);
}

int makeacol(int r, int g, int b, int a) {
	error("TODO: makeacol");
}

int makeacol_depth(int color_depth, int r, int g, int b, int a) {
	error("TODO: makeacol_depth");
}

void hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b) {
	error("TODO: hsv_to_rgb");
}

void rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v) {
	error("TODO: rgb_to_hsv");
}

int makecol15(int r, int g, int b) {
	return (((r >> 3) << _rgb_r_shift_15) |
		((g >> 3) << _rgb_g_shift_15) |
		((b >> 3) << _rgb_b_shift_15));
}

int makecol16(int r, int g, int b) {
	return (((r >> 3) << _rgb_r_shift_16) |
		((g >> 2) << _rgb_g_shift_16) |
		((b >> 3) << _rgb_b_shift_16));
}

int makecol24(int r, int g, int b) {
	return ((r << _rgb_r_shift_24) |
		(g << _rgb_g_shift_24) |
		(b << _rgb_b_shift_24));
}

int makecol32(int r, int g, int b) {
	return ((r << _rgb_r_shift_32) |
		(g << _rgb_g_shift_32) |
		(b << _rgb_b_shift_32));
}

int makeacol32(int r, int g, int b, int a) {
	return ((r << _rgb_r_shift_32) |
		(g << _rgb_g_shift_32) |
		(b << _rgb_b_shift_32) |
		(a << _rgb_a_shift_32));
}

int getr8(int c) {
	return (int)_current_palette[c].r;
}

int getg8(int c) {
	return (int)_current_palette[c].g;
}

int getb8(int c) {
	return (int)_current_palette[c].b;
}

int getr15(int c) {
   return _rgb_scale_5[(c >> _rgb_r_shift_15) & 0x1F];
}

int getg15(int c) {
	return _rgb_scale_5[(c >> _rgb_g_shift_15) & 0x1F];
}

int getb15(int c) {
   return _rgb_scale_5[(c >> _rgb_b_shift_15) & 0x1F];
}

int getr16(int c) {
   return _rgb_scale_5[(c >> _rgb_r_shift_16) & 0x1F];
}

int getg16(int c) {
   return _rgb_scale_6[(c >> _rgb_g_shift_16) & 0x3F];
}

int getb16(int c) {
   return _rgb_scale_5[(c >> _rgb_b_shift_16) & 0x1F];
}

int getr32(int c) {
	error("TODO: getr32");
}

int getg32(int c) {
	error("TODO: getg32");
}

int getb32(int c) {
	error("TODO: getb32");
}

int geta32(int c) {
	error("TODO: geta32");
}

int makecol(byte r, byte g, byte b) {
	return (b) | (g << 8) | (r << 16);
}

int makecol8(byte r, byte g, byte b) {
	return (b) | (g << 8) | (r << 16);
}

int getr_depth(int color_depth, int c) {
	error("TOD: getr_depth");
}

int getg_depth(int color_depth, int c) {
	error("TOD: getg_depth");
}

int getb_depth(int color_depth, int c) {
	error("TOD: getb_depth");
}

int geta_depth(int color_depth, int c) {
	error("TOD: geta_depth");
}

} // namespace AGS3