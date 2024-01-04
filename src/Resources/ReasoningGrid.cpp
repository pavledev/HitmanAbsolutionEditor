#include "Resources/ReasoningGrid.h"
#include "Glacier/Serializer/ZBinaryDeserializer.h"
#include "Logger.h"

ReasoningGrid::~ReasoningGrid()
{
	operator delete(reasoningGrid, std::align_val_t(alignment));
}

SReasoningGrid* ReasoningGrid::GetReasoningGrid()
{
	return reasoningGrid;
}

void ReasoningGrid::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;

	reasoningGrid = static_cast<SReasoningGrid*>(binaryDeserializer.Deserialize(resourceData, resourceDataSize));
	alignment = binaryDeserializer.GetAlignment();
}

void ReasoningGrid::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}

void ReasoningGrid::SerializeToJson(const std::string& outputFilePath)
{
	reasoningGrid->SerializeToJson(outputFilePath);
}
