#pragma once

#include "Glacier/BehaviorTree/SBehaviorTreeInfo.h"

#include "Resource.h"

class BehaviorTreeEntityBlueprint : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void Parse(void* behaviorTreeInfo);
	std::shared_ptr<SBehaviorTreeInfo> GetBehaviorTreeInfo();

private:
	std::shared_ptr<SBehaviorTreeInfo> behaviorTreeInfo;
};
