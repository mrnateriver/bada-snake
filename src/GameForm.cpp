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
#include "GameForm.h"
#include "Utilities.h"
#include "HighscoreManager.h"
#include "GameSerializer.h"
#include <FSystem.h>
#include <FIo.h>

using namespace Osp::Base::Utility;
using namespace Osp::System;
using namespace Osp::Io;

GameForm::GameForm(void) {
	__pContinueButton = null;
	__pExitButton = null;
	__pExitTextLabel = null;
	__pExitYesButton = null;
	__pExitNoButton = null;

	__pHowToPlayLabel = null;
	__pHowToPlayButton = null;

	__pSnake = null;
	__pUpdateTimer = null;
	__pCanvas = null;
	__died = false;
	__pFontBuffer = null;

	__snakeMovementCounter = 0;
	__snakeDeathCounter = 0;
	__snakeFadeCounter = 0;
	__dotFadeCounter = 0;

	__scores = 0;
	__lives = 4;
	__paused = false;
	__curDotMode = MODE_NORMAL;

	__switchDir = true;
	__stepsSinceBonus = 0;
	__dotFadeSwitch = false;
	__setGame = true;

	SNAKE_MOVEMENT_SPEED = 100;
}

GameForm::~GameForm(void) {
	if (__pSnake) {
		delete __pSnake;
	}
	if (__pUpdateTimer) {
		__pUpdateTimer->Cancel();
		delete __pUpdateTimer;
	}
	if (__pCanvas) {
		delete __pCanvas;
	}
	if (__pFontBuffer) {
		delete __pFontBuffer;
	}
}

void GameForm::CheckIfHowToPlayWasShown(void) {
	AppRegistry *reg = Application::GetInstance()->GetAppRegistry();
	int flag = -1;
	if (IsFailed(reg->Get(L"HAS_SHOWN_HOW_TO_PLAY", flag)) || flag < 1) {
		__paused = true;

		//init labels and stuff
		String howToPlayText = Utilities::GetString(L"GAMEFORM_INSTRUCTIONS");
		int lines = Utilities::GetLinesForText(howToPlayText, 27, 386);

		__pHowToPlayLabel = new Label;
		result res = __pHowToPlayLabel->Construct(Rectangle(30, 330, 406, lines * 28), howToPlayText);
		if (IsFailed(res)) {
			AppLogDebug("[%s]: failed to construct instructions label", GetErrorMessage(res));

			delete __pHowToPlayLabel;
			__pHowToPlayLabel = null;

			__paused = false;

			return;
		}
		__pHowToPlayLabel->SetTextColor(Color::COLOR_WHITE);
		__pHowToPlayLabel->SetTextConfig(27, LABEL_TEXT_STYLE_NORMAL);
		__pHowToPlayLabel->SetTextHorizontalAlignment(ALIGNMENT_CENTER);
		__pHowToPlayLabel->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
		AddControl(*__pHowToPlayLabel);

		Bitmap *pressedBmp = Utilities::GetBitmapN(L"small_button_pressed.png");
		Bitmap *normalBmp = Utilities::GetBitmapN(L"small_button_normal.png");

		__pHowToPlayButton = new Button;
		res = __pHowToPlayButton->Construct(Rectangle(156, 350 + lines * 28, 168, 60), Utilities::GetString(L"GAMEFORM_INSTRUCTIONS_BUTTON_OK"));
		if (IsFailed(res)) {
			AppLogDebug("[%s]: failed to construct instructions button", GetErrorMessage(res));

			delete __pHowToPlayLabel;
			__pHowToPlayLabel = null;
			delete __pHowToPlayButton;
			__pHowToPlayButton = null;

			__paused = false;

			return;
		}
		__pHowToPlayButton->SetTextColor(Color::COLOR_WHITE);
		if (normalBmp) {
			__pHowToPlayButton->SetNormalBackgroundBitmap(*normalBmp);
		}
		if (pressedBmp) {
			__pHowToPlayButton->SetPressedBackgroundBitmap(*pressedBmp);
		}
		__pHowToPlayButton->SetActionId(HANDLER(GameForm::OnButtonHowToPlayClicked));
		__pHowToPlayButton->AddActionEventListener(*this);
		AddControl(*__pHowToPlayButton);

		if (normalBmp) {
			delete normalBmp;
		}
		if (pressedBmp) {
			delete pressedBmp;
		}

		reg->Add(L"HAS_SHOWN_HOW_TO_PLAY", 1);
	}
}

