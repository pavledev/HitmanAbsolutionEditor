#pragma once

#include "Glacier/BehaviorTree/SBehaviorTreeInfo.h"

#include "Resource.h"

class BehaviorTreeEntityBlueprint : public Resource
{
public:
	void Deserialize() override;
	void Parse(void* behaviorTreeInfo);
	std::shared_ptr<SBehaviorTreeInfo> GetBehaviorTreeInfo();
	void SerializeToJson(const std::string& outputFilePath);

private:
	std::shared_ptr<SBehaviorTreeInfo> behaviorTreeInfo;
};
