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
#include "GameSerializer.h"
#include <FIo.h>

using namespace Osp::Base;
using namespace Osp::Io;

extern GameSerializer *GameSerializer::__pInstance;

result GameSerializer::Initialize(void) {
	__pInstance = new GameSerializer();
	return __pInstance->Construct();
}

void GameSerializer::RemoveInstance(void) {
	if (__pInstance) {
		delete __pInstance;
		__pInstance = null;
	}
}

GameSerializer *GameSerializer::GetInstance(void) {
	return __pInstance;
}

GameSerializer::GameSerializer(void) {
	__hasGame = false;

	__score = 0;
	__lives = 3;
	__speed = 100;
	__pSnake = null;
	__dotX = 0;
	__dotY = 0;
	__dotMode = 0;
}

GameSerializer::~GameSerializer(void) {
	if (__pSnake) {
		delete __pSnake;
	}
}

result GameSerializer::Construct(void) {
	if (File::IsFileExist(L"/Home/game.dat")) {
		ByteBuffer *buf = new ByteBuffer;
		result res = buf->Construct(18032);
		if (IsFailed(res)) {
			delete buf;
			return res;
		}

		File *file = new File;
		res = file->Construct(L"/Home/game.dat", L"r", false);
		if (IsFailed(res)) {
			delete buf;
			delete file;
			return res;
		}

		file->Read(*buf);
		delete file;

		buf->SetPosition(0);

		int hasGame = 0;
		buf->GetInt(hasGame);

		if (hasGame) {
			int score = 0;
			int lives = 3;
			int speed = 100;
			int dotX = 0;
			int dotY = 0;
			int dotMode = 0;

			int snakeSections = 0;

			buf->GetInt(score);
			buf->GetInt(lives);
			buf->GetInt(speed);
			buf->GetInt(dotX);
			buf->GetInt(dotY);
			buf->GetInt(dotMode);

			buf->GetInt(snakeSections);

			SnakeBody *body = null;
			for (int i = 0; i < snakeSections; i++) {
				int sectX = -1;
				int sectY = -1;
				int direction = -1;

				buf->GetInt(sectX);
				buf->GetInt(sectY);
				buf->GetInt(direction);

				if (sectX >= 0 && sectY >= 0 && direction >= 0) {
					if (!body) {
						body = new SnakeBody;
						body->Construct(sectX, sectY, (MovingDirection)direction, 1);
					} else {
						SnakeSection *section = new SnakeSection(sectX, sectY, (MovingDirection)direction);
						body->AddTailSection(section);
					}
				}
			}

			if (body) {
				__pSnake = body;

				__score = score;
				__lives = lives;
				__speed = speed;
				__dotX = dotX;
				__dotY = dotY;
				__dotMode = dotMode;
			} else {
				hasGame = 0;
			}
		}
		__hasGame = (bool)hasGame;
		delete buf;
	} else {
		ByteBuffer *buf = new ByteBuffer;
		result res = buf->Construct(4);
		if (IsFailed(res)) {
			delete buf;
			return res;
		}

		buf->SetInt(0);
		buf->SetPosition(0);

		File *file = new File;
		res = file->Construct(L"/Home/game.dat", L"w", true);
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

result GameSerializer::Serialize(void) {
	ByteBuffer *buf = new ByteBuffer;
	result res = buf->Construct((__hasGame && __pSnake) ? 32 + 12*__pSnake->GetNumberOfSections() : 4);
	if (IsFailed(res)) {
		delete buf;
		return res;
	}

	if (!__hasGame) {
		buf->SetInt(0);
	} else {
		buf->SetInt(1);

		buf->SetInt(__score);
		buf->SetInt(__lives);
		buf->SetInt(__speed);
		buf->SetInt(__dotX);
		buf->SetInt(__dotY);
		buf->SetInt(__dotMode);
		if (__pSnake) {
			buf->SetInt(__pSnake->GetNumberOfSections());

			IEnumeratorT<SnakeSection *> *pEnum = __pSnake->GetEnumeratorN();
			while (!IsFailed(pEnum->MoveNext())) {
				SnakeSection *sect = null;
				pEnum->GetCurrent(sect);
				if (sect) {
					buf->SetInt(sect->GetSectionX());
					buf->SetInt(sect->GetSectionY());
					buf->SetInt((int)sect->GetDirection());
				}
			}
			delete pEnum;
		} else {
			buf->SetInt(0);
		}
	}
	buf->SetPosition(0);

	File *file = new File;
	res = file->Construct(L"/Home/game.dat", L"w", true);
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
}

void GameSerializer::SetGame(GameState state) {
	__pSnake = state.snake;

	__score = state.score;
	__lives = state.lives;
	__speed = state.speed;
	__dotX = state.dotX;
	__dotY = state.dotY;
	__dotMode = state.dotMode;

	__hasGame = true;
}

GameState GameSerializer::GetGame(void) const {
	GameState state;

	state.snake = __pSnake;
	state.score = __score;
	state.lives = __lives;
	state.speed = __speed;
	state.dotX = __dotX;
	state.dotY = __dotY;
	state.dotMode = __dotMode;

	return state;
}

void GameSerializer::RemoveGame(bool deallocSnake) {
	__hasGame = false;

	if (deallocSnake && __pSnake) {
		delete __pSnake;
	}
	__pSnake = null;
}
