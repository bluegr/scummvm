#ifndef INNOCENT_INTER_H
#define INNOCENT_INTER_H

#include <memory>

#include "common/list.h"
#include "common/rect.h"
#include "common/util.h"
#include "config.h"

#include "innocent/value.h"

namespace Innocent {

class Logic;
class Opcode;
class Engine;
class Resources;
class Graphics;

#define UNIMPLEMENTED { error("type conversion unimplemented"); }

enum Status {
	kReturned = 0,
	kInvalidOpcode = 1
};

class Interpreter {
public:
	Interpreter(Logic *l, byte *base, const char *name);

	void init();

	/**
	 * Run bytecode.
	 * @param code a Common::ReadStream pointing to code. The interpreter takes ownership of it.
	 * @param mode interpreting mode.
	 */
	Status run(uint16 offset, OpcodeMode mode);
	void tick();
	void executeRestricted(byte *code);

	Value *getArgument(byte *&code);

	friend class Opcode;

	template <int opcode>
	void opcodeHandler(ValueVector args);

	template <int N>
	void init_opcodes();

	typedef void (Interpreter::*OpcodeHandler)(ValueVector args);
	OpcodeHandler _handlers[256];
	static const uint8 _argumentsCounts[];

	Logic *_logic;

	const char *name() const { return _name; }

	byte *rawCode(uint16 offset) const { return _base + offset; }

	friend class CodePointer;

	Resources *resources() const { return _resources; }

private:
	char _name[100];
	template<class T>
	T *readArgument(byte *&code);

	byte *_base;
	byte *_code;
	uint16 _mode;

	Status run(uint16 offset);

	void failedCondition();
	void endIf();
	void goBack();
	void setRoomLoop(byte *code);

	uint16 _failedCondition;
	bool _return;
	byte *_roomLoop;

	Engine *_engine;
	Resources *_resources;
	Graphics *_graphics;
};

} // End of namespace Innocent

#endif // INNOCENT_INTER_H
