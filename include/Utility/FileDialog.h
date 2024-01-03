#pragma once

#include <string>

class FileDialog
{
public:
	static std::string OpenFolder();
	static void OpenSaveFileDialog(const char* defaultFileName, const char* defaultExtension, const char* filters, std::string& outputFilePath, std::string& extension);
};
