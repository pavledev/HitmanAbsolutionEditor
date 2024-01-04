#include <unordered_map>

#include "Resources/Network.h"

void Network::Deserialize()
{
	networkDef = static_cast<MR::NetworkDef*>(buffer);

	networkDef->Locate();
}

void Network::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = networkDef->SerializeToJson();
}
