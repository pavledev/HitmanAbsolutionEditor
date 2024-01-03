#pragma once

#include <iostream>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "../Containers/TArray.h"
#include "STemplateSubEntityBlueprint.h"
#include "SEntityTemplatePinConnection.h"
#include "../TypeInfo/STypeID.h"

struct STemplateEntityBlueprint
{
	void SerializeToJson(const std::string& outputFilePath);
	void SerializeToMemory(ZBinarySerializer& binarySerializer);
	static STemplateEntityBlueprint* DeserializeFromJson(const rapidjson::Document& document);

	int rootEntityIndex;
	TArray<STemplateSubEntityBlueprint> entityTemplates;
	TArray<SEntityTemplatePinConnection> pinConnections;
	TArray<SEntityTemplatePinConnection> inputPinForwardings;
	TArray<SEntityTemplatePinConnection> outputPinForwardings;
};
