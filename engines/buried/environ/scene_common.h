/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * Additional copyright for this file:
 * Copyright (C) 1995 Presto Studios, Inc.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef BURIED_SCENE_COMMON_H
#define BURIED_SCENE_COMMON_H

#include "buried/bookdata.h"
#include "buried/environ/scene_base.h"

namespace Buried {

class BasicDoor : public SceneBase {
public:
	BasicDoor(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int left = -1, int top = -1, int right = -1, int bottom = -1, int timeZone = -1, int environment = -1,
			int node = -1, int facing = -1, int orientation = -1, int depth = -1, int transitionType = -1, int transitionData = -1,
			int transitionStartFrame = -1, int transitionLength = -1, int openingSoundID = -1);

	int mouseDown(Window *viewWindow, const Common::Point &pointLocation);
	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

	bool _clicked;
	Common::Rect _clickable;
	DestinationScene _destData;
	int _openingSoundID;
};

class TurnDepthPreChange : public SceneBase {
public:
	TurnDepthPreChange(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int flagOffset = -1, int upDepth = -1, int leftDepth = -1, int rightDepth = -1, int downDepth = -1, int forwardDepth = -1);
};

class GenericItemAcquire : public SceneBase {
public:
	GenericItemAcquire(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int left = 0, int top = 0, int right = 0, int bottom = 0, int itemID = 0, int clearStillFrame = 0, int itemFlagOffset = 0);
	int mouseDown(Window *viewWindow, const Common::Point &pointLocation);
	int droppedItem(Window *viewWindow, int itemID, const Common::Point &pointLocation, int itemFlags);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	bool _itemPresent;
	Common::Rect _acquireRegion;
	int _fullFrameIndex;
	int _clearFrameIndex;
	int _itemID;
	int _itemFlagOffset;
};

class PlaySoundExitingFromScene : public SceneBase {
public:
	PlaySoundExitingFromScene(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int soundFileNameID = -1);
	int postExitRoom(Window *viewWindow, const Location &newLocation);

private:
	int _soundFileNameID;
};

class PlaySoundExitingFromSceneDeux : public SceneBase {
public:
	PlaySoundExitingFromSceneDeux(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int soundFileNameID = -1);
	int postExitRoom(Window *viewWindow, const Location &newLocation);

private:
	int _soundFileNameID;
};

class ClickChangeScene : public SceneBase {
public:
	ClickChangeScene(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int left = -1, int top = -1, int right = -1, int bottom = -1, int cursorID = 0,
			int timeZone = -1, int environment = -1, int node = -1, int facing = -1, int orientation = -1, int depth = -1,
			int transitionType = -1, int transitionData = -1, int transitionStartFrame = -1, int transitionLength = -1);
	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	int _cursorID;
	Common::Rect _clickRegion;
	DestinationScene _clickDestination;
};

class ClickChangeSceneSetFlag : public ClickChangeScene {
public:
	ClickChangeSceneSetFlag(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int left = -1, int top = -1, int right = -1, int bottom = -1, int cursorID = 0,
			int timeZone = -1, int environment = -1, int node = -1, int facing = -1, int orientation = -1, int depth = -1,
			int transitionType = -1, int transitionData = -1, int transitionStartFrame = -1, int transitionLength = -1, int flagIndex = -1);
};

class PlayStingers : public SceneBase {
public:
	PlayStingers(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int stingerVolume = 127, int lastStingerFlagOffset = -1, int effectIDFlagOffset = -1, int firstStingerFileID = -1, int lastStingerFileID = -1);
	int postEnterRoom(Window *viewWindow, const Location &priorLocation);

private:
	int _stingerVolume;
	int _lastStingerFlagOffset;
	int _effectIDFlagOffset;
	int _firstStingerFileID;
	int _lastStingerFileID;
};

class ClickPlaySound : public SceneBase {
public:
	ClickPlaySound(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int flagOffset = -1, int soundID = 0, int cursorID = 0, int left = 0, int top = 0, int right = 0, int bottom = 0);
	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	int _cursorID;
	int _soundID;
	Common::Rect _clickRegion;
	int _flagOffset;
};

class PlaySoundEnteringFromScene : public SceneBase {
public:
	PlaySoundEnteringFromScene(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int soundFileNameID = -1, int timeZone = -1, int environment = -1, int node = -1, int facing = -1, int orientation = -1, int depth = -1);
	int postEnterRoom(Window *viewWindow, const Location &priorLocation);

private:
	Location _soundLocation;
	int _soundFileNameID;
};

class SetFlagOnEntry : public SceneBase {
public:
	SetFlagOnEntry(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int flagOffset = -1, byte flagNewValue = 1);
};

class DisplayMessageWithEvidenceWhenEnteringNode : public SceneBase {
public:
	DisplayMessageWithEvidenceWhenEnteringNode(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int evidenceID = -1, int messageBoxTextID = -1);
	int postEnterRoom(Window *viewWindow, const Location &priorLocation);

private:
	int _messageBoxTextID;
	byte _evidenceID;
};

class OneShotEntryVideoWarning : public SceneBase {
public:
	OneShotEntryVideoWarning(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int animID = 0, int flagOffset = 0, int warningMessageID = -1);
	int postEnterRoom(Window *viewWindow, const Location &priorLocation);

private:
	int _animID;
	int _flagOffset;
	int _warningMessageID;
};

class CycleEntryVideoWarning : public SceneBase {
public:
	CycleEntryVideoWarning(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int animIDA = 0, int animIDB = 0, int flagOffset = 0, int warningMessageID = -1);
	int postEnterRoom(Window *viewWindow, const Location &priorLocation);

private:
	int _animIDA;
	int _animIDB;
	int _flagOffset;
	int _warningMessageID;
};

class ClickPlayVideoSwitch : public SceneBase {
public:
	ClickPlayVideoSwitch(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int animID = 0, int cursorID = -1, int flagOffset = 0, int left = 0, int top = 0, int right = 0, int bottom = 0); 

	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	int _cursorID;
	int _animID;
	Common::Rect _clickRegion;
	int _flagOffset;
};

class ClickPlayVideo : public SceneBase {
public:
	ClickPlayVideo(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int animID = 0, int cursorID = -1, int left = 0, int top = 0, int right = 0, int bottom = 0); 

	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	int _cursorID;
	int _animID;
	Common::Rect _clickRegion;
};

class VideoDeath : public SceneBase {
public:
	VideoDeath(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation, int deathID = 0, int messageTextID = -1);
	int postEnterRoom(Window *viewWindow, const Location &priorLocation);
	int postExitRoom(Window *viewWindow, const Location &newLocation);

private:
	int _deathID;
	int _messageTextID;
};

class ClickChangeDepth : public SceneBase {
public:
	ClickChangeDepth(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int newDepth = 0, int cursorID = 0, int left = -1, int top = -1, int right = -1, int bottom = -1);
	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	int _newDepth;
	int _cursorID;
	Common::Rect _clickableRegion;
};

class BrowseBook : public SceneBase {
public:
	BrowseBook(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int bookResID = 0, int textStartResID = -1, int startingPageID = 0, int timeZone = -1, int environment = -1,
			int node = -1, int facing = -1, int orientation = -1, int depth = -1, int transitionType = -1, int transitionData = -1,
			int transitionStartFrame = -1, int transitionLength = -1);
	int gdiPaint(Window *viewWindow);
	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);
	int mouseMove(Window *viewWindow, const Common::Point &pointLocation);

private:
	int pageChanged(Window *viewWindow);
	int textTranslated(Window *viewWindow);

	Common::Array<BookPage> _bookDatabase;
	int _curPage;
	Common::Rect _top, _bottom, _left, _right, _putDown;
	DestinationScene _putDownDestination;
	int _translatedTextResourceID;
	int _curLineIndex;
};

class ClickPlaySoundSynchronous : public SceneBase {
public:
	ClickPlaySoundSynchronous(BuriedEngine *vm, Window *viewWindow, const LocationStaticData &sceneStaticData, const Location &priorLocation,
			int flagOffset = -1, int soundID = 0, int cursorID = 0, int left = 0, int top = 0, int right = 0, int bottom = 0);
	int mouseUp(Window *viewWindow, const Common::Point &pointLocation);
	int specifyCursor(Window *viewWindow, const Common::Point &pointLocation);

private:
	int _cursorID;
	int _soundID;
	Common::Rect _clickRegion;
	int _flagOffset;
};

} // End of namespace Buried

#endif