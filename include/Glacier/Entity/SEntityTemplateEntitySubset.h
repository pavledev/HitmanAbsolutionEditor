#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "../TypeInfo/STypeID.h"

struct SEntityTemplateEntitySubset
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SEntityTemplateEntitySubset* DeserializeFromJson(const rapidjson::Value& object);

	STypeID* subsetType;
	TArray<int> entities;
};
