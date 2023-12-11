#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../ZString.h"
#include "../Serializer/ZBinarySerializer.h"

struct SEntityTemplatePropertyAlias
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SEntityTemplatePropertyAlias* DeserializeFromJson(const rapidjson::Value& object);

	ZString sAliasName;
	int entityID;
	ZString sPropertyName;
};
