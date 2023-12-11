#pragma once

#include <string>

class FileSystem
{
public:
	static std::string GetDirectoryFromFilePath(const std::string& path);
	static std::string GetFileNameFromFilePath(const std::string& path);
};
