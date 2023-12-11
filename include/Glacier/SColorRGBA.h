#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

class ZBinarySerializer;

struct SColorRGBA
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SColorRGBA* DeserializeFromJson(const rapidjson::Value& object);
	bool operator==(const SColorRGBA& other) const;

	float r;
	float g;
	float b;
	float a;
};
