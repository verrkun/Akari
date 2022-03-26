
#pragma once

#include "../Engine/Exports/paf/View.h"

class Helpers
{
public:
	Helpers();

	void OnUpdate();

	void TakeScreenshot();
	bool ScreenshotsBinds();

public:
	paf::View* system_plugin{};
	paf::PhWidget* page_autooff_guide{};

	bool m_IsHen{};

private:
	unsigned int m_ScreenshotLastTime{};
};

extern Helpers g_Helpers;