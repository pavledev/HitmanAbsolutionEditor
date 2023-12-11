#pragma once

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "../Math/float4.h"

class ZBinarySerializer;

struct SGProperties
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static void DeserializeFromJson(SGProperties& gProperties, const rapidjson::Value& object);

	float4 vMin;
	float4 vMax;
	int nGridWidth;
	float fGridSpacing;
};
