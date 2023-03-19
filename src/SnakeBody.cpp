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
#include "SnakeBody.h"

SnakeBody::SnakeBody(void) {
	__pSections = null;
}

SnakeBody::~SnakeBody(void) {
	if (__pSections) {
		IEnumeratorT<SnakeSection *> *pEnum = __pSections->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			SnakeSection *sect = null;
			pEnum->GetCurrent(sect);
			if (sect) {
				delete sect;
			}
		}
		delete pEnum;
		delete __pSections;
	}
}

void SnakeBody::Construct(int x, int y, MovingDirection dir, int sections) {
	__pSections = new LinkedListT<SnakeSection *>;

	int xc = x, yc = y;
	for (int i = 0; i < sections; i++) {
		SnakeSection *sect = new SnakeSection(xc, yc, dir);
		__pSections->Add(sect);

		if (dir == DIRECTION_UP) {
			yc++;
		} else if (dir == DIRECTION_DOWN) {
			yc--;
		} else if (dir == DIRECTION_LEFT) {
			xc++;
		} else if (dir == DIRECTION_RIGHT) {
			xc--;
		}
	}
}

void SnakeBody::AddTailSection(void) {
	SnakeSection *lastSect = null;
	__pSections->GetAt(__pSections->GetCount() - 1, lastSect);
	if (lastSect) {
		int xc = lastSect->GetSectionX(), yc = lastSect->GetSectionY();
		MovingDirection dir = lastSect->GetDirection();

		if (dir == DIRECTION_UP) {
			yc++;
		} else if (dir == DIRECTION_DOWN) {
			yc--;
		} else if (dir == DIRECTION_LEFT) {
			xc++;
		} else if (dir == DIRECTION_RIGHT) {
			xc--;
		}

		SnakeSection *sect = new SnakeSection(xc, yc, lastSect->GetDirection());
		__pSections->Add(sect);
	}
}

void SnakeBody::AddTailSection(SnakeSection *sect) {
	__pSections->Add(sect);
}

void SnakeBody::RemoveTailSections(int n) {
	if (__pSections->GetCount() > n) {
		int target = __pSections->GetCount() - n;
		while (__pSections->GetCount() > target) {
			__pSections->RemoveAt(__pSections->GetCount() - 1);
		}
	}
}

void SnakeBody::MoveBySection(void) {
	MovingDirection lastDir = DIRECTION_UP;
	bool setDir = false;

	IEnumeratorT<SnakeSection *> *pEnum = __pSections->GetEnumeratorN();
	while (!IsFailed(pEnum->MoveNext())) {
		SnakeSection *sect = null;
		pEnum->GetCurrent(sect);
		if (sect) {
			sect->MoveBySection();

			MovingDirection tmpDir = sect->GetDirection();
			if (setDir) {
				sect->SetDirection(lastDir);
			}
			lastDir = tmpDir;
			setDir = true;
		}
	}
	delete pEnum;
}

bool SnakeBody::CoversCoords(int x, int y) const {
	IEnumeratorT<SnakeSection *> *pEnum = __pSections->GetEnumeratorN();
	while (!IsFailed(pEnum->MoveNext())) {
		SnakeSection *sect = null;
		pEnum->GetCurrent(sect);
		if (sect) {
			if (sect->GetSectionX() == x && sect->GetSectionY() == y) {
				delete pEnum;
				return true;
			}
		}
	}
	delete pEnum;
	return false;
}

bool SnakeBody::IntersectsItself(void) const {
	IEnumeratorT<SnakeSection *> *pEnum = __pSections->GetEnumeratorN();
	while (!IsFailed(pEnum->MoveNext())) {
		SnakeSection *sect = null;
		pEnum->GetCurrent(sect);
		if (sect) {
			IEnumeratorT<SnakeSection *> *pEnumIns = __pSections->GetEnumeratorN();
			while (!IsFailed(pEnumIns->MoveNext())) {
				SnakeSection *sectIns = null;
				pEnumIns->GetCurrent(sectIns);
				if (sectIns) {
					if (sectIns != sect && (sect->GetSectionX() == sectIns->GetSectionX() && sect->GetSectionY() == sectIns->GetSectionY())) {
						delete pEnumIns;
						delete pEnum;
						return true;
					}
				}
			}
			delete pEnumIns;
		}
	}
	delete pEnum;
	return false;
}

SnakeSection *SnakeBody::GetHeadSection(void) const {
	SnakeSection *head = null;
	__pSections->GetAt(0, head);

	return head;
}

int SnakeBody::GetHeadSectionX(void) const {
	SnakeSection *head = GetHeadSection();
	if (head) {
		return head->GetSectionX();
	} else {
		return -1;
	}
}

int SnakeBody::GetHeadSectionY(void) const {
	SnakeSection *head = GetHeadSection();
	if (head) {
		return head->GetSectionY();
	} else {
		return -1;
	}
}
