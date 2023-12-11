#pragma once

#include <string>

#include "Glacier/BehaviorTree/SBehaviorTreeInfo.h"

#include "IO/BinaryReader.h"

class BehaviorTreeEntityBlueprint
{
public:
	~BehaviorTreeEntityBlueprint();
	SBehaviorTreeInfo* GetBehaviorTreeInfo();
	bool Deserialize(const std::string& filePath);
	bool Deserialize(void* data, const unsigned int dataSize);
	bool Deserialize(BinaryReader& binaryReader);
	void SerializeToJson(const std::string& outputFilePath);

private:
	SBehaviorTreeInfo* behaviorTreeInfo;
	unsigned char alignment;
};
