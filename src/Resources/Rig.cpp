#include <unordered_map>

#include "Resources/Rig.h"

void Rig::Deserialize()
{
	rig = static_cast<MR::Rig*>(resourceData);

	rig->Locate();
}

void Rig::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}

void Rig::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = rig->SerializeToJson();
}
