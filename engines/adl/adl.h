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

#ifndef ADL_ADL_H
#define ADL_ADL_H

#include "common/random.h"
#include "common/rect.h"

#include "engines/engine.h"

#include "gui/debugger.h"

namespace Common {
class ReadStream;
class SeekableReadStream;
}

namespace Adl {

#define SAVEGAME_VERSION 0
#define SAVEGAME_NAME_LEN 32

class Display;
class Parser;
class Console;
struct AdlGameDescription;

enum GameType {
	kGameTypeNone = 0,
	kGameTypeHires1
};

struct StringOffset {
	int stringIdx;
	uint offset;
};

// Messages used outside of scripts
enum EngineMessage {
	IDI_MSG_CANT_GO_THERE,
	IDI_MSG_DONT_UNDERSTAND,
	IDI_MSG_ITEM_DOESNT_MOVE,
	IDI_MSG_ITEM_NOT_HERE,
	IDI_MSG_THANKS_FOR_PLAYING
};

// Strings embedded in the executable
enum EngineString {
	IDI_STR_ENTER_COMMAND,
	IDI_STR_VERB_ERROR,
	IDI_STR_NOUN_ERROR,
	IDI_STR_PLAY_AGAIN,

	IDI_STR_TOTAL
};

// Conditional opcodes
#define IDO_CND_ITEM_IN_ROOM   0x03
#define IDO_CND_MOVES_GE       0x05
#define IDO_CND_VAR_EQ         0x06
#define IDO_CND_CUR_PIC_EQ     0x09
#define IDO_CND_ITEM_PIC_EQ    0x0a

// Action opcodes
#define IDO_ACT_VAR_ADD        0x01
#define IDO_ACT_VAR_SUB        0x02
#define IDO_ACT_VAR_SET        0x03
#define IDO_ACT_LIST_ITEMS     0x04
#define IDO_ACT_MOVE_ITEM      0x05
#define IDO_ACT_SET_ROOM       0x06
#define IDO_ACT_SET_CUR_PIC    0x07
#define IDO_ACT_SET_PIC        0x08
#define IDO_ACT_PRINT_MSG      0x09
#define IDO_ACT_SET_LIGHT      0x0a
#define IDO_ACT_SET_DARK       0x0b
#define IDO_ACT_QUIT           0x0d
#define IDO_ACT_SAVE           0x0f
#define IDO_ACT_LOAD           0x10
#define IDO_ACT_RESTART        0x11
#define IDO_ACT_PLACE_ITEM     0x12
#define IDO_ACT_SET_ITEM_PIC   0x13
#define IDO_ACT_RESET_PIC      0x14
#define IDO_ACT_GO_NORTH       0x15
#define IDO_ACT_GO_SOUTH       0x16
#define IDO_ACT_GO_EAST        0x17
#define IDO_ACT_GO_WEST        0x18
#define IDO_ACT_GO_UP          0x19
#define IDO_ACT_GO_DOWN        0x1a
#define IDO_ACT_TAKE_ITEM      0x1b
#define IDO_ACT_DROP_ITEM      0x1c
#define IDO_ACT_SET_ROOM_PIC   0x1d

struct Room {
	byte description;
	byte connections[6];
	byte picture;
	byte curPicture;
};

struct Picture {
	byte block;
	uint16 offset;
};

struct Command {
	byte room;
	byte verb, noun;
	byte numCond, numAct;
	Common::Array<byte> script;
};

enum {
	IDI_ITEM_NOT_MOVED,
	IDI_ITEM_MOVED,
	IDI_ITEM_DOESNT_MOVE
};

#define IDI_NONE 0xfe

struct Item {
	byte noun;
	byte room;
	byte picture;
	bool isLineArt;
	Common::Point position;
	int state;
	byte description;
	Common::Array<byte> roomPictures;
};

struct State {
	Common::Array<Room> rooms;
	Common::Array<Item> items;
	Common::Array<byte> vars;

