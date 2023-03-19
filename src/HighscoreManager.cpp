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
#include "HighscoreManager.h"
#include "Utilities.h"
#include <FIo.h>

using namespace Osp::Io;

extern HighscoreManager *HighscoreManager::__pInstance;

result HighscoreRecordComparer::Compare(HighscoreRecord *const &obj1, HighscoreRecord *const &obj2, int& cmp) const {
	cmp = obj2->score - obj1->score;
	return E_SUCCESS;
}

result HighscoreManager::Initialize(void) {
	__pInstance = new HighscoreManager;
	return __pInstance->Construct();
}
HighscoreManager *HighscoreManager::GetInstance(void) {
	return __pInstance;
}

void HighscoreManager::RemoveInstance(void) {
	if (__pInstance) {
		delete __pInstance;
		__pInstance = null;
	}
}

HighscoreManager::HighscoreManager(void) {
	__pRecords = null;
	__recordsChanged = false;
}

HighscoreManager::~HighscoreManager(void) {
	if (__pRecords) {
		delete __pRecords;
	}
}

result HighscoreManager::Serialize(void) {
	if (!__recordsChanged) {
		return E_SUCCESS;
	}
	if (__pRecords) {
		TrimExtraRecords();

		ByteBuffer *buf = new ByteBuffer;
		result res = buf->Construct(160);
		if (IsFailed(res)) {
			delete buf;
			return res;
		}

		IEnumeratorT<HighscoreRecord *> *pEnum = __pRecords->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			HighscoreRecord *rec = null;
			pEnum->GetCurrent(rec);
			if (rec) {
				buf->SetInt(rec->recordDate.GetDay());
				buf->SetInt(rec->recordDate.GetMonth());
				buf->SetInt(rec->recordDate.GetYear());
				buf->SetInt(rec->score);
			}
		}
		buf->SetPosition(0);

		File *file = new File;
		res = file->Construct(L"/Home/scores.dat", L"w", true);
		if (IsFailed(res)) {
			delete buf;
			delete file;
			return res;
		}

		file->Write(*buf);
		file->Flush();

		delete file;
		delete buf;

		return E_SUCCESS;
	} else {
		return E_INVALID_STATE;
	}
}

result HighscoreManager::Construct(void) {
	if (File::IsFileExist(L"/Home/scores.dat")) {
		ByteBuffer *buf = new ByteBuffer;
		result res = buf->Construct(160);
		if (IsFailed(res)) {
			delete buf;
			return res;
		}

		__pRecords = new ArrayListT<HighscoreRecord *>;
		res = __pRecords->Construct(10);
		if (IsFailed(res)) {
			delete buf;
			delete __pRecords;
			__pRecords = null;
			return res;
		}

		File *file = new File;
		res = file->Construct(L"/Home/scores.dat", L"r", false);
		if (IsFailed(res)) {
			delete buf;
			delete file;
			return res;
		}

		file->Read(*buf);
		delete file;

		buf->SetPosition(0);
		while (buf->GetPosition() < buf->GetLimit()) {
			int day = -1;
			int mon = -1;
			int year = -1;
			int score = -1;

			buf->GetInt(day);
			buf->GetInt(mon);
			buf->GetInt(year);
			buf->GetInt(score);

			if (day >= 0 && mon >= 0 && year >= 0 && score >= 0) {
				DateTime dt;
				dt.SetValue(year, mon, day, 0, 0, 0);

				HighscoreRecord *record = new HighscoreRecord;
				record->recordDate = dt;
				record->score = score;

				__pRecords->Add(record);
			}
		}

		delete buf;
	} else {
		ByteBuffer *buf = new ByteBuffer;
		result res = buf->Construct(160);
		if (IsFailed(res)) {
			delete buf;
			return res;
		}

		__pRecords = new ArrayListT<HighscoreRecord *>;
		res = __pRecords->Construct(10);
		if (IsFailed(res)) {
			delete buf;
			delete __pRecords;
			__pRecords = null;
			return res;
		}

		DateTime current = Utilities::GetCurrentLocalDateTime();

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(5000);

		HighscoreRecord *record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 5000;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(4500);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 4500;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(4000);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 4000;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(3500);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 3500;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(3000);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 3000;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(2500);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 2500;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(2000);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 2000;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(1500);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 1500;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(1000);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 1000;
		__pRecords->Add(record);

		current.AddDays(-1);

		buf->SetInt(current.GetDay());
		buf->SetInt(current.GetMonth());
		buf->SetInt(current.GetYear());
		buf->SetInt(500);

		record = new HighscoreRecord;
		record->recordDate = current;
		record->score = 500;
		__pRecords->Add(record);

		buf->SetPosition(0);

		File *file = new File;
		res = file->Construct(L"/Home/scores.dat", L"w", true);
		if (IsFailed(res)) {
			delete buf;
			delete file;
			return res;
		}

		file->Write(*buf);
		file->Flush();

		delete file;
		delete buf;
	}
	return E_SUCCESS;
}

int HighscoreManager::AddRecord(int score) {
	if (__pRecords) {
		DateTime current = Utilities::GetCurrentLocalDateTime();
		HighscoreRecord *record = new HighscoreRecord;
		record->recordDate = current;
		record->score = score;
		__pRecords->Add(record);

		__pRecords->Sort(HighscoreRecordComparer());
		TrimExtraRecords();

		bool found = false;
		int index = 0;
		IEnumeratorT<HighscoreRecord *> *pEnum = __pRecords->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			HighscoreRecord *rec = null;
			pEnum->GetCurrent(rec);
			if (rec) {
				if (rec->recordDate.GetDay() == current.GetDay() && rec->recordDate.GetMonth() == current.GetMonth()
					&& rec->recordDate.GetYear() == current.GetYear() && rec->score == score) {
					found = true;
					break;
				}
			}
			index++;
		}
		delete pEnum;

		__recordsChanged = true;
		if (found) {
			return index;
		} else {
			return -1;
		}
	}
	return -1;
}

void HighscoreManager::TrimExtraRecords(void) {
	if (__pRecords) {
		while (__pRecords->GetCount() > 10) {
			__pRecords->RemoveAt(__pRecords->GetCount() - 1);
			__recordsChanged = true;
		}
	}
}
