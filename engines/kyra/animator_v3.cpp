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

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * $URL$
 * $Id$
 *
 */

#include "kyra/kyra_v3.h"
#include "kyra/wsamovie.h"

namespace Kyra {

void KyraEngine_v3::clearAnimObjects() {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::clearAnimObjects()");

	for (int i = 0; i < 67; ++i)
		_animObjects[i].enabled = false;

	_animObjects[0].index = 0;
	_animObjects[0].type = 0;
	_animObjects[0].enabled = true;
	_animObjects[0].unk8 = 1;
	_animObjects[0].flags = 0x800;
	_animObjects[0].width = 57;
	_animObjects[0].height = 91;
	_animObjects[0].width2 = 4;
	_animObjects[0].height2 = 10;

	for (int i = 1; i < 17; ++i) {
		_animObjects[i].type = 2;
		_animObjects[i].flags = 0;
		_animObjects[i].enabled = false;
		_animObjects[i].needRefresh = 0;
		_animObjects[i].unk8 = 1;
	}

	for (int i = 17; i <= 66; ++i) {
		_animObjects[i].type = 1;
		_animObjects[i].unk8 = 1;
		_animObjects[i].flags = 0x800;
		_animObjects[i].width = 24;
		_animObjects[i].height = 20;
		_animObjects[i].width2 = 0;
		_animObjects[i].height2 = 0;
	}
}

KyraEngine_v3::AnimObj *KyraEngine_v3::initAnimList(AnimObj *list, AnimObj *entry) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::initAnimList(%p, %p)", (const void*)list, (const void*)entry);
	entry->nextObject = list;
	return entry;
}

KyraEngine_v3::AnimObj *KyraEngine_v3::addToAnimListSorted(AnimObj *list, AnimObj *add) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::addToAnimListSorted(%p, %p)", (const void*)list, (const void*)add);
	add->nextObject = 0;

	if (!list)
		return add;

	if (add->yPos1 <= list->yPos1) {
		add->nextObject = list;
		return add;
	}

	AnimObj *cur = list;
	AnimObj *prev = list;
	while (add->yPos1 > cur->yPos1) {
		AnimObj *temp = cur->nextObject;
		if (!temp)
			break;
		prev = cur;
		cur = temp;
	}

	if (add->yPos1 <= cur->yPos1) {
		prev->nextObject = add;
		add->nextObject = cur;
	} else {
		cur->nextObject = add;
		add->nextObject = 0;
	}
	return list;
}

KyraEngine_v3::AnimObj *KyraEngine_v3::deleteAnimListEntry(AnimObj *list, AnimObj *entry) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::addToAnimListSorted(%p, %p)", (const void*)list, (const void*)entry);
	if (!list)
		return 0;

	AnimObj *old = 0;
	AnimObj *cur = list;

	while (true) {
		if (cur == entry)
			break;
		if (!cur->nextObject)
			break;
		old = cur;
		cur = cur->nextObject;
	}

	if (cur != entry)
		return list;

	if (cur == list) {
		if (!cur->nextObject)
			return 0;
		cur = cur->nextObject;
		return cur;
	}

	if (!cur->nextObject) {
		if (!old)
			return 0;
		old->nextObject = 0;
		return list;
	}

	if (cur != entry)
		return list;

	old->nextObject = entry->nextObject;
	return list;
}

void KyraEngine_v3::animSetupPaletteEntry(AnimObj *anim) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::animSetupPaletteEntry(%p)", (const void*)anim);
	int layer = _screen->getLayer(anim->xPos1, anim->yPos1) - 1;
	int16 count = 0;
	for (int i = 0; i < 3; ++i)
		count += _sceneDatPalette[layer*3+i];
	count /= 3;
	count *= -1;
	count = MAX<int16>(0, MIN<int16>(count, 10));
	anim->palette = count / 3;
}

void KyraEngine_v3::restorePage3() {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::restorePage3()");
	musicUpdate(0);
	_screen->copyBlockToPage(3, 0, 0, 320, 200, _gamePlayBuffer);
	musicUpdate(0);
}

void KyraEngine_v3::drawAnimObjects() {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::drawAnimObjects()");
	for (AnimObj *curObject = _animList; curObject; curObject = curObject->nextObject) {
		if (!curObject->enabled)
			continue;

		int x = curObject->xPos2 - (_screen->getScreenDim(2)->sx << 3);
		int y = curObject->yPos2 - _screen->getScreenDim(2)->sy;
		int layer = 7;

		if (curObject->flags & 0x800) {
			if (!curObject->unk8)
				layer = 0;
			else
				layer = getDrawLayer(curObject->xPos1, curObject->yPos1);
		}

		if (curObject->index)
			drawSceneAnimObject(curObject, x, y, layer);
		else
			drawCharacterAnimObject(curObject, x, y, layer);
	}
}

