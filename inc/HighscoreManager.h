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
#ifndef HIGHSCOREMANAGER_H_
#define HIGHSCOREMANAGER_H_

#include <FBase.h>

using namespace Osp::Base;
using namespace Osp::Base::Collection;

struct HighscoreRecord {
	DateTime recordDate;
	int score;
};

class HighscoreRecordComparer: public IComparerT<HighscoreRecord *> {
	virtual result Compare(HighscoreRecord *const &obj1, HighscoreRecord *const &obj2, int& cmp) const;
};

class HighscoreManager {
public:
	static result Initialize(void);
	static HighscoreManager *GetInstance(void);
	static void RemoveInstance(void);

private:
	static HighscoreManager *__pInstance;

public:
	HighscoreManager(void);
	~HighscoreManager(void);

	result Construct(void);
	result Serialize(void);

	int AddRecord(int score);

	ArrayListT<HighscoreRecord *> *GetRecords(void) const {
		return __pRecords;
	}

private:
	void TrimExtraRecords(void);

	ArrayListT<HighscoreRecord *> *__pRecords;
	bool __recordsChanged;
};

#endif
