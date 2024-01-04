#pragma once

#include <string>

#include "Glacier/ReasoningGrid/SReasoningGrid.h"

#include "Resources/Resource.h"

class ReasoningGrid : public Resource
{
public:
	~ReasoningGrid();
	SReasoningGrid* GetReasoningGrid();
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(const std::string& outputFilePath);

private:
	SReasoningGrid* reasoningGrid;
	unsigned char alignment;
};
