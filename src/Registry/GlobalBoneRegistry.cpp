#include "Registry/GlobalBoneRegistry.h"

GlobalBoneRegistry& GlobalBoneRegistry::GetInstance()
{
	static GlobalBoneRegistry instance;

	return instance;
}

void GlobalBoneRegistry::Load()
{
	for (unsigned int i = 0; i < 63; ++i)
	{
		const char* boneName = boneNames[i];

		boneNameToGlobalIDMap[boneName] = boneNameToGlobalIDMap.size();
	}
}

unsigned int GlobalBoneRegistry::GetBoneID(const std::string& boneName)
{
	auto iterator = boneNameToGlobalIDMap.find(boneName);

	if (iterator != boneNameToGlobalIDMap.end())
	{
		return iterator->second;
	}

	return -1;
}

unsigned int GlobalBoneRegistry::RegisterBoneID(const std::string& boneName)
{
	boneNameToGlobalIDMap[boneName] = boneNameToGlobalIDMap.size();

	return boneNameToGlobalIDMap[boneName];
}
