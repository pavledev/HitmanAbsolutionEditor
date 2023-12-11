#pragma once

#include <vector>

#include "Glacier/Cloth/ZFabricResourceEntity.h"

#include "IO/BinaryReader.h"
#include "Resources/FabricResourceEntityBlueprint.h"

class FabricResourceEntity
{
public:
	void Deserialize(const std::string& filePath);
	void Deserialize(void* buffer, const unsigned int dataSize);
	void Deserialize(BinaryReader& binaryReader);
	std::string SerializeToJson();
	std::string SerializeToJson(const FabricResourceEntityBlueprint& fabricResourceEntityBlueprint);

private:
	enum class PropertyType
	{
		ClothPiece = 1,
		ClothPieceExtended,
		Transform,
		Strands
	};

	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet> clothPiecePropertySets;
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet> clothPieceExtendedPropertySets;
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet> transformPropertySets;
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet> strandsPropertySets;
};
