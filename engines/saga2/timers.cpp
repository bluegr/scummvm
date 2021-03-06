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

#define FORBIDDEN_SYMBOL_ALLOW_ALL // FIXME: Remove

#include "common/timer.h"

#include "saga2/saga2.h"
#include "saga2/fta.h"
#include "saga2/timers.h"
#include "saga2/objects.h"
#include "saga2/savefile.h"

namespace Saga2 {

volatile int32 gameTime;
bool timerPaused = false;

void timerCallback(void *refCon) {
	if (!timerPaused)
		gameTime++;
}

void initTimer(void) {
	gameTime = 0;

	g_vm->getTimerManager()->installTimerProc(&timerCallback, 1000000 / 72, nullptr, "saga2");
}

void pauseTimer() {
	timerPaused = true;
}

void resumeTimer() {
	timerPaused = false;
}

void saveTimer(SaveFileConstructor &saveGame) {
	int32   time = gameTime;

	saveGame.writeChunk(
	    MakeID('T', 'I', 'M', 'E'),
	    &time,
	    sizeof(time));
}

void saveTimer(Common::OutSaveFile *out) {
	debugC(2, kDebugSaveload, "Saving time");

	int32 time = gameTime;

	out->write("TIME", 4);
	out->writeUint32LE(sizeof(time));
	out->writeUint32LE(time);

	debugC(3, kDebugSaveload, "... time = %d", time);
}

void loadTimer(SaveFileReader &saveGame) {
	int32   time;

	saveGame.read(&time, sizeof(time));
	gameTime = time;
}

void loadTimer(Common::InSaveFile *in) {
	debugC(2, kDebugSaveload, "Loading time");

	gameTime = in->readUint32LE();

	debugC(3, kDebugSaveload, "... time = %d", gameTime);
}

/* ====================================================================== *
   Alarms
 * ====================================================================== */

void Alarm::write(Common::OutSaveFile *out) {
	out->writeUint32LE(basetime);
	out->writeUint32LE(duration);
}

void Alarm::read(Common::InSaveFile *in) {
	basetime = in->readUint32LE();
	duration = in->readUint32LE();
}

void Alarm::set(uint32 dur) {
	basetime = gameTime;
	duration = dur;
}

bool Alarm::check(void) {
	return ((uint32)(gameTime - basetime) > duration);
}

// time elapsed since alarm set

uint32 Alarm::elapsed(void) {
	return (uint32)(gameTime - basetime);
}

/* ===================================================================== *
   TimerList management functions
 * ===================================================================== */

//----------------------------------------------------------------------
//	Fetch a specified object's TimerList

TimerList *fetchTimerList(GameObject *obj) {
	for (Common::List<TimerList *>::iterator it = g_vm->_timerLists.begin(); it != g_vm->_timerLists.end(); ++it)
		if ((*it)->getObject() == obj)
			return *it;

	return nullptr;
}

void deleteTimer(Timer *t) {
	g_vm->_timers.remove(t);
}

//----------------------------------------------------------------------
//	Check all active Timers
void checkTimers(void) {
	for (Common::List<Timer *>::iterator it = g_vm->_timers.begin(); it != g_vm->_timers.end(); it++) {
		if ((*it)->_active == false)
			continue;

		if ((*it)->check()) {
			debugC(2, kDebugTimers, "Timer tick for %p (%s): %p (duration %d)", (void *)(*it)->getObject(), (*it)->getObject()->objName(), (void *)(*it), (*it)->getInterval());
			(*it)->reset();
			(*it)->getObject()->timerTick((*it)->thisID());
		}
	}

	for (Common::List<Timer *>::iterator it = g_vm->_timers.begin(); it != g_vm->_timers.end(); it++) {
		if ((*it)->_active == false) {
			delete *it;
			it = g_vm->_timers.erase(it);
		}
	}
}

//----------------------------------------------------------------------
//	Initialize the Timers

void initTimers(void) {
	//  Nothing to do
}

//----------------------------------------------------------------------
//	Save the active Timers in a save file

void saveTimers(SaveFileConstructor &saveGame) {
	warning("STUB: saveTimers");

#if 0
	int16                   timerListCount = 0,
	                        timerCount = 0;

	TimerListHolder     *listHolder;
	TimerHolder         *timerHolder;

	void                *archiveBuffer,
	                    *bufferPtr;
	int32               archiveBufSize = 0;

	//  Add the sizes of the timer list count an timer count
	archiveBufSize += sizeof(timerListCount) + sizeof(timerCount);

	//  Tally the timer lists
	for (listHolder = (TimerListHolder *)timerListList.first(); listHolder != NULL; listHolder = (TimerListHolder *)listHolder->next())
		timerListCount++;

	//  Add the total archive size of all of the timer lists
	archiveBufSize += timerListCount * TimerList::archiveSize();

	//  Tally the timers
	for (timerHolder = (TimerHolder *)timerList.first(); timerHolder != NULL; timerHolder = (TimerHolder *)timerHolder->next())
		timerCount++;

	//  Add the total archive size of all of the timers
	archiveBufSize += timerCount * Timer::archiveSize();

	//  Allocate an archive buffer
	archiveBuffer = RNewPtr(archiveBufSize, NULL, "archive buffer");
	if (archiveBuffer == NULL)
		error("Unable to allocate timer archive buffer");

	bufferPtr = archiveBuffer;

	//  Store the timer list count and timer count
	*((int16 *)bufferPtr)      = timerListCount;
	*((int16 *)bufferPtr + 1)  = timerCount;
	bufferPtr = (int16 *)bufferPtr + 2;

	//  Archive all timer lists
	for (listHolder = (TimerListHolder *)timerListList.first();
	        listHolder != NULL;
	        listHolder = (TimerListHolder *)listHolder->next())
		bufferPtr = listHolder->getTimerList()->archive(bufferPtr);

	//  Archive all timers
	for (timerHolder = (TimerHolder *)timerList.first();
	        timerHolder != NULL;
	        timerHolder = (TimerHolder *)timerHolder->next())
		bufferPtr = timerHolder->getTimer()->archive(bufferPtr);

	assert(bufferPtr == &((uint8 *)archiveBuffer)[archiveBufSize]);

	//  Write the data to the save file
	saveGame.writeChunk(
	    MakeID('T', 'I', 'M', 'R'),
	    archiveBuffer,
	    archiveBufSize);

	RDisposePtr(archiveBuffer);
#endif
}

static int getTimerListID(TimerList *t) {
	int i = 0;
	for (Common::List<TimerList *>::iterator it = g_vm->_timerLists.begin(); it != g_vm->_timerLists.end(); it++, i++) {
		if ((*it) == t)
			return i;
	}
	return -1;
}

static int getTimerID(Timer *t) {
	int i = 0;
	for (Common::List<Timer *>::iterator it = g_vm->_timers.begin(); it != g_vm->_timers.end(); it++, i++) {
		if ((*it) == t)
			return i;
	}
	return -1;
}

void saveTimers(Common::OutSaveFile *out) {
	debugC(2, kDebugSaveload, "Saving Timers");

	int16 timerListCount = 0,
	      timerCount = 0;

	int32 archiveBufSize = 0;

	//  Add the sizes of the timer list count an timer count
	archiveBufSize += sizeof(timerListCount) + sizeof(timerCount);

	//  Tally the timer lists
	timerListCount = g_vm->_timerLists.size();

	//  Add the total archive size of all of the timer lists
	archiveBufSize += timerListCount * TimerList::archiveSize();

	//  Tally the timers
	timerCount = g_vm->_timers.size();

	debugC(3, kDebugSaveload, "... timerListCount = %d", timerListCount);
	debugC(3, kDebugSaveload, "... timerCount = %d", timerCount);

	//  Add the total archive size of all of the timers
	archiveBufSize += timerCount * Timer::archiveSize();

	out->write("TIMR", 4);
	out->writeUint32LE(archiveBufSize);

	//  Store the timer list count and timer count
	out->writeSint16LE(timerListCount);
	out->writeSint16LE(timerCount);

	//  Archive all timer lists
	for (Common::List<TimerList *>::iterator it = g_vm->_timerLists.begin(); it != g_vm->_timerLists.end(); it++) {
		debugC(3, kDebugSaveload, "Saving TimerList %d", getTimerListID(*it));
		(*it)->write(out);
	}

	for (Common::List<Timer *>::iterator it = g_vm->_timers.begin(); it != g_vm->_timers.end(); it++) {
		if ((*it)->_active == false)
			continue;
		debugC(3, kDebugSaveload, "Saving Timer %d", getTimerID(*it));

		(*it)->write(out);
	}
}

//----------------------------------------------------------------------
//	Load the Timers from a save file

void loadTimers(SaveFileReader &saveGame) {
	warning("STUB: loadTimers");

#if 0
	int16       i,
	            timerListCount,
	            timerCount;

	void        *archiveBuffer,
	            *bufferPtr;

	//  Allocate a buffer in which to read the archive data
	archiveBuffer = RNewPtr(saveGame.getChunkSize(), NULL, "archive buffer");
	if (archiveBuffer == NULL)
		error("Unable to allocate timer archive buffer");

	//  Read the data
	saveGame.read(archiveBuffer, saveGame.getChunkSize());

	bufferPtr = archiveBuffer;

	//  Get the timer list count and timer count
	timerListCount  = *((int16 *)bufferPtr);
	timerCount      = *((int16 *)bufferPtr + 1);
	bufferPtr = (int16 *)bufferPtr + 2;

	//  Restore all timer lists
	for (i = 0; i < timerListCount; i++)
		new TimerList(&bufferPtr);

	//  Restore all timers
	for (i = 0; i < timerCount; i++) {
		Timer       *timer;
		TimerList   *timerList;

		timer = new Timer(&bufferPtr);

		assert(timer != NULL);

		//  Get the objects's timer list
		timerList = fetchTimerList(timer->getObject());

		assert(timerList != NULL);

		//  Append this timer to the objects's timer list
		timerList->addTail(*timer);
	}

	assert(bufferPtr == &((uint8 *)archiveBuffer)[saveGame.getChunkSize()]);

	RDisposePtr(archiveBuffer);
#endif
}

void loadTimers(Common::InSaveFile *in) {
	debugC(2, kDebugSaveload, "Loading Timers");

	int16 timerListCount,
	      timerCount;

	//  Get the timer list count and timer count
	timerListCount = in->readSint16LE();
	timerCount = in->readSint16LE();

	debugC(3, kDebugSaveload, "... timerListCount = %d", timerListCount);
	debugC(3, kDebugSaveload, "... timerCount = %d", timerCount);

	//  Restore all timer lists
	for (int i = 0; i < timerListCount; i++) {
		debugC(3, kDebugSaveload, "Loading TimerList %d", i);
		new TimerList(in);
	}

	//  Restore all timers
	for (int i = 0; i < timerCount; i++) {
		Timer       *timer;
		TimerList   *timerList;

		debugC(3, kDebugSaveload, "Loading Timer %d", i);

		timer = new Timer(in);

		assert(timer != NULL);

		//  Get the objects's timer list
		timerList = fetchTimerList(timer->getObject());

		assert(timerList != NULL);

		//  Append this timer to the objects's timer list
		timerList->_timers.push_back(timer);
	}
}

//----------------------------------------------------------------------
//	Cleanup the active Timers

void cleanupTimers(void) {
	while (g_vm->_timerLists.size() > 0)
		delete g_vm->_timerLists.front();

	while (g_vm->_timers.size() > 0) {
		Timer *t = g_vm->_timers.front();
		deleteTimer(t);
		delete t;
	}
}

/* ===================================================================== *
   TimerList member functions
 * ===================================================================== */

TimerList::TimerList(GameObject *o) : _obj(o) {
	debugC(1, kDebugTimers, "Creating timer list %p for %p (%s)",
		   (void *)this, (void *)o, o->objName());
	g_vm->_timerLists.push_back(this);
}

TimerList::TimerList(void **buf) {
	ObjectID        *bufferPtr = (ObjectID *)*buf;

	warning("STUB: TimerList::TimerList(buf)");

	assert(isObject(*bufferPtr) || isActor(*bufferPtr));

	//  Restore the object pointer
	_obj = GameObject::objectAddress(*bufferPtr++);

	*buf = bufferPtr;

	g_vm->_timerLists.push_back(this);
}

TimerList::TimerList(Common::InSaveFile *in) {
	ObjectID id = in->readUint16LE();

	assert(isObject(id) || isActor(id));

	//  Restore the object pointer
	_obj = GameObject::objectAddress(id);

	g_vm->_timerLists.push_back(this);
}

TimerList::~TimerList() {
	debugC(1, kDebugTimers, "Deleting timer list %p for %p (%s))",
		   (void *)this, (void *)_obj, _obj->objName());
	g_vm->_timerLists.remove(this);
}

//----------------------------------------------------------------------
//	Archive this object in a buffer

void *TimerList::archive(void *buf) {
	//  Store the object's ID
	*((ObjectID *)buf) = _obj->thisID();
	buf = (ObjectID *)buf + 1;

	return buf;
}

void TimerList::write(Common::OutSaveFile *out) {
	//  Store the object's ID
	out->writeUint16LE(_obj->thisID());
}

/* ===================================================================== *
   Timer member functions
 * ===================================================================== */

//----------------------------------------------------------------------
//	Constructor -- reconstruct from an archive buffer

Timer::Timer(void **buf) {
	void  *bufferPtr = *buf;

	warning("STUB: Timer::Timer(buf)");

	assert(isObject(*((ObjectID *)bufferPtr)) || isActor(*((ObjectID *)bufferPtr)));

	//  Restore the object pointer
	_obj = GameObject::objectAddress(*((ObjectID *)bufferPtr));
	bufferPtr = (ObjectID *)bufferPtr + 1;

	//  Restore the timer's ID
	_id = *((TimerID *)bufferPtr);
	bufferPtr = (TimerID *)bufferPtr + 1;

	//  Restore the frame interval
	_interval = *((int16 *)bufferPtr);
	bufferPtr = (int16 *)bufferPtr + 1;

	//  Restore the alarm
	memcpy(&_alarm, bufferPtr, sizeof(Alarm));
	bufferPtr = (FrameAlarm *)bufferPtr + 1;

	*buf = bufferPtr;

	g_vm->_timers.push_back(this);
}

Timer::Timer(Common::InSaveFile *in) {
	ObjectID id = in->readUint16LE();

	assert(isObject(id) || isActor(id));

	//  Restore the object pointer
	_obj = GameObject::objectAddress(id);

	//  Restore the timer's ID
	_id = in->readSint16LE();

	//  Restore the frame interval
	_interval = in->readSint16LE();

	//  Restore the alarm
	_alarm.read(in);

	g_vm->_timers.push_back(this);
}

Timer::~Timer() {
	debugC(1, kDebugTimers, "Deleting timer %p (obj %p)",
		   (void *)this, (void *)_obj);
}

//----------------------------------------------------------------------
//	Return the number of bytes needed to archive this object in a buffer

int32 Timer::archiveSize(void) {
	return      sizeof(ObjectID)         //  obj's ID
	            +   sizeof(TimerID)
	            +   sizeof(int16)
	            +   sizeof(FrameAlarm);
}

//----------------------------------------------------------------------
//	Archive this object in a buffer

void *Timer::archive(void *buf) {
	//  Store the obj's ID
	*((ObjectID *)buf) = _obj->thisID();
	buf = (ObjectID *)buf + 1;

	//  Store the timer's ID
	*((TimerID *)buf) = _id;
	buf = (TimerID *)buf + 1;

	//  Store the frame interval
	*((int16 *)buf) = _interval;
	buf = (int16 *)buf + 1;

	//  Store the alarm
	memcpy(buf, &_alarm, sizeof(Alarm));
	buf = (FrameAlarm *)buf + 1;

	return buf;
}

void Timer::write(Common::OutSaveFile *out) {
	//  Store the obj's ID
	out->writeUint16LE(_obj->thisID());

	//  Store the timer's ID
	out->writeSint16LE(_id);

	//  Store the frame interval
	out->writeSint16LE(_interval);

	//  Store the alarm
	_alarm.write(out);
}

} // end of namespace Saga2
