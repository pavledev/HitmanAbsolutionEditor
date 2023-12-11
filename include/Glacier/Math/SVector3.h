#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

class ZBinarySerializer;

struct float4;
struct SVector4;

class Vector3;

struct SVector3
{
	SVector3();
	SVector3(const float x, const float y, const float z);
	SVector3(const float4& vector);
	SVector3(const SVector4& vector);
	SVector3(const Vector3& other);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SVector3* DeserializeFromJson(const rapidjson::Value& object);
	bool operator==(const SVector3& other) const;
	bool operator>(const SVector3& other) const;
	bool operator<(const SVector3& other) const;
	SVector3& operator*=(const float value);
	SVector3 operator*(const float value) const;
	SVector3& operator+=(const SVector3& other);
	SVector3 operator+(const SVector3& other) const;
	SVector3 operator-(const SVector3& other) const;
	SVector3 operator-(const float value) const;
	SVector3& operator-=(const SVector3& other);
	const float& operator[](const unsigned int index) const;
	float& operator[](const unsigned int index);
	SVector3 Normalize();
	SVector3 Normalize(float& length);
	static SVector3 Normalize(const SVector3& v);
	static void Normalize(SVector3& vector);
	static SVector3 Normalize(const SVector3& v, float& length);
	static void Normalize(SVector3& vector, float& length);
	bool IsNormalized() const;
	static float Dot(const SVector3& v1, const SVector3& v2);
	float Dot(const SVector3& rhs) const;
	static SVector3 Cross(const SVector3& vector, const SVector3& vector2);
	SVector3 Cross(const SVector3& vector) const;
	float Length() const;
	static float Length(const SVector3& vector, const SVector3& vector2);
	float SquaredLength() const;
	static SVector3 Negate(const SVector3& vector);
	void Negate();
	static float NormalizedAngle(const SVector3& vector, const SVector3& vector2);
	static SVector3 Ortho(const SVector3& vector);
	static int DominantAxis(const SVector3& vector);
	static SVector3 ProjectNormalized(const SVector3& p, const SVector3& proj);
	static float Angle(const SVector3& vector, const SVector3& vector2);

	union
	{
		struct
		{
			float x;
			float y;
			float z;
		};

		float v[3];
	};
};
