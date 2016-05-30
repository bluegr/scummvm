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

#ifndef TITANIC_BELLBOT_SCRIPT_H
#define TITANIC_BELLBOT_SCRIPT_H

#include "titanic/true_talk/tt_npc_script.h"

namespace Titanic {

class BellbotScript : public TTnpcScript {
private:
	int _array[150];
	int _field2D0;
	int _field2D4;
	int _field2D8;
	int _field2DC;
public:
	BellbotScript(int val1, const char *charClass, int v2,
		const char *charName, int v3, int val2);

	virtual int proc6(TTnpcScript *npcScript, TTsentence *sentence, uint tag);
	virtual void proc7(int v1, int v2);
	virtual int proc10() const;
	virtual int proc15() const;
	virtual bool proc16() const;
	virtual bool proc17() const;
	virtual bool proc18() const;
	virtual int proc21(int v1, int v2, int v3);
	virtual int proc22() const;
	virtual int proc23() const;
	virtual const int *getTablePtr(int id);
	virtual int proc25() const;
	virtual void proc26();
	virtual int proc36(int val) const;
	virtual uint translateId(uint id) const;
};

} // End of namespace Titanic

#endif /* TITANIC_BELLBOT_SCRIPT_H */
