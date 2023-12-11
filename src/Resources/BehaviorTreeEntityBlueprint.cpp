#include "Resources/BehaviorTreeEntityBlueprint.h"
#include "Glacier/Serializer/ZBinaryDeserializer.h"
#include "Logger.h"

BehaviorTreeEntityBlueprint::~BehaviorTreeEntityBlueprint()
{
	operator delete(behaviorTreeInfo, std::align_val_t(alignment));
}

SBehaviorTreeInfo* BehaviorTreeEntityBlueprint::GetBehaviorTreeInfo()
{
	return behaviorTreeInfo;
}

bool BehaviorTreeEntityBlueprint::Deserialize(const std::string& filePath)
{
	BinaryReader binaryReader = BinaryReader(filePath);

	return Deserialize(binaryReader);
}

bool BehaviorTreeEntityBlueprint::Deserialize(void* data, const unsigned int dataSize)
{
	BinaryReader binaryReader = BinaryReader(data, dataSize);

	return Deserialize(binaryReader);
}

bool BehaviorTreeEntityBlueprint::Deserialize(BinaryReader& binaryReader)
{
	ZBinaryDeserializer binaryDeserializer;

	behaviorTreeInfo = static_cast<SBehaviorTreeInfo*>(binaryDeserializer.Deserialize(binaryReader));
	alignment = binaryDeserializer.GetAlignment();

	if (!behaviorTreeInfo)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize behavior tree info!");

		return false;
	}

	return true;
}

void BehaviorTreeEntityBlueprint::SerializeToJson(const std::string& outputFilePath)
{
	behaviorTreeInfo->SerializeToJson(outputFilePath);
}
