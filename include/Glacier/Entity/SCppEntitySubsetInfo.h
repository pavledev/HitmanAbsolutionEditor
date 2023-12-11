#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../ZString.h"
#include "../TypeInfo/STypeID.h"

struct SCppEntitySubsetInfo
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SCppEntitySubsetInfo* DeserializeFromJson(const rapidjson::Value& object);

	ZString name;
	STypeID* type;
	unsigned int flags;
};
