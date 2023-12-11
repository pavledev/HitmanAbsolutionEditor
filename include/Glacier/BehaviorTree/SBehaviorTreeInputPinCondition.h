#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../ZString.h"

class ZBinarySerializer;

struct SBehaviorTreeInputPinCondition
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static void DeserializeFromJson(SBehaviorTreeInputPinCondition& behaviorTreeInputPinCondition, const rapidjson::Value& object);

	ZString m_sName;
};
