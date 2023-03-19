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
#ifndef GAMEFORM_H_
#define GAMEFORM_H_

#include "BasicForm.h"
#include "SnakeBody.h"
#include <FGraphics.h>

using namespace Osp::Base::Runtime;
using namespace Osp::Graphics;

enum DotMode {
	MODE_NORMAL,
	MODE_LIVE,
	MODE_SHORTEN
};

class GameForm: public BasicForm, public ITimerEventListener, public ITouchEventListener {
public:
	GameForm(void);
	virtual ~GameForm(void);

private:
	void OnButtonHowToPlayClicked(const Control &src);
	void OnButtonPauseClicked(const Control &src);
	void OnButtonContinueClicked(const Control &src);
	void OnButtonBackToMenuClicked(const Control &src);
	void OnButtonExitYesClicked(const Control &src);
	void OnButtonExitNoClicked(const Control &src);

	void CheckIfHowToPlayWasShown(void);
	MovingDirection GetDirectionBasedOnPosition(MovingDirection curDir, int x, int y, int clickX, int clickY);
	bool UpdateSnake(void);
	void DrawSnake(void);
	void DrawDot(void);
	void DrawScores(void);
	void DrawLives(void);
	void DrawShade(void);
	void DrawFrame(void);
	void ResetSnake(void);

	virtual result Initialize(void);
	virtual result Terminate(void);

	virtual result OnGivenData(IList *pArgs, DataReceiveReason reason, int srcID);

	virtual result OnDraw(void);

	virtual void OnTimerExpired(Timer &timer);

	virtual void OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo);
	virtual void OnTouchLongPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) { }
	virtual void OnTouchReleased(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) { }
	virtual void OnTouchMoved(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) { }
	virtual void OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) { }
	virtual void OnTouchFocusOut(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) { }

	Button *__pContinueButton;
	Button *__pExitButton;
	Label *__pExitTextLabel;
	Button *__pExitYesButton;
	Button *__pExitNoButton;

	Label *__pHowToPlayLabel;
	Button *__pHowToPlayButton;

	SnakeBody *__pSnake;
	Timer *__pUpdateTimer;
	Canvas *__pCanvas;
	Point __curDot;
	bool __died;
	ByteBuffer *__pFontBuffer;

	static const int SNAKE_DEATH_FADE_SPEED = 550;
	int SNAKE_MOVEMENT_SPEED;

	int __snakeMovementCounter;
	int __snakeDeathCounter;
	int __snakeFadeCounter;
	int __dotFadeCounter;

	int __scores;
	int __lives;
	bool __paused;
	DotMode __curDotMode;

	bool __switchDir;
	bool __askingForRestart;
	int __stepsSinceBonus;
	bool __dotFadeSwitch;
	bool __setGame;
};

#endif
