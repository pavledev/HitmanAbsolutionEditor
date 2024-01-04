#include "Resources/RenderMaterialEntityBlueprint.h"

void RenderMaterialEntityBlueprint::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		std::shared_ptr<Modifier> modifier = std::make_shared<Modifier>();

		modifier->type = static_cast<ModifierType>(binaryReader.Read<unsigned char>());
		modifier->name = binaryReader.ReadString(static_cast<size_t>(binaryReader.Read<unsigned int>()) - 1);

		modifiers.push_back(modifier);
	}

	isResourceDeserialized = true;
}

void RenderMaterialEntityBlueprint::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}

std::vector<std::shared_ptr<RenderMaterialEntityBlueprint::Modifier>>& RenderMaterialEntityBlueprint::GetModifiers()
{
	return modifiers;
}
