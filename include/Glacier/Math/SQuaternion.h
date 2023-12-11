#pragma once

#include "float4.h"
#include "SVector3.h"

struct SMatrix33;

class Quaternion;

struct SQuaternion
{
	SQuaternion();
	SQuaternion(float x, float y, float z, float w);
	SQuaternion Negate(const SQuaternion& quaternion) const;
	SQuaternion(const Quaternion& quaternion);
	const float& operator[](const unsigned int index) const;
	float& operator[](const unsigned int index);
	void Negate();
	float LengthSquared() const;
	float Dot(const SQuaternion& rhs) const;
	static float Dot(const SQuaternion& a, const SQuaternion& b);
	static SQuaternion Multiply(const SQuaternion& quaternionA, const SQuaternion& quaternionB);
	SQuaternion Normalized();
	SQuaternion Normalized(float& length);
	static SQuaternion Normalize(const SQuaternion& quaternion);
	static void Normalize(SQuaternion& quaternion);
	SMatrix33 ToMatrix() const;
	void Conjugate();
	SQuaternion Conjugated() const;
	static SQuaternion Conjugate(const SQuaternion& quaternion);
	SQuaternion operator*(const SQuaternion& rhs) const;
	SVector3 operator*(const SVector3& rhs) const;
	SQuaternion operator*(const float rhs) const;
	SQuaternion& operator*=(const float rhs);
	static SQuaternion RotationBetweenVectorsToQuaternion(const SVector3& vector, const SVector3& vector2);
	static SQuaternion AxisAngleToQuaternion(const SVector3& axis, const float angle);
	static SQuaternion NormalizedAxisAngleToQuaternion(const SVector3& axis, const float angle);
	static SQuaternion Unit();
	static SQuaternion RotationDifference(const SVector3& vector, const SVector3& vector2);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	float4 w128;
};
