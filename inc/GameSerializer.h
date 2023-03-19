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
#ifndef GAMESERIALIZER_H_
#define GAMESERIALIZER_H_

#include "SnakeBody.h"

struct GameState {
	int score;
	int lives;
	int speed;

	SnakeBody *snake;

	int dotX;
	int dotY;
	int dotMode;
};

class GameSerializer {
public:
	static result Initialize(void);
	static GameSerializer *GetInstance(void);
	static void RemoveInstance(void);

private:
	static GameSerializer *__pInstance;

public:
	GameSerializer(void);
	~GameSerializer(void);

	result Construct(void);
	result Serialize(void);

	bool HasGame(void) const {
		return __hasGame;
	}
	void RemoveGame(bool deallocSnake);

	void SetGame(GameState state);
	GameState GetGame(void) const;

private:
	bool __hasGame;

	int __score;
	int __lives;
	int __speed;
	SnakeBody *__pSnake;
	int __dotX;
	int __dotY;
	int __dotMode;
};

#endif
