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
#ifndef _MAINMENU_H_
#define _MAINMENU_H_

#include "BasicForm.h"
#include "AutoSnake.h"
#include <FGraphics.h>

using namespace Osp::Base::Runtime;
using namespace Osp::Graphics;

class MainMenu: public BasicForm, public ITimerEventListener {
public:
	MainMenu(void);
	virtual ~MainMenu(void);

private:
	void OnButtonContinueClicked(const Control &src);
	void OnButtonNewGameClicked(const Control &src);
	void OnButtonHighscoresClicked(const Control &src);
	void OnButtonScoresBackClicked(const Control &src);
	void OnButtonExitClicked(const Control &src);

	bool UpdateSnake(void);
	void DrawSnake(void);
	void DrawDot(void);
	void DrawScores(void);
	void DrawShade(void);
	void ResetSnake(void);

	virtual result Initialize(void);
	virtual result Terminate(void);

	virtual result OnGivenData(IList *pArgs, DataReceiveReason reason, int srcID);

	virtual result OnDraw(void);

	virtual void OnTimerExpired(Timer &timer);

	Label *__pHeaderLabel;
	Button *__pContinueButton;
	Button *__pNewGameButton;
	Button *__pHighscoresButton;
	Button *__pExitButton;
	Button *__pScoresBackButton;

	CustomList *__pScoresList;
	CustomListItemFormat *__pScoresListFormat;

	static const int SCORES_LIST_ELEMENT_DATE = 501;
	static const int SCORES_LIST_ELEMENT_SCORE = 502;

	AutoSnake *__pSnake;
	Timer *__pUpdateTimer;
	Canvas *__pCanvas;
	Point __curDot;
	bool __died;

	static const int SNAKE_MOVEMENT_SPEED = 50;
	static const int SNAKE_DEATH_FADE_SPEED = 700;

	int __snakeMovementCounter;
	int __snakeDeathCounter;
	int __snakeFadeCounter;

	int __scores;
	bool __paused;
};

#endif
