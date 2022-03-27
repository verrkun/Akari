
#pragma once

#include "../../Engine/Rendering.h"
#include "../../Utilities/Math.h"
#include "../../Utilities/System/ConsoleInfo.h"
#include <sys/ppu_thread.h>

class Overlay
{
public:
	Overlay();
	~Overlay();

	void OnUpdate();

private:
	void DrawOverlay();

	void UpdatePosition();
	void CalculateFps();
	std::wstring GetTemperatureSymbol();
	static void UpdateInfoThread(uint64_t arg);

public:
	sys_ppu_thread_t UpdateInfoThreadId = SYS_PPU_THREAD_ID_INVALID;

	bool m_StateRunning{};
	bool m_StateGameRunning{};
	bool m_StateGameJustLaunched{};

	float m_CPUTemp{};
	float m_GPUTemp{};
	float m_FanSpeed{};
	std::string m_LocalIp{};
	ConsoleInfo::memUsage_s m_MemoryUsage{};


private:

	vec2 m_Position{};
	CRender::Align m_HorizontalAlignment{};
	CRender::Align m_VerticalAlignment{};

	float m_FPS = 100.0f;
	float m_FpsLastTime = 0;
	int m_FpsFrames = 0;
	int m_FpsFramesLastReport = 0;
	double m_FpsTimeElapsed = 0;
	double m_FpsTimeReport = 0;
	double m_FpsTimeLastReport = 0;
	float m_FpsREPORT_TIME = 1.0f;
};

extern Overlay* g_Overlay;