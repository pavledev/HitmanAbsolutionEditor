#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector4.h"

#include "Resources/RenderMaterialEntityType.h"

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
}

std::vector<std::shared_ptr<RenderMaterialEntityType::Modifier>>& RenderMaterialEntityType::GetModifiers()
{
	return modifiers;
}
