
#include "Helpers.h"
#include "Configuration.h"
#include "../Engine/Input.h"
#include "../Engine/Exports/vsh/vshcommon.h"
#include "../Engine/Exports/vsh/vshmain.h"
#include "../Utilities/System.h"
#include <vsh/stdc.h>
#include <vsh/interfaces/system_plugin.h>

Helpers g_Helpers;

Helpers::Helpers()
{
	m_IsHen = Syscall::sys_mapi_is_hen();
}

void Helpers::OnUpdate()
{
	system_plugin = paf::View::Find("system_plugin");
	page_autooff_guide = system_plugin ? system_plugin->FindWidget("page_autooff_guide") : nullptr;

	if (g_Config->screenshots.enableBinds && ScreenshotsBinds())
		TakeScreenshot();
}

void Helpers::TakeScreenshot()
{
	unsigned int timeNow = Timers::GetTimeNow();

	if ((timeNow - m_ScreenshotLastTime) < 500)
		return;

	m_ScreenshotLastTime = timeNow;

	if (vsh::GetCooperationMode() == vsh::CooperationMode::XMB)
	{
		if (!system_plugin)
			return;

		system_plugin_interface* system_interface = system_plugin->GetInterface<system_plugin_interface*>(1);
		if (!system_interface)
			return;

		std::string screenshotPath = File::GetCurrentDir() + "Screenshots";
		stdc::mkdir(screenshotPath.c_str(), 40777);

		time_t rawtime;
		stdc::time(&rawtime);
		tm* timeinfo = stdc::localtime(&rawtime);

		char datestr[100];
		stdc::strftime(datestr, 100, "%Y-%m-%d_%H-%M-%S", timeinfo);

		std::string screenshotName = screenshotPath + "/" + datestr + ".bmp";

		system_interface->saveBMP(screenshotName.c_str());

		wchar_t buffer[130];
		stdc::swprintf(buffer, 0xA0, L"Screenshot saved as: %s", screenshotName.c_str());
		vsh::ShowButtonNavigationMessage(buffer);
	}
	else
		vsh::ShowButtonNavigationMessage(L"Not supported in game");
}

bool Helpers::ScreenshotsBinds()
{
	switch (g_Config->screenshots.binds)
	{
	case 0: return g_Input.IsButtonBinds(CInput::BUTTON_L1, CInput::BUTTON_PAD_UP);
	case 1: return g_Input.IsButtonBinds(CInput::BUTTON_L1, CInput::BUTTON_PAD_DOWN);
	case 2: return g_Input.IsButtonBinds(CInput::BUTTON_L1, CInput::BUTTON_PAD_LEFT);
	case 3: return g_Input.IsButtonBinds(CInput::BUTTON_L1, CInput::BUTTON_PAD_RIGHT);
	default: return false;
	}
}