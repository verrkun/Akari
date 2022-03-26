
#include "vshcommon.h"
#include "../paf/View.h"
#include <vsh/vshcommon.h>

namespace vsh
{
	void ShowButtonNavigationMessage(const std::wstring& message)
	{
		paf::View* system_plugin = paf::View::Find("system_plugin");
		if (!system_plugin)
			return;

		paf::PhWidget* page_autooff_guide = system_plugin->FindWidget("page_autooff_guide");
		if (!page_autooff_guide)
			return;

		vshcommon_F55812AE((int32_t)page_autooff_guide, (wchar_t*)message.c_str(), 4, 0);
	}

	void ShowButtonNavigationMessage(const std::string& message)
	{
		std::wstring ws(message.begin(), message.end());
		ShowButtonNavigationMessage(ws);
	}

	void ShowSystemNotification(const std::wstring& message, const std::string& texture)
	{
		paf::View* system_plugin = paf::View::Find("system_plugin");
		if (!system_plugin)
			return;

		paf::SurfacePtr pTexture = paf::SurfacePtr();

		if (texture != "")
			pTexture = system_plugin->GetTexture(texture.c_str());

		int32_t unknownOut = 0;
		vshcommon_A20E43DB(0, texture.c_str(), 0, (int32_t*)&pTexture, &unknownOut, "", "", 0.0, message.c_str(), 0, false, 0);
	}

	void ShowSystemNotification(const std::string& message, const std::string& texture)
	{
		std::wstring ws(message.begin(), message.end());
		ShowSystemNotification(ws, texture);
	}
}