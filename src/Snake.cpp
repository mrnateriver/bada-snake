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
#include "Snake.h"
#include "MainMenu.h"
#include "Utilities.h"
#include "HighscoreManager.h"
#include "GameSerializer.h"

Snake::Snake() {
}

Snake::~Snake() {
}

Application *Snake::CreateInstance(void) {
	return new Snake();
}

bool Snake::OnAppInitializing(AppRegistry &appRegistry) {
	FormManager::Initialize(GetAppFrame()->GetFrame());
	Utilities::Initialize();
	result res = HighscoreManager::Initialize();
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to initialize highscore manager", GetErrorMessage(res));
		return false;
	}
	res = GameSerializer::Initialize();
	if (IsFailed(res)) {
		AppLogDebug("[%s]: failed to initialize game serializer", GetErrorMessage(res));
		return false;
	}

	res = FormManager::GetInstance()->SwitchToForm(new MainMenu, null);
	if (IsFailed(res)) {
		return false;
	} else return true;

	return true;
}

bool Snake::OnAppTerminating(AppRegistry &appRegistry, bool forcedTermination) {
	FormManager::GetInstance()->NotifyCurrentForm(REASON_APP_TERMINATING);

	FormManager::RemoveInstance();
	Utilities::RemoveInstance();

	HighscoreManager::GetInstance()->Serialize();
	HighscoreManager::RemoveInstance();

	GameSerializer::GetInstance()->Serialize();
	GameSerializer::RemoveInstance();

	return true;
}

void Snake::OnForeground(void) {
	FormManager::GetInstance()->NotifyCurrentForm(REASON_APP_WENT_FOREGROUND);
}

void Snake::OnBackground(void) {
	FormManager::GetInstance()->NotifyCurrentForm(REASON_APP_WENT_BACKGROUND);
}

void Snake::OnLowMemory(void) {
}

void Snake::OnBatteryLevelChanged(BatteryLevel batteryLevel) {
}

void Snake::OnScreenOn(void) {
}

void Snake::OnScreenOff(void) {
}
