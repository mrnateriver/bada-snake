/*
 * Copyright (c) 2016 Evgenii Dobrovidov
 * This file is part of "Snake".
 *
 * "Snake" is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * "Snake" is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with "Snake".  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef SNAKESECTION_H_
#define SNAKESECTION_H_

enum MovingDirection {
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT
};

class SnakeSection {
public:
	SnakeSection(int x, int y, MovingDirection dir): __sectX(x), __sectY(y), __dir(dir) { }

	int GetSectionX(void) const {
		return __sectX;
	}
	int GetSectionY(void) const {
		return __sectY;
	}
	MovingDirection GetDirection(void) const {
		return __dir;
	}

	void SetDirection(MovingDirection dir) {
		__dir = dir;
	}

	void MoveBySection(void) {
		if (__dir == DIRECTION_UP) {
			__sectY--;
		} else if (__dir == DIRECTION_DOWN) {
			__sectY++;
		} else if (__dir == DIRECTION_LEFT) {
			__sectX--;
		} else if (__dir == DIRECTION_RIGHT) {
			__sectX++;
		}
	}

private:
	int __sectX;
	int __sectY;
	MovingDirection __dir;
};

#endif
