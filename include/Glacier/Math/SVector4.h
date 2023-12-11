#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "SVector3.h"

class ZBinarySerializer;

struct SVector4
{
	SVector4();
	SVector4(const float x, const float y, const float z, const float w);
	const float& operator[](const unsigned int index) const;
	float& operator[](const unsigned int index);
	SVector4 operator*(const float value) const;
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SVector4* DeserializeFromJson(const rapidjson::Value& object);

	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};

		struct
		{
			SVector3 n;
			float d;
		};

		float v[4];
	};
};
