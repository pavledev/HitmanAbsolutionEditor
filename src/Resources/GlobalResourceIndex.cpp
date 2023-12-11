#include "Resources/GlobalResourceIndex.h"
#include "Glacier/Serializer/ZBinaryDeserializer.h"
#include "Logger.h"

GlobalResourceIndex::~GlobalResourceIndex()
{
	operator delete(resourceIndex, std::align_val_t(alignment));
}

SResourceIndex* GlobalResourceIndex::GetResourceIndex()
{
	return resourceIndex;
}

bool GlobalResourceIndex::Deserialize(const std::string& filePath)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	return Deserialize(binaryReader);
}

bool GlobalResourceIndex::Deserialize(void* data, const unsigned int dataSize)
{
	BinaryReader binaryReader = BinaryReader(data, dataSize);

	return Deserialize(binaryReader);
}

bool GlobalResourceIndex::Deserialize(BinaryReader& binaryReader)
{
	ZBinaryDeserializer binaryDeserializer;

	resourceIndex = static_cast<SResourceIndex*>(binaryDeserializer.Deserialize(binaryReader));
	alignment = binaryDeserializer.GetAlignment();

	if (!resourceIndex)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize global resource index!");

		return false;
	}

	return true;
}

void GlobalResourceIndex::SerializeToJson(const std::string& outputFilePath)
{
	resourceIndex->SerializeToJson(outputFilePath);
}
