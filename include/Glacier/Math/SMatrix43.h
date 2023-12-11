#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "SVector3.h"
#include "SMatrix33.h"

struct SMatrix43
{
	SMatrix43();
	SMatrix43(const SMatrix43& other);
	SMatrix43& operator=(const SMatrix43& other);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SMatrix43* DeserializeFromJson(const rapidjson::Value& object);
	bool operator==(const SMatrix43& other) const;
	void Decompose(SVector3& position, SVector3& rotation, SVector3& scale);
	static SMatrix43 Recompose(SVector3& position, SVector3& rotation, SVector3& scale);

	union
	{
		struct
		{
			SVector3 XAxis;
			SVector3 YAxis;
			SVector3 ZAxis;
			SVector3 Trans;
		};

		struct
		{
			SMatrix33 Rot;
			SVector3 Trans;
		};

		struct
		{
			float m11;
			float m12;
			float m13;
			float m21;
			float m22;
			float m23;
			float m31;
			float m32;
			float m33;
			float m41;
			float m42;
			float m43;
		};

		float v[12];
		SVector3 r[4];
	};
};
