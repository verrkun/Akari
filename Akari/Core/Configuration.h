
#pragma once

#include "../Utilities/Math.h"

class Config
{
public:
	Config();
	~Config();

	void Load();
	void Save();

public:
	struct {
		bool enableBinds{};
		int binds{};
	} screenshots;
	struct {
		bool enable{};
		int refreshDelay = 500;
		bool drawFps{};
		int fpsPrecision{};
		bool drawScreenRes{};
		bool drawMemory{};
		bool drawCPUTemp{};
		bool drawGPUTemp{};
		int tempType{};
		bool drawFanSpeed{};
		bool drawLocalIp{};
		int horizontalPos{};
		int verticalPos{};
	} overlay;
	struct {
		vec2 position{ 0, 0 };
		vec2 safeArea{ 31, 18 };
		float sizeWidth = 300;
		float sizeBorder = 2;
		float sizeText = 20;
		Color colorLeft{ 1, 1, 0, 1 };
		Color colorRight{ 0, 1, 1, 1 };
		Color colorBackground{ 0, 0, 0, 0.75 };
		Color colorText{ 1, 1, 1, 1 };
		bool useSound = true;
		bool useDualColor = true;
		bool showControlsHelp = true;
	} menu;
};

extern Config* g_Config;