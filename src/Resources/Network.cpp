#include <unordered_map>

#include "Resources/Network.h"

void Network::Deserialize()
{
	networkDef = static_cast<MR::NetworkDef*>(resourceData);

	networkDef->Locate();
}

void Network::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}

void Network::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = networkDef->SerializeToJson();
}
