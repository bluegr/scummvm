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

#include "titanic/star_control/fpose.h"

namespace Titanic {

// Non-member functions

void fposeProd(const FPose &a, const FPose &m, FPose &C) {
	C._row1._x = a._row1._y * m._row2._x + a._row1._z * m._row3._x + a._row1._x * m._row1._x;
	C._row1._y = a._row1._x * m._row1._y + m._row2._y * a._row1._y + m._row3._y * a._row1._z;
	C._row1._z = a._row1._x * m._row1._z + a._row1._y * m._row2._z + a._row1._z * m._row3._z;
	C._row2._x = m._row1._x * a._row2._x + m._row3._x * a._row2._z + m._row2._x * a._row2._y;
	C._row2._y = m._row3._y * a._row2._z + m._row1._y * a._row2._x + m._row2._y * a._row2._y;
	C._row2._z = a._row2._z * m._row3._z + a._row2._x * m._row1._z + a._row2._y * m._row2._z;
	C._row3._x = m._row1._x * a._row3._x + a._row3._z * m._row3._x + a._row3._y * m._row2._x;
	C._row3._y = a._row3._y * m._row2._y + a._row3._z * m._row3._y + a._row3._x * m._row1._y;
	C._row3._z = a._row3._x * m._row1._z + a._row3._y * m._row2._z + a._row3._z * m._row3._z;
	C._vector._x = a._vector._x * m._row1._x + a._vector._y * m._row2._x + a._vector._z * m._row3._x + m._vector._x;
	C._vector._y = a._vector._x * m._row1._y + a._vector._y * m._row2._y + a._vector._z * m._row3._y + m._vector._y;
	C._vector._z = a._vector._x * m._row1._z + a._vector._y * m._row2._z + a._vector._z * m._row3._z + m._vector._z;
}

// Member functions

FPose::FPose() {
	clear();
}

FPose::FPose(Axis axis, float amount) {
	setRotationMatrix(axis, amount);
}

FPose::FPose(const FPose &src) : FMatrix() {
	copyFrom(src);
}

FPose::FPose(const FPose &s1, const FPose &s2) {
	fposeProd(s1, s2, *this);
}

FPose::FPose(int mode, const FVector &src) {
	switch (mode) {
	case 0:
		_row1._x = 1.0;
		_row2._y = 1.0;
		_row3._z = 1.0;
		_vector = src;
		break;

	case 1:
		_row1._x = src._x;
		_row2._y = src._y;
		_row3._z = src._z;
		break;

	default:
		_row1._x = 1.0;
		_row2._y = 1.0;
		_row3._z = 1.0;
		break;
	}
}

void FPose::identity() {
	FMatrix::identity();
	_vector.clear();
}

// Source: https://en.wikipedia.org/wiki/Rotation_matrix
void FPose::setRotationMatrix(Axis axis, float amount) {
	const float ROTATION = 2 * M_PI / 360.0;
	float sinVal = sin(amount * ROTATION);
	float cosVal = cos(amount * ROTATION);

	switch (axis) {
	case X_AXIS:
		_row1._x = 1.0;
		_row1._y = 0.0;
		_row1._z = 0.0;
		_row2._x = 0.0;
		_row2._y = cosVal;
		_row2._z = sinVal;
		_row3._x = 0.0;
		_row3._y = -sinVal;
		_row3._z = cosVal;
		break;

	case Y_AXIS:
		_row1._x = cosVal;
		_row1._y = 0.0;
		_row1._z = -sinVal;
		_row2._x = 0.0;
		_row2._y = 1.0;
		_row2._z = 0.0;
		_row3._x = sinVal;
		_row3._y = 0.0;
		_row3._z = cosVal;
		break;

	case Z_AXIS:
		_row1._x = cosVal;
		_row1._y = sinVal;
		_row1._z = 0.0;
		_row2._x = -sinVal;
		_row2._y = cosVal;
		_row2._z = 0.0;
		_row3._x = 0.0;
		_row3._y = 0.0;
		_row3._z = 1.0;
		break;

	default:
		break;
	}

	_vector.clear();
}

void FPose::rotVectAxisY(double angleDeg) {
	_row1.rotVectAxisY(angleDeg);
	_row2.rotVectAxisY(angleDeg);
	_row3.rotVectAxisY(angleDeg);
	_vector.rotVectAxisY(angleDeg);
}

void FPose::copyFrom(const FPose &src) {
	_row1 = src._row1;
	_row2 = src._row2;
	_row3 = src._row3;
	_vector = src._vector;
}

void FPose::copyFrom(const FMatrix &src) {
	_row1 = src._row1;
	_row2 = src._row2;
	_row3 = src._row3;
}

FPose FPose::inverseTransform() const {
	FPose result;

	result._row1._x = _row1._x;
	result._row2._x = _row1._y;
	result._row3._x = _row1._z;
	result._row1._y = _row2._x;
	result._row2._y = _row2._y;
	result._row3._y = _row2._z;
	result._row1._z = _row3._x;
	result._row2._z = _row3._y;
	result._row3._z = _row3._z;

	result._vector._x = -(_vector._x * result._row1._x
		+ _vector._y * result._row2._x
		+ _vector._z * result._row3._x);
	result._vector._y = -(_vector._x * result._row1._y
		+ _vector._y * result._row2._y
		+ _vector._z * result._row3._y);
	result._vector._z = -(_vector._x * result._row1._z
		+ _vector._y * result._row2._z
		+ _vector._z * result._row3._z);

	return result;
}

FPose FPose::compose2(const FPose &m) {
	FPose dm;
	dm._row1 = _row1.MatProdRowVect(m);
	dm._row2 = _row2.MatProdRowVect(m);
	dm._row3 = _row3.MatProdRowVect(m);
	dm._vector = _vector.MatProdRowVect(m);

	return dm;
}

} // End of namespace Titanic
