#include <unordered_map>

#include "Resources/MorphemeRuntimeRig.h"

void MorphemeRuntimeRig::Deserialize(const std::string& filePath)
{
}

void MorphemeRuntimeRig::Deserialize(void* buffer)
{
	rig = static_cast<MR::Rig*>(buffer);

	rig->Locate();
}

void MorphemeRuntimeRig::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = rig->SerializeToJson();
}
