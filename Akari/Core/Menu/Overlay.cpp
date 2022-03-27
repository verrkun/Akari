
#include "Overlay.h"
#include "../Configuration.h"
#include "../../Engine/Exports/paf/View.h"
#include "../../Utilities/System/Timers.h"
#include <sys/sys_time.h>
#include <vsh/netctl_main.h>
#include <vsh/paf.h>
#include <vsh/stdc.h>

Overlay* g_Overlay;

Overlay::Overlay()
{
	sys_ppu_thread_create(&UpdateInfoThreadId, UpdateInfoThread, 0, 0xB01, 512, SYS_PPU_THREAD_CREATE_JOINABLE, "Overlay::UpdateInfoThread()");
}

Overlay::~Overlay()
{
	if (UpdateInfoThreadId != SYS_PPU_THREAD_ID_INVALID)
	{
		m_StateRunning = false;

		sys_ppu_thread_yield();
		Timers::Sleep(200);

		uint64_t exitCode;
		sys_ppu_thread_join(UpdateInfoThreadId, &exitCode);
	}
}

void Overlay::OnUpdate()
{
	paf::View* game_plugin = paf::View::Find("game_plugin");
	paf::View* game_ext_plugin = paf::View::Find("game_ext_plugin");

	if ((game_plugin || game_ext_plugin) && !m_StateGameRunning)
	{
		m_StateGameRunning = true;
		m_StateGameJustLaunched = true;
	}
	else if (!game_plugin && !game_ext_plugin)
		m_StateGameRunning = false;


	UpdatePosition();
	CalculateFps();

	if (g_Config->overlay.enable)
		DrawOverlay();
}

void Overlay::DrawOverlay()
{
	std::wstring overlayText = L"";
	wchar_t buffer[0x200];

	if (g_Config->overlay.drawFps)
	{
		wchar_t tmp[0x10];
		stdc::swprintf(tmp, 0x10, L"FPS: %%.%if\n", g_Config->overlay.fpsPrecision);

		stdc::swprintf(buffer, 0x200, tmp, m_FPS);
		overlayText += buffer;
	}

	if (g_Config->overlay.drawScreenRes)
	{
		stdc::swprintf(buffer, 0x200, L"Screen resolution: %ix%i\n", paf_F476E8AA(), paf_AC984A12());
		overlayText += buffer;
	}

	if (g_Config->overlay.drawMemory)
	{
		stdc::swprintf(buffer, 0x200, L"RAM: %.1f%% %i / %i KB\n", m_MemoryUsage.percent, m_MemoryUsage.used, m_MemoryUsage.total);
		overlayText += buffer;
	}

	if (g_Config->overlay.drawCPUTemp)
	{
		stdc::swprintf(buffer, 0x200, L"CPU Temp: %.1f", m_CPUTemp);
		overlayText += buffer + GetTemperatureSymbol() + L"\n";
	}

	if (g_Config->overlay.drawGPUTemp)
	{
		stdc::swprintf(buffer, 0x200, L"GPU Temp: %.1f", m_GPUTemp);
		overlayText += buffer + GetTemperatureSymbol() + L"\n";
	}

	if (g_Config->overlay.drawFanSpeed)
	{
		stdc::swprintf(buffer, 0x200, L"Fan speed: %.1f%%\n", m_FanSpeed);
		overlayText += buffer;
	}

	if (g_Config->overlay.drawLocalIp)
	{
		stdc::swprintf(buffer, 0x200, L"Local IP: %s\n", m_LocalIp.c_str());
		overlayText += buffer;
	}

	g_Render.Text(
		overlayText,
		m_Position,
		g_Config->menu.sizeText,
		m_HorizontalAlignment,
		m_VerticalAlignment,
		g_Config->menu.colorText);
}

