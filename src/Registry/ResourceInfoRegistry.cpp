#include "Registry/ResourceInfoRegistry.h"
#include "Logger.h"
#include "Utility/StringUtility.h"

ResourceInfoRegistry::ResourceInfoRegistry()
{
    isResourcesInfoLoaded = false;
}

ResourceInfoRegistry& ResourceInfoRegistry::GetInstance()
{
	static ResourceInfoRegistry instance;

	return instance;
}

void ResourceInfoRegistry::Load()
{
    std::ifstream inputFile = std::ifstream("assets/Resources.txt");

    if (!inputFile.is_open())
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to open Resource.txt file!");

        return;
    }

    inputFile.seekg(0, inputFile.end);

    size_t fileSize = static_cast<size_t>(inputFile.tellg());

    inputFile.seekg(0, inputFile.beg);

    std::vector<char> hashListData = std::vector<char>(fileSize, 0);
    unsigned int position = 0, lastPosition = 0;

    inputFile.read(hashListData.data(), fileSize);

    while (true)
    {
        if (hashListData.data()[position] == 0xA)
        {
            hashListData.data()[position] = 0;

            std::string line = std::string(&hashListData.data()[lastPosition]);

            if (position == 6)
            {
                resourcesInfo.reserve(std::stoul(line.data()));

                lastPosition = position + 1;

                continue;
            }

            std::vector<std::string> parts;

            parts.reserve(5);

            StringUtility::Split(line, parts, '|');

            ResourceInfo resourceInfo;

            resourceInfo.hash = std::stoull(parts[0], nullptr, 16);
            resourceInfo.resourceID = parts[1];
            resourceInfo.type = parts[2];

            if (parts.size() > 3)
            {
                std::vector<std::string> parts2;

                parts2.reserve(std::stoul(parts[3].data()));

                StringUtility::Split(parts[4], parts2, '#');

                resourceInfo.headerLibraries.reserve(parts2.size());

                for (size_t i = 0; i < parts2.size(); ++i)
                {
                    Resource::HeaderLibrary headerLibrary;
                    size_t index = parts2[i].find("$(");
                    size_t index2 = parts2[i].find(",", index);

                    headerLibrary.resourceID = parts2[i].substr(0, index);
                    headerLibrary.chunkIndex = std::stoul(parts2[i].substr(index + 2, index2 - index - 2));
                    headerLibrary.indexInLibrary = std::stoul(parts2[i].substr(index2 + 1, parts2[i].length() - index2 - 2));

                    resourceInfo.headerLibraries.push_back(headerLibrary);
                }
            }

            resourcesInfo.insert(std::make_pair(resourceInfo.hash, resourceInfo));

            lastPosition = position + 1;
        }

        position++;

        if (position > fileSize)
        {
            break;
        }
    }

    inputFile.close();

    Logger::GetInstance().Log(Logger::Level::Info, "Sucessfully loaded resources.");

    isResourcesInfoLoaded = true;
}

const ResourceInfoRegistry::ResourceInfo& ResourceInfoRegistry::GetResourceInfo(const unsigned long long hash)
{
    return resourcesInfo[hash];
}

const std::unordered_map<unsigned long long, ResourceInfoRegistry::ResourceInfo>& ResourceInfoRegistry::GetResourcesInfo() const
{
    return resourcesInfo;
}

const bool ResourceInfoRegistry::IsResourcesInfoLoaded() const
{
	return isResourcesInfoLoaded;
}
