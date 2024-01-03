#include <unordered_map>

#include "Resources/Rig.h"

void Rig::Deserialize(const std::string& filePath)
{
}

void Rig::Deserialize(void* buffer)
{
	rig = static_cast<MR::Rig*>(buffer);

	rig->Locate();
}

void Rig::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = rig->SerializeToJson();
}
