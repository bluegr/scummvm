/* ScummVM - Scumm Interpreter
 * Copyright (C) 2003 The ScummVM project
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * $Header$ 
 *
 */

#ifndef SKY1COMPACT_H
#define SKY1COMPACT_H




namespace SkyCompact {

uint16 car_down_seq[] = {
	28*64,
	251,
	269,
	0,
	255,
	270,
	1,
	260,
	272,
	2,
	264,
	273,
	3,
	269,
	274,
	4,
	273,
	276,
	5,
	278,
	277,
	6,
	283,
	279,
	7,
	287,
	280,
	8,
	292,
	281,
	9,
	296,
	283,
	10,
	301,
	284,
	11,
	305,
	286,
	12,
	310,
	287,
	13,
	314,
	288,
	14,
	319,
	290,
	15,
	323,
	291,
	16,
	328,
	293,
	17,
	333,
	294,
	18,
	337,
	296,
	19,
	342,
	297,
	20,
	346,
	298,
	21,
	0
};

uint16 car_up_seq[] = {
	27*64,
	345,
	304,
	0,
	340,
	302,
	1,
	335,
	301,
	2,
	330,
	299,
	3,
	325,
	297,
	4,
	320,
	296,
	5,
	315,
	294,
	6,
	310,
	293,
	7,
	305,
	291,
	8,
	300,
	289,
	9,
	295,
	288,
	10,
	290,
	286,
	11,
	285,
	285,
	12,
	280,
	283,
	13,
	275,
	281,
	14,
	270,
	280,
	15,
	265,
	278,
	16,
	260,
	277,
	17,
	255,
	275,
	18,
	250,
	273,
	19,
	250,
	273,
	20,
	250,
	273,
	20,
	0
};

uint16 smoke1_seq[] = {
	232*64,
	235,
	137,
	0,
	235,
	137,
	0,
	235,
	137,
	0,
	235,
	137,
	1,
	235,
	137,
	1,
	235,
	137,
	1,
	235,
	137,
	1,
	235,
	137,
	2,
	235,
	137,
	2,
	235,
	137,
	2,
	235,
	137,
	2,
	235,
	137,
	3,
	235,
	137,
	3,
	235,
	137,
	3,
	235,
	137,
	3,
	235,
	137,
	4,
	235,
	137,
	4,
	235,
	137,
	4,
	235,
	137,
	4,
	235,
	137,
	5,
	235,
	137,
	5,
	235,
	137,
	5,
	235,
	137,
	5,
	235,
	137,
	6,
	235,
	137,
	6,
	235,
	137,
	6,
	235,
	137,
	6,
	235,
	137,
	7,
	235,
	137,
	7,
	235,
	137,
	7,
	235,
	137,
	7,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	9,
	235,
	137,
	9,
	235,
	137,
	9,
	235,
	137,
	9,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	10,
	235,
	137,
	10,
	235,
	137,
	10,
	235,
	137,
	10,
	235,
	137,
	11,
	235,
	137,
	11,
	235,
	137,
	11,
	235,
	137,
	11,
	235,
	137,
	12,
	235,
	137,
	12,
	235,
	137,
	12,
	235,
	137,
	12,
	235,
	137,
	13,
	235,
	137,
	13,
	235,
	137,
	13,
	235,
	137,
	13,
	235,
	137,
	14,
	235,
	137,
	14,
	235,
	137,
	14,
	235,
	137,
	14,
	235,
	137,
	15,
	235,
	137,
	15,
	235,
	137,
	15,
	235,
	137,
	15,
	235,
	137,
	16,
	235,
	137,
	16,
	235,
	137,
	16,
	235,
	137,
	16,
	235,
	137,
	17,
	235,
	137,
	17,
	235,
	137,
	17,
	235,
	137,
	17,
	235,
	137,
	18,
	235,
	137,
	18,
	235,
	137,
	18,
	235,
	137,
	18,
	235,
	137,
	19,
	235,
	137,
	19,
	235,
	137,
	19,
	235,
	137,
	19,
	235,
	137,
	20,
	235,
	137,
	20,
	235,
	137,
	20,
	235,
	137,
	20,
	235,
	137,
	21,
	235,
	137,
	21,
	235,
	137,
	21,
	235,
	137,
	21,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	22,
	235,
	137,
	21,
	235,
	137,
	21,
	235,
	137,
	21,
	235,
	137,
	21,
	235,
	137,
	20,
	235,
	137,
	20,
	235,
	137,
	20,
	235,
	137,
	20,
	235,
	137,
	19,
	235,
	137,
	19,
	235,
	137,
	19,
	235,
	137,
	19,
	235,
	137,
	18,
	235,
	137,
	18,
	235,
	137,
	18,
	235,
	137,
	18,
	235,
	137,
	17,
	235,
	137,
	17,
	235,
	137,
	17,
	235,
	137,
	17,
	235,
	137,
	16,
	235,
	137,
	16,
	235,
	137,
	16,
	235,
	137,
	16,
	235,
	137,
	15,
	235,
	137,
	15,
	235,
	137,
	15,
	235,
	137,
	15,
	235,
	137,
	14,
	235,
	137,
	14,
	235,
	137,
	14,
	235,
	137,
	14,
	235,
	137,
	13,
	235,
	137,
	13,
	235,
	137,
	13,
	235,
	137,
	13,
	235,
	137,
	12,
	235,
	137,
	12,
	235,
	137,
	12,
	235,
	137,
	12,
	235,
	137,
	11,
	235,
	137,
	11,
	235,
	137,
	11,
	235,
	137,
	11,
	235,
	137,
	10,
	235,
	137,
	10,
	235,
	137,
	10,
	235,
	137,
	10,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	9,
	235,
	137,
	9,
	235,
	137,
	9,
	235,
	137,
	9,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	8,
	235,
	137,
	7,
	235,
	137,
	7,
	235,
	137,
	7,
	235,
	137,
	7,
	235,
	137,
	6,
	235,
	137,
	6,
	235,
	137,
	6,
	235,
	137,
	6,
	235,
	137,
	5,
	235,
	137,
	5,
	235,
	137,
	5,
	235,
	137,
	5,
	235,
	137,
	4,
	235,
	137,
	4,
	235,
	137,
	4,
	235,
	137,
	4,
	235,
	137,
	3,
	235,
	137,
	3,
	235,
	137,
	3,
	235,
	137,
	3,
	235,
	137,
	2,
	235,
	137,
	2,
	235,
	137,
	2,
	235,
	137,
	2,
	235,
	137,
	1,
	235,
	137,
	1,
	235,
	137,
	1,
	235,
	137,
	1,
	235,
	137,
	0,
	235,
	137,
	0,
	0
};

uint16 screen1_logic[] = {
	ID_FOSTER,
	ID_LFAN2,
	97,
	90,
	105,
	ID_UPLOAD,
	ID_NOTICE,
	ID_NOTICE2,
	ID_LFAN1,
	ID_SMOKE1,
	ID_SMOKE2,
	ID_FIRE1,
	ID_FIRE2,
	ID_CAR_UP,
	ID_CAR_DOWN,
	0XFFFF,
	ID_MENU_LOGIC
};

uint16 fast_list_cut[] = {
	20+0X8000,
	12,
	51,
	53,
	52,
	92,
	93,
	94,
	102,
	25,
	26,
	27,
	28,
	0
};

uint16 lfan2_seq[] = {
	229*64,
	202,
	196,
	0,
	202,
	196,
	1,
	202,
	196,
	2,
	202,
	196,
	3,
	202,
	196,
	4,
	202,
	196,
	5,
	202,
	196,
	6,
	0
};

Compact smoke2 = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	0,	// xcood
	0,	// ycood
	231*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	SMOKE2_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

Compact lfan2 = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	0,	// xcood
	0,	// ycood
	229*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	LFAN2_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

uint16 lfan1_seq[] = {
	228*64,
	165,
	230,
	0,
	165,
	230,
	1,
	165,
	230,
	2,
	165,
	230,
	3,
	165,
	230,
	4,
	165,
	230,
	5,
	0
};

uint16 smoke2_seq[] = {
	231*64,
	297,
	137,
	0,
	297,
	137,
	0,
	297,
	137,
	0,
	297,
	137,
	1,
	297,
	137,
	1,
	297,
	137,
	1,
	297,
	137,
	1,
	297,
	137,
	2,
	297,
	137,
	2,
	297,
	137,
	2,
	297,
	137,
	2,
	297,
	137,
	3,
	297,
	137,
	3,
	297,
	137,
	3,
	297,
	137,
	3,
	297,
	137,
	4,
	297,
	137,
	4,
	297,
	137,
	4,
	297,
	137,
	4,
	297,
	137,
	5,
	297,
	137,
	5,
	297,
	137,
	5,
	297,
	137,
	5,
	297,
	137,
	6,
	297,
	137,
	6,
	297,
	137,
	6,
	297,
	137,
	6,
	297,
	137,
	7,
	297,
	137,
	7,
	297,
	137,
	7,
	297,
	137,
	7,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	9,
	297,
	137,
	9,
	297,
	137,
	9,
	297,
	137,
	9,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	10,
	297,
	137,
	10,
	297,
	137,
	10,
	297,
	137,
	10,
	297,
	137,
	11,
	297,
	137,
	11,
	297,
	137,
	11,
	297,
	137,
	11,
	297,
	137,
	12,
	297,
	137,
	12,
	297,
	137,
	12,
	297,
	137,
	12,
	297,
	137,
	13,
	297,
	137,
	13,
	297,
	137,
	13,
	297,
	137,
	13,
	297,
	137,
	14,
	297,
	137,
	14,
	297,
	137,
	14,
	297,
	137,
	14,
	297,
	137,
	15,
	297,
	137,
	15,
	297,
	137,
	15,
	297,
	137,
	15,
	297,
	137,
	16,
	297,
	137,
	16,
	297,
	137,
	16,
	297,
	137,
	16,
	297,
	137,
	17,
	297,
	137,
	17,
	297,
	137,
	17,
	297,
	137,
	17,
	297,
	137,
	18,
	297,
	137,
	18,
	297,
	137,
	18,
	297,
	137,
	18,
	297,
	137,
	19,
	297,
	137,
	19,
	297,
	137,
	19,
	297,
	137,
	19,
	297,
	137,
	20,
	297,
	137,
	20,
	297,
	137,
	20,
	297,
	137,
	20,
	297,
	137,
	21,
	297,
	137,
	21,
	297,
	137,
	21,
	297,
	137,
	21,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	22,
	297,
	137,
	21,
	297,
	137,
	21,
	297,
	137,
	21,
	297,
	137,
	21,
	297,
	137,
	20,
	297,
	137,
	20,
	297,
	137,
	20,
	297,
	137,
	20,
	297,
	137,
	19,
	297,
	137,
	19,
	297,
	137,
	19,
	297,
	137,
	19,
	297,
	137,
	18,
	297,
	137,
	18,
	297,
	137,
	18,
	297,
	137,
	18,
	297,
	137,
	17,
	297,
	137,
	17,
	297,
	137,
	17,
	297,
	137,
	17,
	297,
	137,
	16,
	297,
	137,
	16,
	297,
	137,
	16,
	297,
	137,
	16,
	297,
	137,
	15,
	297,
	137,
	15,
	297,
	137,
	15,
	297,
	137,
	15,
	297,
	137,
	14,
	297,
	137,
	14,
	297,
	137,
	14,
	297,
	137,
	14,
	297,
	137,
	13,
	297,
	137,
	13,
	297,
	137,
	13,
	297,
	137,
	13,
	297,
	137,
	12,
	297,
	137,
	12,
	297,
	137,
	12,
	297,
	137,
	12,
	297,
	137,
	11,
	297,
	137,
	11,
	297,
	137,
	11,
	297,
	137,
	11,
	297,
	137,
	10,
	297,
	137,
	10,
	297,
	137,
	10,
	297,
	137,
	10,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	9,
	297,
	137,
	9,
	297,
	137,
	9,
	297,
	137,
	9,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	8,
	297,
	137,
	7,
	297,
	137,
	7,
	297,
	137,
	7,
	297,
	137,
	7,
	297,
	137,
	6,
	297,
	137,
	6,
	297,
	137,
	6,
	297,
	137,
	6,
	297,
	137,
	5,
	297,
	137,
	5,
	297,
	137,
	5,
	297,
	137,
	5,
	297,
	137,
	4,
	297,
	137,
	4,
	297,
	137,
	4,
	297,
	137,
	4,
	297,
	137,
	3,
	297,
	137,
	3,
	297,
	137,
	3,
	297,
	137,
	3,
	297,
	137,
	2,
	297,
	137,
	2,
	297,
	137,
	2,
	297,
	137,
	2,
	297,
	137,
	1,
	297,
	137,
	1,
	297,
	137,
	1,
	297,
	137,
	1,
	297,
	137,
	0,
	297,
	137,
	0,
	0
};

Compact car_down = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC+ST_RECREATE,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	20,	// xcood
	20,	// ycood
	28*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	CAR_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

uint16 fire2_seq[] = {
	26*64,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	1,
	302,
	136,
	2,
	302,
	136,
	3,
	302,
	136,
	4,
	302,
	136,
	5,
	302,
	136,
	6,
	302,
	136,
	7,
	302,
	136,
	8,
	302,
	136,
	9,
	302,
	136,
	10,
	302,
	136,
	0,
	302,
	136,
	0,
	302,
	136,
	0,
	0
};

Compact smoke1 = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	0,	// xcood
	0,	// ycood
	232*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	SMOKE1_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

uint16 fast_list_sc1[] = {
	12,
	51,
	52,
	53,
	13,
	92,
	93,
	94,
	25,
	26,
	27,
	28,
	0
};

Compact fire_notice2 = {
	L_SCRIPT,	// logic
	ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	202,	// xcood
	192,	// ycood
	0,	// frame
	4151,	// cursorText
	STD_ON,	// mouseOn
	STD_OFF,	// mouseOff
	ADVISOR_188,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	13,	// mouseSize_x
	13,	// mouseSize_y
	NOTICE2_ACTION,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	NOTICE_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

Compact car_up = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC+ST_RECREATE,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	20,	// xcood
	20,	// ycood
	27*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	CAR_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

Compact fire1 = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	20,	// xcood
	20,	// ycood
	25*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	FIRE1_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

Compact ss_sign = {
	0,	// logic
	ST_MOUSE,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	376,	// xcood
	143,	// ycood
	0,	// frame
	4176,	// cursorText
	STD_ON,	// mouseOn
	STD_OFF,	// mouseOff
	ADVISOR_188,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	25,	// mouseSize_x
	77,	// mouseSize_y
	SS_SIGN_ACTION,	// actionScript
	0,
	0,
	0,
	0,
	0,
	{ 0, 0, 0 },
	0,
	0,
	0,
	0,
	0
};

Compact lfan1 = {
	L_SCRIPT,	// logic
	ST_FOREGROUND+ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	0,	// xcood
	0,	// ycood
	228*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	LFAN1_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

uint16 chip_list_1[] = {
	21,
	98,
	226,
	228,
	229,
	232,
	231,
	99,
	96,
	0
};

uint16 sc1_mouse[] = {
	95,
	ID_NOTICE2,
	97,
	ID_SS_SIGN,
	18,
	0XFFFF,
	ID_TEXT_MOUSE
};

Compact fire2 = {
	L_SCRIPT,	// logic
	ST_BACKGROUND+ST_LOGIC,	// status
	0,	// sync
	1,	// screen
	0,	// place
	0,	// getToTable
	20,	// xcood
	20,	// ycood
	26*64,	// frame
	0,	// cursorText
	0,	// mouseOn
	0,	// mouseOff
	0,	// mouseClick
	0,	// mouseRel_x
	0,	// mouseRel_y
	0,	// mouseSize_x
	0,	// mouseSize_y
	0,	// actionScript
	0,	// upFlag
	0,	// downFlag
	0,	// getToFlag
	0,	// flag
	0,	// mood
	{ 0, 0, 0 },
	0,	// offset
	C_BASE_MODE,	// mode
	FIRE2_LOGIC,	// baseSub
	0,	// baseSub_off
	0
};

uint16 fire1_seq[] = {
	25*64,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	0,
	256,
	136,
	1,
	256,
	136,
	2,
	256,
	136,
	3,
	256,
	136,
	4,
	256,
	136,
	5,
	256,
	136,
	6,
	256,
	136,
	7,
	256,
	136,
	8,
	256,
	136,
	9,
	256,
	136,
	10,
	256,
	136,
	11,
	256,
	136,
	12,
	256,
	136,
	12,
	256,
	136,
	12,
	256,
	136,
	12,
	256,
	136,
	13,
	0
};

}; // namespace SkyCompact

#endif
