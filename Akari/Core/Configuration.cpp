
#include "Configuration.h"
#include "../Utilities/System/File.h"

Config* g_Config;

Config::Config()
{
	Load();
}

Config::~Config()
{
	Save();
}

void Config::Load()
{
	std::string configPath = File::GetCurrentDir() + "AkariConfig.bin";
	File::ReadFile(configPath, this, sizeof(Config));
}

void Config::Save()
{
	std::string configPath = File::GetCurrentDir() + "AkariConfig.bin";
	File::WriteFile(configPath, this, sizeof(Config));
}