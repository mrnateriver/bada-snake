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
#include "MainMenu.h"
#include "GameForm.h"
#include "Utilities.h"
#include "HighscoreManager.h"
#include "GameSerializer.h"
#include <FSystem.h>
#include <FIo.h>

using namespace Osp::Base::Utility;
using namespace Osp::System;
using namespace Osp::Io;

MainMenu::MainMenu(void) {
	__pHeaderLabel = null;
	__pContinueButton = null;
	__pNewGameButton = null;
	__pHighscoresButton = null;
	__pExitButton = null;
	__pScoresBackButton = null;
	__pScoresList = null;
	__pScoresListFormat = null;

	__pSnake = null;
	__pUpdateTimer = null;
	__pCanvas = null;
	__died = false;

	__snakeMovementCounter = 0;
	__snakeDeathCounter = 0;
	__snakeFadeCounter = 0;

	__scores = 0;
	__paused = false;
}

MainMenu::~MainMenu(void) {
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
}

void MainMenu::OnButtonExitClicked(const Control &src) {
	Application::GetInstance()->Terminate();
}

void MainMenu::OnButtonContinueClicked(const Control &src) {
	__paused = true;
	__pUpdateTimer->Cancel();

	ArrayList *pArgs = new ArrayList;
	pArgs->Construct(1);
	pArgs->Add(*(new Boolean(true)));

	GetFormManager()->SwitchToForm(new GameForm, pArgs);

	pArgs->RemoveAll(true);
	delete pArgs;
}

void MainMenu::OnButtonNewGameClicked(const Control &src) {
	__paused = true;
	__pUpdateTimer->Cancel();

	ArrayList *pArgs = new ArrayList;
	pArgs->Construct(1);
	pArgs->Add(*(new Boolean(false)));

	GetFormManager()->SwitchToForm(new GameForm, pArgs);

	pArgs->RemoveAll(true);
	delete pArgs;
}

void MainMenu::OnButtonHighscoresClicked(const Control &src) {
	ArrayListT<HighscoreRecord *> *records = HighscoreManager::GetInstance()->GetRecords();
	if (records) {
		__pScoresList->RemoveAllItems();
		DateTimeFormatter *pFormatter = DateTimeFormatter::CreateDateFormatterN(DATE_TIME_STYLE_SHORT);

		Bitmap *empty = Utilities::GetBitmapN(L"empty.png");

		IEnumeratorT<HighscoreRecord *> *pEnum = records->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			HighscoreRecord *rec = null;
			pEnum->GetCurrent(rec);
			if (rec) {
				CustomListItem *item = new CustomListItem;
				item->Construct(54);
				item->SetItemFormat(*__pScoresListFormat);

				String dt;
				pFormatter->Format(rec->recordDate, dt);

				item->SetElement(SCORES_LIST_ELEMENT_DATE, dt);
				item->SetElement(SCORES_LIST_ELEMENT_SCORE, Integer::ToString(rec->score));
				if (empty) {
					item->SetNormalItemBackgroundBitmap(*empty);
					item->SetFocusedItemBackgroundBitmap(*empty);
				}

				__pScoresList->AddItem(*item);
			}
		}
		delete pEnum;
		delete pFormatter;
		if (empty) {
			delete empty;
		}

		__pContinueButton->SetShowState(false);
		__pNewGameButton->SetShowState(false);
		__pHighscoresButton->SetShowState(false);
		__pExitButton->SetShowState(false);

		__pScoresList->SetShowState(true);
		__pScoresBackButton->SetShowState(true);
	}
}

void MainMenu::OnButtonScoresBackClicked(const Control &src) {
	__pContinueButton->SetShowState(true);
	__pNewGameButton->SetShowState(true);
	__pHighscoresButton->SetShowState(true);
	__pExitButton->SetShowState(true);

	__pScoresList->SetShowState(false);
	__pScoresBackButton->SetShowState(false);
}

