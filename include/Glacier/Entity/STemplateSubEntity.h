#pragma once

#include "../Containers/TArray.h"
#include "SEntityTemplateProperty.h"

struct STemplateSubEntity
{
	STemplateSubEntity();
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int index);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static STemplateSubEntity* DeserializeFromJson(const rapidjson::Value& object);

	int parentIndex;
	int entityTypeResourceIndex;
	TArray<SEntityTemplateProperty> propertyValues;
	TArray<SEntityTemplateProperty> postInitPropertyValues;
};
