#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../ZVariant.h"

class ZBinarySerializer;

struct SEntityTemplateProperty
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SEntityTemplateProperty* DeserializeFromJson(const rapidjson::Value& object);

	unsigned int nPropertyID;
	ZVariant value;
};
