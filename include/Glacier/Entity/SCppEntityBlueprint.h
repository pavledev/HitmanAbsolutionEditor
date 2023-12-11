#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "SCppEntitySubsetInfo.h"
#include "../TypeInfo/STypeID.h"

struct SCppEntityBlueprint
{
	void SerializeToJson(const std::string& outputFilePath);
	void SerializeToMemory(ZBinarySerializer& binarySerializer);
	static SCppEntityBlueprint* DeserializeFromJson(const rapidjson::Document& document);

	STypeID* typeName;
	TArray<SCppEntitySubsetInfo> subsets;
};
