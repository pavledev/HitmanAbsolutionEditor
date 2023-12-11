#include <fstream>
#include <filesystem>

#define MINI_CASE_SENSITIVE

#include <ini.h>

#include "Settings.h"

Settings& Settings::GetInstance()
{
	static Settings instance;

	return instance;
}

void Settings::LoadSettings()
{
    GetValueFromIniFile("Settings", "RuntimeFolderPath", runtimeFolderPath);
}

const bool Settings::IniFileHasKey(const std::string& section, const std::string& key)
{
    const std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionEditor.ini", std::filesystem::current_path().string());

    if (!std::filesystem::exists(iniFilePath))
    {
        return false;
    }

    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    iniFile.read(iniStructure);

    if (iniStructure.has(section) && iniStructure[section].has(key))
    {
        return true;
    }

    return false;
}

void Settings::GetValueFromIniFile(const std::string& section, const std::string& key, std::string& outputValue)
{
    const std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionEditor.ini", std::filesystem::current_path().string());

    if (!std::filesystem::exists(iniFilePath))
    {
        return;
    }

    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    iniFile.read(iniStructure);

    if (iniStructure.has(section) && iniStructure[section].has(key))
    {
        outputValue = iniStructure[section][key];
    }
}

void Settings::UpdateIniFile(const std::string& key, const std::string& value)
{
    const std::filesystem::path iniFilePath = std::format("{}\\HitmanAbsolutionEditor.ini", std::filesystem::current_path().string());
    const mINI::INIFile iniFile = mINI::INIFile(iniFilePath.string());
    mINI::INIStructure iniStructure;

    if (std::filesystem::exists(iniFilePath))
    {
        iniFile.read(iniStructure);
    }

    iniStructure["Settings"][key] = value;

    iniFile.generate(iniStructure, true);
}

const std::string Settings::GetRuntimeFolderPath() const
{
	return runtimeFolderPath;
}

void Settings::SetRuntimeFolderPath(const std::string& runtimeFolderPath)
{
	this->runtimeFolderPath = runtimeFolderPath;
}
