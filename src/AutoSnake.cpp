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
#include "AutoSnake.h"

void AutoSnake::MoveBySection(int dotX, int dotY) {
	SnakeSection *head = GetHeadSection();
	if (head->GetSectionX() == dotX && dotY > head->GetSectionY()) {
		if (head->GetDirection() == DIRECTION_UP) {
			if (head->GetSectionX() <= 0) {
				head->SetDirection(DIRECTION_RIGHT);
			} else {
				head->SetDirection(DIRECTION_LEFT);
			}
		} else {
			head->SetDirection(DIRECTION_DOWN);
		}
	} else if (head->GetSectionX() == dotX && dotY < head->GetSectionY()) {
		if (head->GetDirection() == DIRECTION_DOWN) {
			if (head->GetSectionX() <= 0) {
				head->SetDirection(DIRECTION_RIGHT);
			} else {
				head->SetDirection(DIRECTION_LEFT);
			}
		} else {
			head->SetDirection(DIRECTION_UP);
		}
	} else if (head->GetSectionX() > dotX) {
		if (head->GetDirection() == DIRECTION_RIGHT) {
			if (head->GetSectionY() <= 0) {
				head->SetDirection(DIRECTION_DOWN);
			} else {
				head->SetDirection(DIRECTION_UP);
			}
		} else {
			head->SetDirection(DIRECTION_LEFT);
		}
	} else if (head->GetSectionX() < dotX) {
		if (head->GetDirection() == DIRECTION_LEFT) {
			if (head->GetSectionY() <= 0) {
				head->SetDirection(DIRECTION_DOWN);
			} else {
				head->SetDirection(DIRECTION_UP);
			}
		} else {
			head->SetDirection(DIRECTION_RIGHT);
		}
	}
	SnakeBody::MoveBySection();
}