void GameForm::OnButtonHowToPlayClicked(const Control &src) {
	if (__pHowToPlayLabel) {
		RemoveControl(*__pHowToPlayLabel);
		__pHowToPlayLabel = null;
	}
	if (__pHowToPlayButton) {
		RemoveControl(*__pHowToPlayButton);
		__pHowToPlayButton = null;
	}
	__paused = false;
}

void GameForm::OnButtonPauseClicked(const Control &src) {
	if (!__paused) {
		__pContinueButton->SetShowState(true);
		__pExitButton->SetShowState(true);

		__paused = true;
	}
}

void GameForm::OnButtonContinueClicked(const Control &src) {
	if (__paused) {
		__pContinueButton->SetShowState(false);
		__pExitButton->SetShowState(false);

		__paused = false;
	}
}

void GameForm::OnButtonBackToMenuClicked(const Control &src) {
	if (__paused) {
		__pContinueButton->SetShowState(false);
		__pExitButton->SetShowState(false);

		__pExitTextLabel->SetShowState(true);
		__pExitYesButton->SetShowState(true);
		__pExitNoButton->SetShowState(true);
	}
}

void GameForm::OnButtonExitYesClicked(const Control &src) {
	if (!__askingForRestart) {
		__pUpdateTimer->Cancel();

		GameState state;
		state.snake = __pSnake;
		state.score = __scores;
		state.lives = __lives;
		state.speed = SNAKE_MOVEMENT_SPEED;
		state.dotX = __curDot.x;
		state.dotY = __curDot.y;
		state.dotMode = (int)__curDotMode;

		__pSnake = null;
		GameSerializer::GetInstance()->SetGame(state);

		__setGame = false;
		GetFormManager()->SwitchBack(null, REASON_DIALOG_CANCEL, true);
	} else {
		__pExitTextLabel->SetShowState(false);
		__pExitYesButton->SetShowState(false);
		__pExitNoButton->SetShowState(false);

		__scores = 0;
		__lives = 4;
		SNAKE_MOVEMENT_SPEED = 100;

		__pExitTextLabel->SetText(Utilities::GetString(L"GAMEFORM_EXIT_CONFIRMATION_TEXT"));
		__askingForRestart = false;

		ResetSnake();
		__paused = false;
	}
}

void GameForm::OnButtonExitNoClicked(const Control &src) {
	if (!__askingForRestart) {
		__pContinueButton->SetShowState(true);
		__pExitButton->SetShowState(true);

		__pExitTextLabel->SetShowState(false);
		__pExitYesButton->SetShowState(false);
		__pExitNoButton->SetShowState(false);
	} else {
		__setGame = false;

		int ind = -1;
		if (__scores > 0) {
			ind = HighscoreManager::GetInstance()->AddRecord(__scores);
		}

		ArrayList *pArgs = new ArrayList;
		pArgs->Construct(1);
		pArgs->Add(*(new Integer(ind)));

		GetFormManager()->SwitchBack(pArgs, REASON_DIALOG_SUCCESS, true);

		pArgs->RemoveAll(true);
		delete pArgs;
	}
}