void KyraEngine_v3::drawSceneAnimObject(AnimObj *obj, int x, int y, int layer) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::drawSceneAnimObject(%p, %d, %d, %d)", (const void*)obj, x, y, layer);
	if (obj->type == 1) {
		if (obj->shapeIndex == 0xFFFF)
			return;
		int scale = getScale(obj->xPos1, obj->yPos1);
		_screen->drawShape(2, getShapePtr(obj->shapeIndex), x, y, 2, obj->flags | 104, _paletteOverlay, obj->palette, layer, scale, scale);
	} else {
		if (obj->shapePtr) {
			_screen->drawShape(2, obj->shapePtr, x, y, 2, obj->flags, 7);
		} else {
			if (obj->shapeIndex3 == 0xFFFF || obj->animNum == 0xFFFF)
				return;
			uint16 flags = 0x4000;
			if (obj->flags & 0x800)
				flags |= 0x8000;
			int x = obj->xPos2 - _sceneAnimMovie[obj->animNum]->xAdd();
			int y = obj->yPos1 - _sceneAnimMovie[obj->animNum]->yAdd();
			_sceneAnimMovie[obj->animNum]->setDrawPage(2);
			_sceneAnimMovie[obj->animNum]->setX(x);
			_sceneAnimMovie[obj->animNum]->setY(y);
			_sceneAnimMovie[obj->animNum]->displayFrame(obj->shapeIndex3, flags | layer);
		}
	}
}

void KyraEngine_v3::drawCharacterAnimObject(AnimObj *obj, int x, int y, int layer) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::drawCharacterAnimObject(%p, %d, %d, %d)", (const void*)obj, x, y, layer);
	if (_drawNoShapeFlag)
		return;

	if (_mainCharacter.animFrame < 9)
		_mainCharacter.animFrame = 87;

	if (obj->shapeIndex == 0xFFFF || _mainCharacter.animFrame == 87)
		return;

	_screen->drawShape(2, getShapePtr(421), _mainCharacter.x3, _mainCharacter.y3, 2, obj->flags | 304, _paletteOverlay, 3, layer, _charScale, _charScale);
	uint8 *shape = getShapePtr(_mainCharacter.animFrame);
	if (shape)
		_screen->drawShape(2, shape, x, y, 2, obj->flags | 2, layer, _charScale, _charScale);
}

void KyraEngine_v3::refreshAnimObjects(int force) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::refreshAnimObjects(%d)", force);
	for (AnimObj *curObject = _animList; curObject; curObject = curObject->nextObject) {
		if (!curObject->enabled)
			continue;
		if (!curObject->needRefresh && !force)
			continue;

		const int scale = (curObject->index == 0) ? _charScale : 0;

		int x = curObject->xPos2 - curObject->width2;
		if (scale)
			x -= (0x100 - scale) >> 4;

		if (x < 0)
			x = 0;
		if (x >= 320)
			x = 319;

		int y = curObject->yPos2 - curObject->height2;
		if (scale)
			y -= (0x100 - scale) >> 3;
		if (y < 0)
			y = 0;
		if (y >= 187)
			y = 186;

		int width = curObject->width + curObject->width2 + 8;
		int height = curObject->height + curObject->height2*2;
		if (width + x > 320)
			width -= width + x - 322;

		const int maxY = _inventoryState ? 143 : 187;
		if (height + y > maxY)
			height -= height + y - (maxY + 1);

		if (height > 0) {
			_screen->hideMouse();
			_screen->copyRegion(x, y, x, y, width, height, 2, 0, Screen::CR_NO_P_CHECK);
			_screen->showMouse();
		}

		curObject->needRefresh = false;
	}
}

void KyraEngine_v3::refreshAnimObjectsIfNeed() {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::refreshAnimObjectsIfNeed()");
	for (AnimObj *curEntry = _animList; curEntry; curEntry = curEntry->nextObject) {
		if (curEntry->enabled && curEntry->needRefresh) {
			restorePage3();
			drawAnimObjects();
			refreshAnimObjects(0);
			_screen->updateScreen();
			return;
		}
	}
}

void KyraEngine_v3::updateCharacterAnim(int charId) {
	debugC(9, kDebugLevelAnimator, "KyraEngine_v3::updateCharacterAnim(%d)", charId);

	AnimObj *obj = &_animObjects[0];
	obj->needRefresh = true;
	obj->flags &= ~1;
	obj->xPos1 = _mainCharacter.x1;
	obj->yPos1 = _mainCharacter.y1;
	obj->shapePtr = getShapePtr(_mainCharacter.animFrame);
	obj->shapeIndex = obj->shapeIndex2 = _mainCharacter.animFrame;

	int shapeOffsetX = 0, shapeOffsetY = 0;
	//XXX if (_mainCharacter.animFrame >= 50 && _mainCharacter.animFrame <= 87) {
		shapeOffsetX = _malcolmShapeXOffset;
		shapeOffsetY = _malcolmShapeYOffset;
	//} else {
	//}

	obj->xPos2 = _mainCharacter.x1;
	obj->yPos2 = _mainCharacter.y1;
	_charScale = getScale(_mainCharacter.x1, _mainCharacter.y1);
	obj->xPos2 += (shapeOffsetX * _charScale) >> 8;
	obj->yPos2 += (shapeOffsetY * _charScale) >> 8;
	_mainCharacter.x3 = _mainCharacter.x1 - (_charScale >> 4) - 1;
	_mainCharacter.y3 = _mainCharacter.y1 - (_charScale >> 6) - 1;
	//if (_charSpecialWidth2 == -1) {
		obj->width2 = 4;
		obj->height2 = 10;
	//}

	for (int i = 1; i <= 16; ++i) {
		if (_animObjects[i].enabled && _animObjects[i].unk8)
			_animObjects[i].needRefresh = true;
	}

	obj = deleteAnimListEntry(_animList, obj);
	if (_animList)
		_animList = addToAnimListSorted(_animList, obj);
	else
		_animList = initAnimList(_animList, obj);

	if (!_loadingState)
		updateCharPal(1);
}

} // end of namespace Kyra
