#include "Glacier/ZString.h"

#include "Resources/FabricResourceEntityBlueprint.h"

void FabricResourceEntityBlueprint::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		PropertyType propertyType = static_cast<PropertyType>(binaryReader.Read<unsigned char>());
		unsigned int stringLength = binaryReader.Read<unsigned int>();
		std::string propertyName = binaryReader.ReadString(static_cast<size_t>(stringLength - 1));

		switch (propertyType)
		{
			case PropertyType::ClothPiece:
				clothPieceNames.push_back(propertyName);
				break;
			case PropertyType::ClothPieceExtended:
				clothPieceExtendedNames.push_back(propertyName);
				break;
			case PropertyType::Transform:
				transformNames.push_back(propertyName);
				break;
			case PropertyType::Strands:
				strandsNames.push_back(propertyName);
				break;
			default:
				break;
		}
	}

	isResourceDeserialized = true;
}

std::string FabricResourceEntityBlueprint::SerializeToJson()
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("clothPieceNames");
	writer.StartArray();

	for (size_t i = 0; i < clothPieceNames.size(); ++i)
	{
		writer.String(clothPieceNames[i].c_str());
	}

	writer.EndArray();

	writer.String("clothPieceExtendedNames");
	writer.StartArray();

	for (size_t i = 0; i < clothPieceExtendedNames.size(); ++i)
	{
		writer.String(clothPieceExtendedNames[i].c_str());
	}

	writer.EndArray();

	writer.String("transformNames");
	writer.StartArray();

	for (size_t i = 0; i < transformNames.size(); ++i)
	{
		writer.String(transformNames[i].c_str());
	}

	writer.EndArray();

	writer.String("strandsNames");
	writer.StartArray();

	for (size_t i = 0; i < strandsNames.size(); ++i)
	{
		writer.String(strandsNames[i].c_str());
	}

	writer.EndArray();

	writer.EndObject();

	return stringBuffer.GetString();
}

const std::vector<std::string>& FabricResourceEntityBlueprint::GetClothPieceNames() const
{
	return clothPieceNames;
}

const std::vector<std::string>& FabricResourceEntityBlueprint::GetClothPieceExtendedNames() const
{
	return clothPieceExtendedNames;
}

const std::vector<std::string>& FabricResourceEntityBlueprint::GetTransformNames() const
{
	return transformNames;
}

const std::vector<std::string>& FabricResourceEntityBlueprint::GetStrandsNames() const
{
	return strandsNames;
}

std::vector<std::string>& FabricResourceEntityBlueprint::GetClothPieceNames()
{
	return clothPieceNames;
}

std::vector<std::string>& FabricResourceEntityBlueprint::GetClothPieceExtendedNames()
{
	return clothPieceExtendedNames;
}

std::vector<std::string>& FabricResourceEntityBlueprint::GetTransformNames()
{
	return transformNames;
}

std::vector<std::string>& FabricResourceEntityBlueprint::GetStrandsNames()
{
	return strandsNames;
}
