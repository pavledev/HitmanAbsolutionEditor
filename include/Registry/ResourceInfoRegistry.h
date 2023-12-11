#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Resources/Resource.h"

class ResourceInfoRegistry
{
public:
	struct ResourceInfo
	{
		unsigned long long hash;
		std::string resourceID;
		std::string type;
		std::vector<Resource::HeaderLibrary> headerLibraries;
	};

	static ResourceInfoRegistry& GetInstance();
	void Load();
	const ResourceInfo& GetResourceInfo(const unsigned long long hash);
	const std::unordered_map<unsigned long long, ResourceInfo>& GetResourcesInfo() const;
	const bool IsResourcesInfoLoaded() const;

private:
	ResourceInfoRegistry();
	ResourceInfoRegistry(const ResourceInfoRegistry& other) = delete;
	ResourceInfoRegistry& operator=(const ResourceInfoRegistry& other) = delete;

	std::unordered_map<unsigned long long, ResourceInfo> resourcesInfo;
	bool isResourcesInfoLoaded;
};