result MainMenu::Initialize(void) {
	result res = Form::Construct(null);
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct parent form", GetErrorMessage(res));
		return res;
	}

	__pHeaderLabel = new Label;
	res = __pHeaderLabel->Construct(Rectangle(0, 0, 480, 180), L" ");
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct header label", GetErrorMessage(res));
		return res;
	}
	String lang = Utilities::GetString(L"LANG_IDENT");
	Bitmap *headBmp = Utilities::GetBitmapN(lang.Equals(L"ru", false) ? L"main_menu_header_ru.png" : L"main_menu_header_en.png");
	if (headBmp) {
		__pHeaderLabel->SetBackgroundBitmap(*headBmp);
		delete headBmp;
	}
	AddControl(*__pHeaderLabel);

	Bitmap *pressedBmp = Utilities::GetBitmapN(L"button_pressed.png");
	Bitmap *normalBmp = Utilities::GetBitmapN(L"button_normal.png");

	__pContinueButton = new Button;
	res = __pContinueButton->Construct(Rectangle(70, 315, 346, 60), Utilities::GetString(L"MAINMENU_BUTTON_CONTINUE"));
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
	__pContinueButton->SetActionId(HANDLER(MainMenu::OnButtonContinueClicked));
	__pContinueButton->AddActionEventListener(*this);
	AddControl(*__pContinueButton);

	__pNewGameButton = new Button;
	res = __pNewGameButton->Construct(Rectangle(70, 385, 346, 60), Utilities::GetString(L"MAINMENU_BUTTON_NEWGAME"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct new game button", GetErrorMessage(res));
		return res;
	}
	__pNewGameButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pNewGameButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pNewGameButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pNewGameButton->SetActionId(HANDLER(MainMenu::OnButtonNewGameClicked));
	__pNewGameButton->AddActionEventListener(*this);
	AddControl(*__pNewGameButton);

	__pHighscoresButton = new Button;
	res = __pHighscoresButton->Construct(Rectangle(70, 455, 346, 60), Utilities::GetString(L"MAINMENU_BUTTON_HIGHSCORES"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct highscores button", GetErrorMessage(res));
		return res;
	}
	__pHighscoresButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pHighscoresButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pHighscoresButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pHighscoresButton->SetActionId(HANDLER(MainMenu::OnButtonHighscoresClicked));
	__pHighscoresButton->AddActionEventListener(*this);
	AddControl(*__pHighscoresButton);

	__pExitButton = new Button;
	res = __pExitButton->Construct(Rectangle(70, 525, 346, 60), Utilities::GetString(L"MAINMENU_BUTTON_EXIT"));
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
	__pExitButton->SetActionId(HANDLER(MainMenu::OnButtonExitClicked));
	__pExitButton->AddActionEventListener(*this);
	AddControl(*__pExitButton);

	__pScoresBackButton = new Button;
	res = __pScoresBackButton->Construct(Rectangle(70, 730, 346, 60), Utilities::GetString(L"MAINMENU_BUTTON_SCORES_BACK"));
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct scores back button", GetErrorMessage(res));
		return res;
	}
	__pScoresBackButton->SetTextColor(Color::COLOR_WHITE);
	if (normalBmp) {
		__pScoresBackButton->SetNormalBackgroundBitmap(*normalBmp);
	}
	if (pressedBmp) {
		__pScoresBackButton->SetPressedBackgroundBitmap(*pressedBmp);
	}
	__pScoresBackButton->SetActionId(HANDLER(MainMenu::OnButtonScoresBackClicked));
	__pScoresBackButton->AddActionEventListener(*this);
	__pScoresBackButton->SetShowState(false);
	AddControl(*__pScoresBackButton);

	__pScoresList = new CustomList;
	res = __pScoresList->Construct(Rectangle(20, 180, 440, 540), CUSTOM_LIST_STYLE_NORMAL, false);
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to construct scores list", GetErrorMessage(res));
		return res;
	}
	__pScoresList->SetTextOfEmptyList(L" ");
	__pScoresList->SetShowState(false);
	AddControl(*__pScoresList);

	__pScoresListFormat = new CustomListItemFormat;
	__pScoresListFormat->Construct();
	if (lang.Equals(L"ru", false)) {
		__pScoresListFormat->AddElement(SCORES_LIST_ELEMENT_DATE, Rectangle(339, 10, 220, 44), 30);
		__pScoresListFormat->AddElement(SCORES_LIST_ELEMENT_SCORE, Rectangle(5, 5, 334, 44), 44);
	} else {
		__pScoresListFormat->AddElement(SCORES_LIST_ELEMENT_DATE, Rectangle(311, 10, 220, 44), 30);
		__pScoresListFormat->AddElement(SCORES_LIST_ELEMENT_SCORE, Rectangle(5, 5, 307, 44), 44);
	}

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
	__pCanvas->SetBackgroundColor(Color::COLOR_BLACK);
	__pCanvas->SetForegroundColor(Color::COLOR_WHITE);

	Font font;
	font.Construct(FONT_STYLE_PLAIN, 39);
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

	ResetSnake();
	return E_SUCCESS;
}

result MainMenu::Terminate(void) {
	return E_SUCCESS;
}

