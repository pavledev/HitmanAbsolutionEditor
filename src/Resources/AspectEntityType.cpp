#include "Resources/AspectEntityType.h"

void AspectEntityType::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		referenceIndices.push_back(binaryReader.Read<unsigned int>());
	}

	isResourceDeserialized = true;
}

void AspectEntityType::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}
