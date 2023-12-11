#include <unordered_map>

#include "Resources/MorphemeRuntimeNetwork.h"

void MorphemeRuntimeNetwork::Deserialize(const std::string& filePath)
{
}

void MorphemeRuntimeNetwork::Deserialize(void* buffer)
{
	networkDef = static_cast<MR::NetworkDef*>(buffer);

	networkDef->Locate();
}

void MorphemeRuntimeNetwork::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = networkDef->SerializeToJson();
}
