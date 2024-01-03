#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector4.h"

#include "Resources/RenderMaterialEntityType.h"
#include "Resources/RenderMaterialEntityBlueprint.h"
#include "Registry/ResourceInfoRegistry.h"

RenderMaterialEntityType::Modifier::Modifier()
{
	value = nullptr;
}

RenderMaterialEntityType::Modifier::~Modifier()
{
	operator delete(value);
}

void RenderMaterialEntityType::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		std::shared_ptr<Modifier> modifier = std::make_shared<Modifier>();

		modifier->type = static_cast<ModifierType>(binaryReader.Read<unsigned char>());

		switch (modifier->type)
		{
			case ModifierType::UInt:
				modifier->value = binaryReader.Read<void>(sizeof(unsigned int));
				break;
			case ModifierType::SColorRGB:
				modifier->value = binaryReader.Read<void>(sizeof(SColorRGB));
				break;
			case ModifierType::SColorRGBA:
				modifier->value = binaryReader.Read<void>(sizeof(SColorRGBA));
				break;
			case ModifierType::Float:
				modifier->value = binaryReader.Read<void>(sizeof(float));
				break;
			case ModifierType::SVector2:
				modifier->value = binaryReader.Read<void>(sizeof(SVector2));
				break;
			case ModifierType::SVector3:
				modifier->value = binaryReader.Read<void>(sizeof(SVector3));
				break;
			case ModifierType::SVector4:
				modifier->value = binaryReader.Read<void>(sizeof(SVector4));
				break;
		}

		modifiers.push_back(modifier);
	}

	isResourceDeserialized = true;
}

void RenderMaterialEntityType::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		SerializeToJson(outputPath);
	}
}

void RenderMaterialEntityType::SerializeToJson(const std::string& outputFilePath)
{
	std::shared_ptr<RenderMaterialEntityBlueprint> renderMaterialEntityBlueprint;
	std::vector<std::shared_ptr<Resource>>& clotReferences = GetReferences();

	for (size_t i = 0; i < clotReferences.size(); ++i)
	{
		const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(clotReferences[i]->GetHash());

		if (referenceInfo.type == "MATB")
		{
			renderMaterialEntityBlueprint = std::static_pointer_cast<RenderMaterialEntityBlueprint>(clotReferences[i]);

			break;
		}
	}

	const ResourceInfoRegistry::ResourceInfo& matbResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(renderMaterialEntityBlueprint->GetHash());

	renderMaterialEntityBlueprint->LoadResource(0, matbResourceInfo.headerLibraries[0].chunkIndex, matbResourceInfo.headerLibraries[0].indexInLibrary, false, false, true);
	renderMaterialEntityBlueprint->Deserialize();
	renderMaterialEntityBlueprint->DeleteResourceData();

	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("modifiers");
	writer.StartObject();

	const std::vector<std::shared_ptr<Resource>>& mattReferences = GetReferences();
	const std::vector<std::shared_ptr<RenderMaterialEntityBlueprint::Modifier>>& matbModifiers = renderMaterialEntityBlueprint->GetModifiers();

	for (size_t i = 0; i < modifiers.size(); ++i)
	{
		writer.String(matbModifiers[i]->name.c_str());

		switch (modifiers[i]->type)
		{
			case ModifierType::UInt:
			{
				const unsigned int textureReferenceIndex = *static_cast<unsigned int*>(modifiers[i]->value);

				if (textureReferenceIndex != -1)
				{
					writer.String(mattReferences[*static_cast<unsigned int*>(modifiers[i]->value)]->GetResourceID().c_str());
				}
				else
				{
					writer.Uint(textureReferenceIndex);
				}

				break;
			}
			case ModifierType::SColorRGB:
				static_cast<SColorRGB*>(modifiers[i]->value)->SerializeToJson(writer);
				break;
			case ModifierType::SColorRGBA:
				static_cast<SColorRGBA*>(modifiers[i]->value)->SerializeToJson(writer);
				break;
			case ModifierType::Float:
				writer.Double(*static_cast<float*>(modifiers[i]->value));
				break;
			case ModifierType::SVector2:
				static_cast<SVector2*>(modifiers[i]->value)->SerializeToJson(writer);
				break;
			case ModifierType::SVector3:
				static_cast<SVector3*>(modifiers[i]->value)->SerializeToJson(writer);
				break;
			case ModifierType::SVector4:
				static_cast<SVector4*>(modifiers[i]->value)->SerializeToJson(writer);
				break;
		}
	}

	writer.EndObject();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}

std::vector<std::shared_ptr<RenderMaterialEntityType::Modifier>>& RenderMaterialEntityType::GetModifiers()
{
	return modifiers;
}
