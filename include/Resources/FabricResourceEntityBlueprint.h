#pragma once

#include <vector>

#include "IO/BinaryReader.h"

class FabricResourceEntityBlueprint
{
public:
	void Deserialize(const std::string& filePath);
	void Deserialize(void* buffer, const unsigned int dataSize);
	void Deserialize(BinaryReader& binaryReader);
	std::string SerializeToJson();
	const std::vector<std::string>& GetClothPieceNames() const;
	const std::vector<std::string>& GetClothPieceExtendedNames() const;
	const std::vector<std::string>& GetTransformNames() const;
	const std::vector<std::string>& GetStrandsNames() const;

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
