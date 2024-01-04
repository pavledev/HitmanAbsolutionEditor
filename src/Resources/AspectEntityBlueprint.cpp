#include "Resources/AspectEntityBlueprint.h"

void AspectEntityBlueprint::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		referenceIndices.push_back(binaryReader.Read<unsigned int>());
	}

	isResourceDeserialized = true;
}

void AspectEntityBlueprint::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}
