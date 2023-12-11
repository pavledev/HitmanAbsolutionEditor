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

bool ReasoningGrid::Deserialize(const std::string& filePath)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	return Deserialize(binaryReader);
}

bool ReasoningGrid::Deserialize(void* data, const unsigned int dataSize)
{
	BinaryReader binaryReader = BinaryReader(data, dataSize);

	return Deserialize(binaryReader);
}

bool ReasoningGrid::Deserialize(BinaryReader& binaryReader)
{
	ZBinaryDeserializer binaryDeserializer;

	reasoningGrid = static_cast<SReasoningGrid*>(binaryDeserializer.Deserialize(binaryReader));
	alignment = binaryDeserializer.GetAlignment();

	if (!reasoningGrid)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize reasoning grid!");

		return false;
	}

	return true;
}

void ReasoningGrid::SerializeToJson(const std::string& outputFilePath)
{
	reasoningGrid->SerializeToJson(outputFilePath);
}
