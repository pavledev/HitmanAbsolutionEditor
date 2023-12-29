#pragma once

#include <vector>

#include "Resource.h"

class FabricResourceEntityBlueprint : public Resource
{
public:
	void Deserialize() override;
	std::string SerializeToJson();
	const std::vector<std::string>& GetClothPieceNames() const;
	const std::vector<std::string>& GetClothPieceExtendedNames() const;
	const std::vector<std::string>& GetTransformNames() const;
	const std::vector<std::string>& GetStrandsNames() const;
	std::vector<std::string>& GetClothPieceNames();
	std::vector<std::string>& GetClothPieceExtendedNames();
	std::vector<std::string>& GetTransformNames();
	std::vector<std::string>& GetStrandsNames();

private:
	enum class PropertyType
	{
		ClothPiece = 1,
		ClothPieceExtended,
		Transform,
		Strands
	};

	std::vector<std::string> clothPieceNames;
	std::vector<std::string> clothPieceExtendedNames;
	std::vector<std::string> transformNames;
	std::vector<std::string> strandsNames;
};
