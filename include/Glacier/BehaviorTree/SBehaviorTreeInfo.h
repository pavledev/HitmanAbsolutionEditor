#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "SBehaviorTreeEntityReference.h"
#include "SBehaviorTreeInputPinCondition.h"

class ZBinarySerializer;

struct SBehaviorTreeInfo
{
	void SerializeToJson(const std::string& outputFilePath);
	void SerializeToMemory(ZBinarySerializer& binarySerializer);
	static SBehaviorTreeInfo* DeserializeFromJson(const rapidjson::Document& document);

	TArray<SBehaviorTreeEntityReference> m_references;
	TArray<SBehaviorTreeInputPinCondition> m_inputPinConditions;
};