result MainMenu::OnGivenData(IList *pArgs, DataReceiveReason reason, int srcID) {
	if (reason == REASON_APP_WENT_BACKGROUND) {
		__paused = true;
		__pUpdateTimer->Cancel();
	} else if (reason != REASON_APP_TERMINATING) {
		__paused = false;
		__pUpdateTimer->Start(50);

		__pContinueButton->SetEnabled(GameSerializer::GetInstance()->HasGame());

		if (reason == REASON_DIALOG_SUCCESS) {
			if (pArgs && pArgs->GetCount() > 0) {
				Integer *index = static_cast<Integer *>(pArgs->GetAt(0));
				int scIndex = index->ToInt();
				if (scIndex >= 0) {
					ArrayListT<HighscoreRecord *> *records = HighscoreManager::GetInstance()->GetRecords();
					if (records) {
						__pScoresList->RemoveAllItems();
						DateTimeFormatter *pFormatter = DateTimeFormatter::CreateDateFormatterN(DATE_TIME_STYLE_SHORT);

						Bitmap *empty = Utilities::GetBitmapN(L"empty.png");

						int ind = 0;
						IEnumeratorT<HighscoreRecord *> *pEnum = records->GetEnumeratorN();
						while (!IsFailed(pEnum->MoveNext())) {
							HighscoreRecord *rec = null;
							pEnum->GetCurrent(rec);
							if (rec) {
								CustomListItem *item = new CustomListItem;
								item->Construct(54);
								item->SetItemFormat(*__pScoresListFormat);

								String dt;
								pFormatter->Format(rec->recordDate, dt);

								item->SetElement(SCORES_LIST_ELEMENT_DATE, dt);
								item->SetElement(SCORES_LIST_ELEMENT_SCORE, Integer::ToString(rec->score));
								if (empty) {
									item->SetNormalItemBackgroundBitmap(*empty);
									item->SetFocusedItemBackgroundBitmap(*empty);
								}

								if (ind == scIndex) {
									item->SetValue(100);
								}

								__pScoresList->AddItem(*item);
								ind++;
							}
						}
						delete pEnum;
						delete pFormatter;
						if (empty) {
							delete empty;
						}

						__pContinueButton->SetShowState(false);
						__pNewGameButton->SetShowState(false);
						__pHighscoresButton->SetShowState(false);
						__pExitButton->SetShowState(false);

						__pScoresList->SetShowState(true);
						__pScoresBackButton->SetShowState(true);
					}
				}
			}
		}
	} else {
		AppLogDebug("SHIT HAPPENS!");
	}
	return E_SUCCESS;
}

result MainMenu::OnDraw(void) {
	Canvas* pCanvas = GetCanvasN();
	if (pCanvas) {
		pCanvas->Copy(Point(GetClientAreaBounds().x, GetClientAreaBounds().y), *__pCanvas, GetClientAreaBounds());
		delete pCanvas;
	}
	__pCanvas->Clear();
	return E_SUCCESS;
}

void MainMenu::ResetSnake(void) {
	if (__pSnake) {
		delete __pSnake;
	}
	__pSnake = new AutoSnake;
	__pSnake->Construct(Math::Rand() % 30, Math::Rand() % 50, (MovingDirection)(Math::Rand() % 3), Math::Rand() % 3 + 1);

	__curDot = Point(Math::Rand() % 30, Math::Rand() % 50);

	__died = false;
	__snakeFadeCounter = 0;

	__snakeDeathCounter = 0;
	__snakeMovementCounter = 0;

	__scores = 0;
}

bool MainMenu::UpdateSnake(void) {
	if (__pSnake) {
		__pSnake->MoveBySection(__curDot.x, __curDot.y);
		if (__pSnake->CoversCoords(__curDot.x, __curDot.y)) {
			__pSnake->AddTailSection();
			__curDot = Point(Math::Rand() % 30, Math::Rand() % 50);

			int scoreBonus = (__pSnake->GetNumberOfSections() % 5 == 0 ? 2 : 1) * 10;
			__scores += scoreBonus;
		}
		if (__pSnake->IntersectsItself() || __pSnake->GetHeadSectionX() < 0 || __pSnake->GetHeadSectionX() > 30
			|| __pSnake->GetHeadSectionY() < 0 || __pSnake->GetHeadSectionY() > 50) {
			return true;
		}
	}
	return false;
}

void MainMenu::DrawSnake(void) {
	if (__pSnake) {
		IEnumeratorT<SnakeSection *> *pEnum = __pSnake->GetEnumeratorN();
		while (!IsFailed(pEnum->MoveNext())) {
			SnakeSection *sect = null;
			pEnum->GetCurrent(sect);
			if (sect) {
				__pCanvas->FillEllipse(Color::COLOR_WHITE, Rectangle(sect->GetSectionX() * 16, sect->GetSectionY() * 16, 16, 16));
			}
		}
		delete pEnum;
	}
}

void MainMenu::DrawDot(void) {
	__pCanvas->FillEllipse(Color::COLOR_WHITE, Rectangle(__curDot.x * 16, __curDot.y * 16, 16, 16));
}

void MainMenu::DrawScores(void) {
	__pCanvas->DrawText(Point(10,760), Integer::ToString(__scores));
}

void MainMenu::DrawShade(void) {
	__pCanvas->FillRectangle(Color(0, 0, 0, 180), Rectangle(0, 0, 480, 800));
}

void MainMenu::OnTimerExpired(Timer &timer) {
	if (!__paused) {
		if (!__died) {
			__snakeMovementCounter += 50;
			if (__snakeMovementCounter >= SNAKE_MOVEMENT_SPEED) {
				__died = UpdateSnake();
				__snakeMovementCounter = 0;
			}
		}

		//DrawScores();
		DrawDot();
		if (__died) {
			if (__snakeFadeCounter == 0 || __snakeFadeCounter == 2 || __snakeFadeCounter == 4) {
				DrawSnake();
			}

			__snakeDeathCounter += 50;
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

		DrawShade();
		RequestRedraw(true);
	}
	__pUpdateTimer->Start(50);
}
