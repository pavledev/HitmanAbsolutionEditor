#pragma once

#include <cstddef>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "../ZString.h"

class ZBinarySerializer;

struct SEntityTemplateReference
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SEntityTemplateReference* DeserializeFromJson(const rapidjson::Value& object);
	bool operator==(const SEntityTemplateReference& other);

	int entityIndex;
	ZString exposedEntity;
};
