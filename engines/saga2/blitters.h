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

#ifndef SAGA2_BLITTERS_H
#define SAGA2_BLITTERS_H

#include "saga2/gdraw.h"

namespace Saga2 {

// VWPAGE.CPP , GDRAW.CPP

//  Assembly-language function to copy pixel to SVGA (opaque)

void _BltPixels(uint8 *srcPtr, uint32 srcMod,
                                 uint8 *dstPtr, uint32 dstMod,
                                 uint32 width, uint32 height);

//  Assembly-language function to copy pixel to SVGA (transparent)

void _BltPixelsT(uint8 *srcPtr, uint32 srcMod,
                                  uint8 *dstPtr, uint32 dstMod,
                                  uint32 width, uint32 height);

//  Assembly-language function to do rectangle fill (opaque)

void _FillRect(uint8 *dstPtr, uint32 dstMod,
                                uint32 width, uint32 height, uint32 color);

//  Assembly-language function to draw horizontal line (opaque)

void _HLine(uint8 *dstPtr, uint32 width, uint32 color);


/* ===================================================================== *
                    RUNLEN.ASM
 * ===================================================================== */

// INTRFACE.CPP, PLAYMODE.CPP

void unpackImage(gPixelMap *map,
                                  int32 width,
                                  int32 rowCount,
                                  int8 *srcData);

void unpackImage(gPixelMap &map,
                                  int16 width,
                                  int16 rowCount,
                                  int8 *srcData);
/* ===================================================================== *
                    SPRDRAW.ASM
 * ===================================================================== */

// SPRITE.CPP

void unpackSprite(gPixelMap *map, uint8 *sprData);
void compositePixels(
    gPixelMap       *compMap,
    gPixelMap       *sprMap,
    int32           xpos,
    int32           ypos,
    uint8           *lookup);
void compositePixelsRvs(
    gPixelMap       *compMap,
    gPixelMap       *sprMap,
    int32           xpos,
    int32           ypos,
    uint8           *lookup);

// FTA.H, AUTOMAP.CPP, DISPNODE.CPP, INTRFACE.CPP, MOUSEIMG.CPP, SPRITE.CPP
// TILE.CPP, ITEVIDEO.CPP

//  Fast transparent blitting routine in assembly

void TBlit(gPixelMap *d, gPixelMap *s, int32 x, int32 y);
void TBlit4(gPixelMap *d, gPixelMap *s, int32 x, int32 y);

/* ===================================================================== *
                    TILEDRAW.ASM
 * ===================================================================== */

// TILE.H, TILELOAD.CPP

void drawTile(gPixelMap *map,
                               int32 x, int32 y, int32 height,
                               uint8 *srcData);

void maskTile(gPixelMap *map,
                               int32 x, int32 y, int32 height,
                               uint8 *srcData);

} // end of namespace Saga2

#endif
