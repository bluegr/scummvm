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

#include "dune/graphics.h"

#include "dune/hsq.h"

#include "common/debug.h"
#include "common/endian.h"
#include "common/memstream.h"
#include "common/substream.h"
#include "common/system.h"
#include "common/util.h"
#include "graphics/palette.h"

namespace Dune {
Graphics::Graphics() : _global_y_offset(0) {}

void Graphics::drawCursor(int x, int y, const byte *cursorData, byte *frameBuffer) {
	int hotspot_y = READ_LE_INT16(cursorData + 0);
	int hotspot_x = READ_LE_INT16(cursorData + 2);

	y = MAX(y - hotspot_y, 0);
	x = MAX(x - hotspot_x, 0);

	// DNVGA includes _global_y_offset but doesn't check for overflow?

	int w = MIN(16, 200 - x);
	int h = MIN(16, 320 - y);

	byte *dst = frameBuffer + 320 * y + x;

	for (int cy = 0; cy != h; ++cy) {
		uint16_t mask_b = READ_LE_UINT16(cursorData + 4 + 2 * cy);
		uint16_t mask_w = READ_LE_UINT16(cursorData + 4 + 2 * cy + 32);
		for (int cx = 0; cx != w; ++cx) {
			byte v = *dst;
			if (!(mask_b & 0x8000)) {
				v = 0x00;
			}
			if (mask_w & 0x8000) {
				v = 0x0f;
			}
			mask_b <<= 1;
			mask_w <<= 1;
			*dst++ = v;
		}
		dst += 320 - w;
	}
}

void Graphics::setGlobalYOffset(int y_offset) {
	assert(y_offset >= 0);
	assert(y_offset < 200);

	_global_y_offset = y_offset;
}

void Graphics::blitGraphics(byte *buf, int buf_len, byte *frameBuffer, int x_offset, int y_offset, int w, int h, byte flags, byte mode) {
	Common::MemoryReadStream r(buf, buf_len);
	blitGraphics(&r, frameBuffer, x_offset, y_offset, w, h, flags, mode);
}

void Graphics::blitGraphics(Common::ReadStream *r, byte *frameBuffer, int x_offset, int y_offset, int w, int h, byte flags, byte mode) {

	// debug("%d %d %d %d %02x %02x", x_offset, y_offset, w, h, flags, mode);

	// assert(flags == 0 || flags == 0x80);
	assert(mode == 254 || mode == 255);
	assert(x_offset >= 0);
	assert(x_offset < 320);
	assert(w <= 320);
	assert(x_offset + w <= 320);
	assert(y_offset >= 0);
	assert(y_offset < 200);
	assert(h <= 200);
	assert(y_offset + h <= 200);

	// mode 255 means that 0 is transparent.

	if (flags & 0x80) {

		// Source is compressed
		for (int y = 0; y != h; ++y) {
			byte *dst = frameBuffer + 320 * (y + y_offset) + x_offset;
			int line_remain = w;

			do {
				byte cmd = r->readByte();
				if (cmd & 0x80) {
					int count = 257 - cmd;
					byte value = r->readByte();
					if (mode == 255 && value == 0) {
						dst += count;
					} else {
						for (int i = 0; i != count; ++i) {
							*dst++ = value;
						}
					}
					line_remain -= count;
				} else {
					int count = cmd + 1;
					for (int i = 0; i != count; ++i) {
						byte value = r->readByte();
						if (mode == 255 && value == 0) {
							dst++;
						} else {
							*dst++ = value;
						}
					}
					line_remain -= count;
				}
			} while (line_remain > 0);
		}
	} else {
		for (int y = 0; y < h; ++y) {
			byte *dst = frameBuffer + 320 * (y + y_offset) + x_offset;
			if (mode == 255) {
				for (int x = 0; x != w; ++x) {
					byte b = r->readByte();
					if (b) {
						*dst = b;
					}
					++dst;
				}
			} else {
				r->read(dst, w);
			}
		}
	}
}
} // namespace Dune