result GameForm::Initialize(void) {
	result res = Form::Construct(null);
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct parent form", GetErrorMessage(res));
		return res;
	}
	AddTouchEventListener(*this);

	Bitmap *livesBmp = Utilities::GetBitmapN(L"live_icon.png");

	Label *livesIcon = new Label;
	res = livesIcon->Construct(Rectangle(352, 10, 30, 30), L"");
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct lives icon", GetErrorMessage(res));
		return res;
	}
	if (livesBmp) {
		livesIcon->SetBackgroundBitmap(*livesBmp);
		delete livesBmp;
	}
	AddControl(*livesIcon);

	Bitmap *pauseBmp = Utilities::GetBitmapN(L"pause_button.png");

	Button *pause = new Button;
	res = pause->Construct(Rectangle(430, 0, 40, 40), L"");
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct pause button", GetErrorMessage(res));
		return res;
	}
	if (pauseBmp) {
		pause->SetNormalBackgroundBitmap(*pauseBmp);
		pause->SetPressedBackgroundBitmap(*pauseBmp);

		delete pauseBmp;
	}
	pause->SetActionId(HANDLER(GameForm::OnButtonPauseClicked));
	pause->AddActionEventListener(*this);
	AddControl(*pause);

	Bitmap *pressedBmp = Utilities::GetBitmapN(L"button_pressed.png");
	Bitmap *normalBmp = Utilities::GetBitmapN(L"button_normal.png");

	__pContinueButton = new Button;
	res = __pContinueButton->Construct(Rectangle(70, 335, 346, 60), Utilities::GetString(L"MAINMENU_BUTTON_CONTINUE"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct continue button", GetErrorMessage(res));
		return res;
	}
	__pContinueButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pContinueButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pContinueButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pContinueButton->SetActionId(HANDLER(GameForm::OnButtonContinueClicked));
	__pContinueButton->AddActionEventListener(*this);
	__pContinueButton->SetShowState(false);
	AddControl(*__pContinueButton);

	__pExitButton = new Button;
	res = __pExitButton->Construct(Rectangle(70, 405, 346, 60), Utilities::GetString(L"GAMEFORM_BUTTON_BACK"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct exit button", GetErrorMessage(res));
		return res;
	}
	__pExitButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pExitButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pExitButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pExitButton->SetActionId(HANDLER(GameForm::OnButtonBackToMenuClicked));
	__pExitButton->AddActionEventListener(*this);
	__pExitButton->SetShowState(false);
	AddControl(*__pExitButton);

	if (normalBmp) {
		delete normalBmp;
	}
	if (pressedBmp) {
		delete pressedBmp;
	}

	String exitConfText = Utilities::GetString(L"GAMEFORM_EXIT_CONFIRMATION_TEXT");
	int lines = Utilities::GetLinesForText(exitConfText, 27, 326);

	__pExitTextLabel = new Label;
	res = __pExitTextLabel->Construct(Rectangle(50, 330, 386, lines * 28), exitConfText);
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct exit confirmation label", GetErrorMessage(res));
		return res;
	}
	__pExitTextLabel->SetTextColor(Color::COLOR_WHITE);
	__pExitTextLabel->SetTextConfig(27, LABEL_TEXT_STYLE_NORMAL);
	__pExitTextLabel->SetTextHorizontalAlignment(ALIGNMENT_CENTER);
	__pExitTextLabel->SetTextVerticalAlignment(ALIGNMENT_MIDDLE);
	__pExitTextLabel->SetShowState(false);
	AddControl(*__pExitTextLabel);

	pressedBmp = Utilities::GetBitmapN(L"small_button_pressed.png");
	normalBmp = Utilities::GetBitmapN(L"small_button_normal.png");

	__pExitYesButton = new Button;
	res = __pExitYesButton->Construct(Rectangle(50, 350 + lines * 28, 168, 60), Utilities::GetString(L"GAMEFORM_EXIT_BUTTON_YES"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct exit yes button", GetErrorMessage(res));
		return res;
	}
	__pExitYesButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pExitYesButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pExitYesButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pExitYesButton->SetActionId(HANDLER(GameForm::OnButtonExitYesClicked));
	__pExitYesButton->AddActionEventListener(*this);
	__pExitYesButton->SetShowState(false);
	AddControl(*__pExitYesButton);

	__pExitNoButton = new Button;
	res = __pExitNoButton->Construct(Rectangle(268, 350 + lines * 28, 168, 60), Utilities::GetString(L"GAMEFORM_EXIT_BUTTON_NO"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct exit no button", GetErrorMessage(res));
		return res;
	}
	__pExitNoButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pExitNoButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pExitNoButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pExitNoButton->SetActionId(HANDLER(GameForm::OnButtonExitNoClicked));
	__pExitNoButton->AddActionEventListener(*this);
	__pExitNoButton->SetShowState(false);
	AddControl(*__pExitNoButton);

	if (normalBmp) {
		delete normalBmp;
	}
	if (pressedBmp) {
		delete pressedBmp;
	}

	__pCanvas = new Canvas();
	res = __pCanvas->Construct(GetBounds());
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct backbuffer canvas", GetErrorMessage(res));
		return res;
	}
	//__pCanvas->SetBackgroundColor(Color::COLOR_BLACK);
	__pCanvas->SetBackgroundColor(Color(0x111111, false));
	__pCanvas->SetForegroundColor(Color::COLOR_WHITE);

	Font font;
	File *file = new File;
	if (!IsFailed(file->Construct(L"/Res/fnt1282.ttf", L"r", false))) {
		__pFontBuffer = new ByteBuffer;
		__pFontBuffer->Construct(33 * 1024);

		if (!IsFailed(file->Read(*__pFontBuffer))) {
			font.Construct(*__pFontBuffer, FONT_STYLE_PLAIN, 39);
		} else {
			delete __pFontBuffer;
			__pFontBuffer = null;

			font.Construct(FONT_STYLE_PLAIN, 39);
		}
	} else {
		font.Construct(FONT_STYLE_PLAIN, 39);
	}
	__pCanvas->SetFont(font);

	__pUpdateTimer = new Timer;
	res = __pUpdateTimer->Construct(*this);
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct update timer", GetErrorMessage(res));
		return res;
	}

	TimeSpan timeSpan(0, 0, 0);
	SystemTime::GetUptime(timeSpan);
	Math::Srand((int)(35 * (int)this + timeSpan.GetMilliseconds()));

	return E_SUCCESS;
}

