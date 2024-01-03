#pragma once

#include <string>

#include "Glacier/ReasoningGrid/SReasoningGrid.h"

#include "Resources/Resource.h"

class ReasoningGrid : public Resource
{
public:
	~ReasoningGrid();
	SReasoningGrid* GetReasoningGrid();
	bool Deserialize(const std::string& filePath);
	bool Deserialize(void* data, const unsigned int dataSize);
	bool Deserialize(BinaryReader& binaryReader);
	void SerializeToJson(const std::string& outputFilePath);

private:
	SReasoningGrid* reasoningGrid;
	unsigned char alignment;
};
