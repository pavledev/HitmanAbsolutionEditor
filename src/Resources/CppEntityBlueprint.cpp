#include "Resources/CppEntityBlueprint.h"
#include "Glacier/Serializer/ZBinaryDeserializer.h"
#include "Logger.h"

CppEntityBlueprint::~CppEntityBlueprint()
{
	operator delete(cppEntityBlueprint, std::align_val_t(alignment));
}

SCppEntityBlueprint* CppEntityBlueprint::GetCppEntityBlueprint()
{
	return cppEntityBlueprint;
}

bool CppEntityBlueprint::Deserialize(const std::string& filePath)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	return Deserialize(binaryReader);
}

bool CppEntityBlueprint::Deserialize(void* data, const unsigned int dataSize)
{
	BinaryReader binaryReader = BinaryReader(data, dataSize);

	return Deserialize(binaryReader);
}

bool CppEntityBlueprint::Deserialize(BinaryReader& binaryReader)
{
	ZBinaryDeserializer binaryDeserializer;

	cppEntityBlueprint = static_cast<SCppEntityBlueprint*>(binaryDeserializer.Deserialize(binaryReader));
	alignment = binaryDeserializer.GetAlignment();

	if (!cppEntityBlueprint)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize behavior tree info!");

		return false;
	}

	return true;
}

void CppEntityBlueprint::SerializeToJson(const std::string& outputFilePath)
{
	cppEntityBlueprint->SerializeToJson(outputFilePath);
}