result GameForm::Terminate(void) {
	if (__setGame) {
		__pUpdateTimer->Cancel();

		GameState state;
		state.snake = __pSnake;
		state.score = __scores;
		state.lives = __lives;
		state.speed = SNAKE_MOVEMENT_SPEED;
		state.dotX = __curDot.x;
		state.dotY = __curDot.y;
		state.dotMode = (int)__curDotMode;

		__pSnake = null;
		GameSerializer::GetInstance()->SetGame(state);
	}
	return E_SUCCESS;
}

result GameForm::OnGivenData(IList *pArgs, DataReceiveReason reason, int srcID) {
	if (reason == REASON_APP_WENT_BACKGROUND) {
		OnButtonPauseClicked(*this);
	} else if (reason == REASON_NONE) {
		if (pArgs && pArgs->GetCount() > 0) {
			Boolean *cont = static_cast<Boolean *>(pArgs->GetAt(0));
			if (cont->ToBool() && GameSerializer::GetInstance()->HasGame()) {
				GameState state = GameSerializer::GetInstance()->GetGame();

				__scores = state.score;
				__lives = state.lives;
				SNAKE_MOVEMENT_SPEED = state.speed;

				__curDot = Point(state.dotX, state.dotY);
				__curDotMode = (DotMode)state.dotMode;

				__pSnake = state.snake;

				__pUpdateTimer->Start(50);

				GameSerializer::GetInstance()->RemoveGame(false);

				CheckIfHowToPlayWasShown();
				return E_SUCCESS;
			}
		}
		ResetSnake();
		__pUpdateTimer->Start(50);

		GameSerializer::GetInstance()->RemoveGame(true);
		CheckIfHowToPlayWasShown();
	}
	return E_SUCCESS;
}

result GameForm::OnDraw(void) {
	Canvas* pCanvas = GetCanvasN();
	if (pCanvas) {
		pCanvas->Copy(Point(GetClientAreaBounds().x, GetClientAreaBounds().y), *__pCanvas, GetClientAreaBounds());
		delete pCanvas;
	}
	__pCanvas->Clear();
	return E_SUCCESS;
}