void Overlay::UpdatePosition()
{
	switch (g_Config->overlay.horizontalPos)
	{
	case 0: // Left
	{
		m_Position.x = -VIEWPORT_WIDTH / 2 + g_Config->menu.safeArea.x + 5;
		m_HorizontalAlignment = CRender::Left;
		break;
	}
	case 1: // Center
	{
		m_Position.x = 0;
		m_HorizontalAlignment = CRender::Centered;
		break;
	}
	case 2: // Right
	{
		m_Position.x = VIEWPORT_WIDTH / 2 - g_Config->menu.safeArea.x - 5;
		m_HorizontalAlignment = CRender::Right;
		break;
	}
	}

	switch (g_Config->overlay.verticalPos)
	{
	case 0: // Top
	{
		m_Position.y = VIEWPORT_HEIGHT / 2 - g_Config->menu.safeArea.y - 5;
		m_VerticalAlignment = CRender::Top;
		break;
	}
	case 1: // Center
	{
		m_Position.y = 0;
		m_VerticalAlignment = CRender::Centered;
		break;
	}
	case 2: // Bottom
	{
		m_Position.y = -VIEWPORT_HEIGHT / 2 + g_Config->menu.safeArea.y + 5;
		m_VerticalAlignment = CRender::Bottom;
		break;
	}
	}
}

void Overlay::CalculateFps()
{
	// FPS REPORTING
	// get current timing info
	float timeNow = (float)sys_time_get_system_time() * .000001f;
	float fElapsedInFrame = (float)(timeNow - m_FpsLastTime);
	m_FpsLastTime = timeNow;
	++m_FpsFrames;
	m_FpsTimeElapsed += fElapsedInFrame;

	// report fps at appropriate interval
	if (m_FpsTimeElapsed >= m_FpsTimeReport)
	{
		m_FPS = (m_FpsFrames - m_FpsFramesLastReport) * 1.f / (float)(m_FpsTimeElapsed - m_FpsTimeLastReport);

		m_FpsTimeReport += m_FpsREPORT_TIME;
		m_FpsTimeLastReport = m_FpsTimeElapsed;
		m_FpsFramesLastReport = m_FpsFrames;
	}
}

std::wstring Overlay::GetTemperatureSymbol()
{
	switch (g_Config->overlay.tempType)
	{
	case 0: return L"°C";
	case 1: return L"°F";
	case 2: return L"K";
	default: return L"";
	}
}

void Overlay::UpdateInfoThread(uint64_t arg)
{
	g_Overlay->m_StateRunning = true;

	while (g_Overlay->m_StateRunning)
	{
		Timers::Sleep(g_Config->overlay.refreshDelay);

		// Using syscalls in a loop on hen will cause a black screen when launching a game
		// so in order to fix this we need to sleep 10/15 seconds when a game is launched
		if (g_Overlay->m_StateGameJustLaunched)
		{
			Timers::Sleep(15 * 1000);
			g_Overlay->m_StateGameJustLaunched = false;
		}

		g_Overlay->m_MemoryUsage = ConsoleInfo::GetMemoryUsage();
		g_Overlay->m_FanSpeed = ConsoleInfo::GetFanSpeed();

		switch (g_Config->overlay.tempType)
		{
		case 0:
			g_Overlay->m_CPUTemp = ConsoleInfo::GetTemperatureCelsius(0);
			g_Overlay->m_GPUTemp = ConsoleInfo::GetTemperatureCelsius(1);
			break;
		case 1:
			g_Overlay->m_CPUTemp = ConsoleInfo::GetTemperatureFahrenheit(0);
			g_Overlay->m_GPUTemp = ConsoleInfo::GetTemperatureFahrenheit(1);
			break;
		case 2:
			g_Overlay->m_CPUTemp = ConsoleInfo::GetTemperatureKelvin(0);
			g_Overlay->m_GPUTemp = ConsoleInfo::GetTemperatureKelvin(1);
			break;
		}

		char ip[16];
		netctl_main_9A528B81(16, ip); // Get local ip address
		g_Overlay->m_LocalIp = (ip[0]) ? std::string(ip) : "0.0.0.0";
	}

	sys_ppu_thread_exit(0);
}