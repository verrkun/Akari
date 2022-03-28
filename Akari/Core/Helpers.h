
#pragma once

#include "../Engine/Exports/paf/View.h"

class Helpers
{
public:
	Helpers();

	void OnUpdate();

	void TakeScreenshot();
	bool ScreenshotsBinds();

private:
	void MonitorGameState();

public:
	paf::View* system_plugin{};
	paf::View* game_plugin{};
	paf::View* game_ext_plugin{};
	paf::PhWidget* page_autooff_guide{};

	bool m_IsHen{};

	bool m_StateGameRunning{};
	bool m_StateGameJustLaunched{};

private:
	unsigned int m_ScreenshotLastTime{};
};

extern Helpers g_Helpers;