void GameForm::ResetSnake(void) {
	if (__pSnake) {
		delete __pSnake;
		__pSnake = null;
	}
	if (__lives <= 0) {
		__paused = true;

		__pExitTextLabel->SetText(Utilities::GetString(L"GAMEFORM_DIED_RESTART_QUESTION"));

		__pExitTextLabel->SetShowState(true);
		__pExitYesButton->SetShowState(true);
		__pExitNoButton->SetShowState(true);

		__askingForRestart = true;
		return;
	}

	__lives--;

	__pSnake = new SnakeBody;
	__pSnake->Construct(Math::Rand() % 20 + 5, Math::Rand() % 40 + 5, (MovingDirection)(Math::Rand() % 3), 3);

	__curDot = Point(Math::Rand() % 30, Math::Rand() % 47 + 3);
	__curDotMode = MODE_NORMAL;

	__died = false;
	__snakeFadeCounter = 0;

	__snakeDeathCounter = 0;
	__snakeMovementCounter = 0;
	__dotFadeCounter = 0;

	__switchDir = true;
}

bool GameForm::UpdateSnake(void) {
	if (__pSnake) {
		__pSnake->MoveBySection();
		__switchDir = true;
		if (__pSnake->CoversCoords(__curDot.x, __curDot.y)) {
			if (__curDotMode == MODE_NORMAL) {
				__pSnake->AddTailSection();
			} else if (__curDotMode == MODE_LIVE) {
				__lives++;
				__pSnake->AddTailSection();
			} else {
				__pSnake->RemoveTailSections((int)((float)__pSnake->GetNumberOfSections() / 3));
			}

			__curDot = Point(Math::Rand() % 30, Math::Rand() % 47 + 3);

			if (__scores >= 1000 && __stepsSinceBonus >= 5) {
				int chance = Math::Rand() % 100;
				if (chance > 8) {
					__curDotMode = MODE_NORMAL;
				} else if (chance > 4) {
					__curDotMode = MODE_SHORTEN;
					__stepsSinceBonus = 0;
				} else {
					__curDotMode = MODE_LIVE;
					__stepsSinceBonus = 0;
				}
			} else {
				__curDotMode = MODE_NORMAL;
			}

			int scoreBonus = (__pSnake->GetNumberOfSections() % 5 == 0 ? 2 : 1) * 10;
			__scores += scoreBonus * ((int)((float)(100 - SNAKE_MOVEMENT_SPEED) / 10) + 1);

			if (__scores >= 25000) {
				SNAKE_MOVEMENT_SPEED = 25;
			} else if (__scores >= 10000) {
				SNAKE_MOVEMENT_SPEED = 50;
			} else if (__scores >= 500) {
				SNAKE_MOVEMENT_SPEED = 75;
			}

			__stepsSinceBonus++;
		}
		if (__pSnake->IntersectsItself() || __pSnake->GetHeadSectionX() < 0 || __pSnake->GetHeadSectionX() > 29
			|| __pSnake->GetHeadSectionY() < 0 || __pSnake->GetHeadSectionY() > 49) {
			return true;
		}
	}
	return false;
}

void GameForm::DrawSnake(void) {
	if (__pSnake) {
		IEnumeratorT<SnakeSection *> *pEnum = __pSnake->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			SnakeSection *sect = null;
			pEnum->GetCurrent(sect);
			if (sect) {
				if (sect->GetSectionX() >= 0 && sect->GetSectionX() <= 29
					&& sect->GetSectionY() >= 0 && sect->GetSectionY() <= 49) {
					__pCanvas->FillEllipse(Color::COLOR_WHITE, Rectangle(sect->GetSectionX() * 16, sect->GetSectionY() * 16, 16, 16));
				}
			}
		}
		delete pEnum;
	}
}

