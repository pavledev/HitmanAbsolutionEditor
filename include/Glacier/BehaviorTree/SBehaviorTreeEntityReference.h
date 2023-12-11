#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../ZString.h"

class ZBinarySerializer;

struct SBehaviorTreeEntityReference
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static void DeserializeFromJson(SBehaviorTreeEntityReference& behaviorTreeEntityReference, const rapidjson::Value& object);

	bool m_bList;
	ZString m_sName;
};
