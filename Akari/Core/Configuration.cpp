
#include "Configuration.h"
#include "../Utilities/System/File.h"

Config g_Config;

Config::Config()
{
	Load();
}

void Config::Load()
{
	std::string configPath = File::GetCurrentDir() + "AkariConfig.bin";

	if (File::FileExist(configPath))
		File::ReadFile(configPath, this, sizeof(Config));
}

void Config::Save()
{
	std::string configPath = File::GetCurrentDir() + "AkariConfig.bin";
	File::WriteFile(configPath, this, sizeof(Config));
}

void Config::Reset()
{
	std::string configPath = File::GetCurrentDir() + "AkariConfig.bin";

	if (File::FileExist(configPath))
		File::DeleteFile(configPath);

	*this = Config();
	Save();
}