void GameForm::DrawDot(void) {
	int alpha = 255;
	if (__dotFadeSwitch) {
		alpha = 160;
	}
	if (__curDotMode == MODE_NORMAL) {
		__pCanvas->FillEllipse(Color(255,255,255,alpha) /*Color::COLOR_WHITE*/, Rectangle(__curDot.x * 16, __curDot.y * 16, 16, 16));
	} else if (__curDotMode == MODE_LIVE) {
		__pCanvas->FillEllipse(Color(255,0,0,alpha) /*Color::COLOR_RED*/, Rectangle(__curDot.x * 16, __curDot.y * 16, 16, 16));
	} else {
		__pCanvas->FillEllipse(Color(0,255,0,alpha) /*Color::COLOR_GREEN*/, Rectangle(__curDot.x * 16, __curDot.y * 16, 16, 16));
	}
}

void GameForm::DrawScores(void) {
	__pCanvas->DrawText(Point(10,7), Integer::ToString(__scores));
}

void GameForm::DrawLives(void) {
	__pCanvas->DrawText(Point(388,7), Integer::ToString(__lives));
}

void GameForm::DrawShade(void) {
	//__pCanvas->FillRectangle(Color(10, 10, 10, 160), Rectangle(0, 0, 480, 800));
	__pCanvas->FillRectangle(Color(0, 0, 0, 150), Rectangle(0, 0, 480, 800));
}

void GameForm::DrawFrame(void) {
	__pCanvas->SetForegroundColor(Color(0x1c1c1c, false));
	__pCanvas->DrawRectangle(Rectangle(0,0,480,800));
	__pCanvas->SetForegroundColor(Color::COLOR_WHITE);
}

void GameForm::OnTimerExpired(Timer &timer) {
	if (!__died && !__paused) {
		__snakeMovementCounter += 25;
		if (__snakeMovementCounter >= SNAKE_MOVEMENT_SPEED) {
			__died = UpdateSnake();
			__snakeMovementCounter = 0;
		}
	}

	if (!__paused) {
		__dotFadeCounter += 25;
		if (__dotFadeCounter >= SNAKE_DEATH_FADE_SPEED) {
			__dotFadeSwitch = !__dotFadeSwitch;
			__dotFadeCounter = 0;
		}

		//DrawFrame();
	}

	DrawDot();
	if (__died && !__paused) {
		if (__snakeFadeCounter == 0 || __snakeFadeCounter == 2 || __snakeFadeCounter == 4) {
			DrawSnake();
		}

		__snakeDeathCounter += 25;
		if (__snakeDeathCounter >= SNAKE_DEATH_FADE_SPEED) {
			__snakeFadeCounter++;
			if (__snakeFadeCounter > 4) {
				ResetSnake();
			}
			__snakeDeathCounter = 0;
		}
	} else {
		DrawSnake();
	}

	if (__paused) {
		DrawShade();
	}
	DrawScores();
	DrawLives();

	RequestRedraw(true);
	__pUpdateTimer->Start(25);
}

MovingDirection GameForm::GetDirectionBasedOnPosition(MovingDirection curDir, int x, int y, int clickX, int clickY) {
	if (curDir == DIRECTION_UP || curDir == DIRECTION_DOWN) {
		if (clickX >= x) {
			return DIRECTION_RIGHT;
		} else {
			return DIRECTION_LEFT;
		}
	} else if (curDir == DIRECTION_LEFT || curDir == DIRECTION_RIGHT) {
		if (clickY <= y) {
			return DIRECTION_UP;
		} else {
			return DIRECTION_DOWN;
		}
	}
	return DIRECTION_UP;
}

void GameForm::OnTouchPressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) {
	if (__pSnake && __switchDir && !__paused) {
		SnakeSection *sect = __pSnake->GetHeadSection();
		if (sect) {
			sect->SetDirection(GetDirectionBasedOnPosition(sect->GetDirection(), sect->GetSectionX() * 16, sect->GetSectionY() * 16, currentPosition.x, currentPosition.y));
			__switchDir = false;
		}
	}
}

void GameForm::OnTouchDoublePressed(const Control &source, const Point &currentPosition, const TouchEventInfo &touchInfo) {
	OnTouchPressed(source, currentPosition, touchInfo);
}
