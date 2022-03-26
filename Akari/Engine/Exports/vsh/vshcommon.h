
#pragma once

#include <string>

namespace vsh
{
	void ShowButtonNavigationMessage(const std::wstring& message);
	void ShowButtonNavigationMessage(const std::string& message);

	void ShowSystemNotification(const std::wstring& message, const std::string& texture);
	void ShowSystemNotification(const std::string& message, const std::string& texture);
}