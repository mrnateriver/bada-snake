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
#ifndef SNAKEBODY_H_
#define SNAKEBODY_H_

#include "SnakeSection.h"
#include <FBase.h>

using namespace Osp::Base::Collection;

class SnakeBody {
public:
	SnakeBody(void);
	virtual ~SnakeBody(void);

	void Construct(int x, int y, MovingDirection dir, int sections = 1);

	int GetNumberOfSections(void) const {
		return __pSections->GetCount();
	}

	void AddTailSection(void);
	void AddTailSection(SnakeSection *sect);
	void RemoveTailSections(int n);

	void MoveBySection(void);

	bool CoversCoords(int x, int y) const;
	bool IntersectsItself(void) const;

	SnakeSection *GetHeadSection(void) const;
	int GetHeadSectionX(void) const;
	int GetHeadSectionY(void) const;

	IEnumeratorT<SnakeSection *> *GetEnumeratorN(void) const {
		return __pSections->GetEnumeratorN();
	}

private:
	LinkedListT<SnakeSection *> *__pSections;
};

#endif
