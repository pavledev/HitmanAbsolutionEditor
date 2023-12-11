#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "SEntityTemplateProperty.h"

struct SCppEntity
{
	std::string SerializeToJson();
	void SerializeToMemory(ZBinarySerializer& binarySerializer);
	static SCppEntity* DeserializeFromJson(const rapidjson::Document& document);

	int blueprintIndexInResourceHeader;
	TArray<SEntityTemplateProperty> propertyValues;
};
