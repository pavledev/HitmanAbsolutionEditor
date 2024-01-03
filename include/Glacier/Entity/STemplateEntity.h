#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "STemplateSubEntity.h"
#include "../Serializer/ZBinarySerializer.h"

struct STemplateEntity
{
	void SerializeToJson(const std::string& outputFilePath);
	void SerializeToMemory(ZBinarySerializer& binarySerializer);
	static STemplateEntity* DeserializeFromJson(const rapidjson::Document& document);

	int blueprintIndexInResourceHeader;
	int rootEntityIndex;
	TArray<STemplateSubEntity> entityTemplates;
};
