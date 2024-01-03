#include <unordered_map>

#include "Resources/Network.h"

void Network::Deserialize(const std::string& filePath)
{
}

void Network::Deserialize(void* buffer)
{
	networkDef = static_cast<MR::NetworkDef*>(buffer);

	networkDef->Locate();
}

void Network::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = networkDef->SerializeToJson();
}