	byte room;
	uint16 moves;
	bool isDark;

	State() : room(1), moves(0), isDark(false) { }
};

typedef Common::List<Command> Commands;
typedef Common::HashMap<Common::String, uint> WordMap;

class AdlEngine : public Engine {
public:
	virtual ~AdlEngine();

	static AdlEngine *create(GameType type, OSystem *syst, const AdlGameDescription *gd);

protected:
	AdlEngine(OSystem *syst, const AdlGameDescription *gd);

	Common::String readString(Common::ReadStream &stream, byte until = 0) const;
	void printStrings(Common::SeekableReadStream &stream, int count = 1) const;
	void printMessage(uint idx, bool wait = true) const;
	void printASCIIString(const Common::String &str) const;
	void loadVerbs(Common::ReadStream &stream) { loadWords(stream, _verbs); }
	void loadNouns(Common::ReadStream &stream) { loadWords(stream, _nouns); }
	void readCommands(Common::ReadStream &stream, Commands &commands);
	Common::String inputString(byte prompt = 0) const;
	void delay(uint32 ms) const;
	byte inputKey() const;

	Display *_display;
	Parser *_parser;

	Common::Array<Common::String> _strings;
	Common::Array<Common::String> _messages;
	Common::Array<Picture> _pictures;
	Common::Array<Common::Point> _itemOffsets;
	Common::Array<Common::Array<byte> > _lineArt;
	Commands _roomCommands;
	Commands _globalCommands;

	// Game state
	State _state;

private:
	virtual void runIntro() { }
	virtual void loadData() = 0;
	virtual void initState() = 0;
	virtual void restartGame() = 0;
	virtual uint getEngineMessage(EngineMessage msg) const = 0;
	virtual void drawPic(byte pic, Common::Point pos = Common::Point()) const = 0;

	// Engine
	Common::Error run();
	bool hasFeature(EngineFeature f) const;
	Common::Error loadGameState(int slot);
	bool canLoadGameStateCurrently() const;
	Common::Error saveGameState(int slot, const Common::String &desc);
	bool canSaveGameStateCurrently() const;

	// Text output
	Common::String getEngineString(int str) const;
	void printEngineMessage(EngineMessage) const;
	void wordWrap(Common::String &str) const;

	// Text input
	void loadWords(Common::ReadStream &stream, WordMap &map);
	byte convertKey(uint16 ascii) const;
	Common::String getLine() const;
	Common::String getWord(const Common::String &line, uint &index) const;
	void getInput(uint &verb, uint &noun);

	// Graphics
	void showRoom() const;
	void clearScreen() const;
	void drawItems() const;
	void drawNextPixel(Common::Point &p, byte color, byte bits, byte quadrant) const;
	void drawLineArt(const Common::Array<byte> &lineArt, Common::Point p, byte rotation = 0, byte scaling = 1, byte color = 0x7f) const;

	// Game state functions
	const Room &room(uint i) const;
	Room &room(uint i);
	const Room &curRoom() const;
	Room &curRoom();
	const Item &item(uint i) const;
	Item &item(uint i);
	const byte &var(uint i) const;
	byte &var(uint i);
	void takeItem(byte noun);
	void dropItem(byte noun);
	bool matchCommand(const Command &command, byte verb, byte noun, uint *actions = nullptr) const;
	bool doOneCommand(const Commands &commands, byte verb, byte noun);
	void doAllCommands(const Commands &commands, byte verb, byte noun);
	void doActions(const Command &command, byte noun, byte offset);

	enum {
		kWordSize = 8
	};

	const AdlGameDescription *_gameDescription;
	bool _isRestarting, _isRestoring;
	byte _saveVerb, _saveNoun, _restoreVerb, _restoreNoun;
	bool _canSaveNow, _canRestoreNow;
	WordMap _verbs;
	WordMap _nouns;
};

AdlEngine *HiRes1Engine__create(OSystem *syst, const AdlGameDescription *gd);

} // End of namespace Adl

#endif
