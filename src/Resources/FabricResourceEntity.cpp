#include "Resources/FabricResourceEntity.h"

void FabricResourceEntity::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		PropertyType propertyType = static_cast<PropertyType>(binaryReader.Read<unsigned char>());

		switch (propertyType)
		{
			case PropertyType::ClothPiece:
				clothPiecePropertySets.push_back(binaryReader.Read<ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet>());
				break;
			case PropertyType::ClothPieceExtended:
			{
				ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet clothPieceExtendedPropertySet{};

				clothPieceExtendedPropertySet.m_vGravity = binaryReader.Read<SVector3>();
				clothPieceExtendedPropertySet.m_nNumIterations = binaryReader.Read<unsigned int>();
				clothPieceExtendedPropertySet.m_fDamping = binaryReader.Read<float>();

				clothPieceExtendedPropertySets.push_back(clothPieceExtendedPropertySet);

				break;
			}
			case PropertyType::Transform:
				transformPropertySets.push_back(ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet());
				break;
			case PropertyType::Strands:
				strandsPropertySets.push_back(binaryReader.Read<ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet>());
				break;
			default:
				break;
		}
	}

	isResourceDeserialized = true;
}

std::string FabricResourceEntity::SerializeToJson()
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("clothPiecePropertySets");
	writer.StartArray();

	for (size_t i = 0; i < clothPiecePropertySets.size(); ++i)
	{
		writer.StartObject();

		clothPiecePropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("clothPieceExtendedPropertySets");
	writer.StartArray();

	for (size_t i = 0; i < clothPieceExtendedPropertySets.size(); ++i)
	{
		writer.StartObject();

		clothPieceExtendedPropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("transformPropertySets");
	writer.StartArray();

	for (size_t i = 0; i < transformPropertySets.size(); ++i)
	{
		writer.StartObject();

		transformPropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("strandsPropertySets");
	writer.StartArray();

	for (size_t i = 0; i < strandsPropertySets.size(); ++i)
	{
		writer.StartObject();

		strandsPropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.EndObject();

	return stringBuffer.GetString();
}

std::string FabricResourceEntity::SerializeToJson(const FabricResourceEntityBlueprint& fabricResourceEntityBlueprint)
{
	std::vector<std::string> clothPieceNames = fabricResourceEntityBlueprint.GetClothPieceNames();
	std::vector<std::string> clothPieceExtendedNames = fabricResourceEntityBlueprint.GetClothPieceExtendedNames();
	std::vector<std::string> transformNames = fabricResourceEntityBlueprint.GetTransformNames();
	std::vector<std::string> strandsNames = fabricResourceEntityBlueprint.GetStrandsNames();

	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("clothPiecePropertySets");
	writer.StartArray();

	for (size_t i = 0; i < clothPiecePropertySets.size(); ++i)
	{
		writer.StartObject();

		writer.String("name");
		writer.String(clothPieceNames[i].c_str());

		clothPiecePropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("clothPieceExtendedPropertySets");
	writer.StartArray();

	for (size_t i = 0; i < clothPieceExtendedPropertySets.size(); ++i)
	{
		writer.StartObject();

		writer.String("name");
		writer.String(clothPieceExtendedNames[i].c_str());

		clothPieceExtendedPropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("transformPropertySets");
	writer.StartArray();

	for (size_t i = 0; i < transformPropertySets.size(); ++i)
	{
		writer.StartObject();

		writer.String("name");
		writer.String(transformNames[i].c_str());

		transformPropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("strandsPropertySets");
	writer.StartArray();

	for (size_t i = 0; i < strandsPropertySets.size(); ++i)
	{
		writer.StartObject();

		writer.String("name");
		writer.String(strandsNames[i].c_str());

		strandsPropertySets[i].SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.EndObject();

	return stringBuffer.GetString();
}

std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet>& FabricResourceEntity::GetClothPiecePropertySets()
{
	return clothPiecePropertySets;
}

std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet>& FabricResourceEntity::GetClothPieceExtendedPropertySets()
{
	return clothPieceExtendedPropertySets;
}

std::vector<ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet>& FabricResourceEntity::GetTransformPropertySets()
{
	return transformPropertySets;
}

std::vector<ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet>& FabricResourceEntity::GetStrandsPropertySets()
{
	return strandsPropertySets;
}
