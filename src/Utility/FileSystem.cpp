#include <filesystem>

#include "Utility/FileSystem.h"

std::string FileSystem::GetDirectoryFromFilePath(const std::string& path)
{
    const size_t last_index = path.find_last_of("\\/");

    if (last_index != std::string::npos)
    {
        return path.substr(0, last_index + 1);
    }

    return "";
}

std::string FileSystem::GetFileNameFromFilePath(const std::string& path)
{
    return std::filesystem::path(path).filename().generic_string();
}
