#pragma once

#include <vector>

#include "Glacier/Cloth/ZFabricResourceEntity.h"

#include "Resource.h"
#include "Resources/FabricResourceEntityBlueprint.h"

class FabricResourceEntity : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(const std::string& outputFilePath);
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet>& GetClothPiecePropertySets();
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet>& GetClothPieceExtendedPropertySets();
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet>& GetTransformPropertySets();
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet>& GetStrandsPropertySets();

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
