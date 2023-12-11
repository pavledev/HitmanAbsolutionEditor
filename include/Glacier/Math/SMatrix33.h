#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "SVector3.h"
#include "SQuaternion.h"

struct SMatrix44;

struct SMatrix33
{
	SMatrix33();
	SMatrix33(float m11, float m12, float m13,
		float m21, float m22, float m23,
		float m31, float m32, float m33);
	SMatrix33(const SMatrix44& matrix);
	SMatrix33(const SMatrix33& other);
	SMatrix33& operator=(const SMatrix33& other);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static SMatrix33* DeserializeFromJson(const rapidjson::Value& object);
	bool operator==(const SMatrix33& other) const;
	const SVector3& operator[](const unsigned int row) const;
	SVector3& operator[](const unsigned int row);
	SMatrix33 operator*(const SMatrix33& matrixB) const;
	SVector3 operator*(const SVector3& vector) const;
	SVector3 ConvertRotationMatrixToEulerAngles();
	static SMatrix33 ConvertEulerAnglesToRotationMatrix(const SVector3& vector);
	SQuaternion MatrixToQuaternion();
	SQuaternion NormalizedMatrixToQuaternionWithChecks();
	SQuaternion NormalizedMatrixToQuaternion();
	float Determinant() const;
	static float Determinant(const SMatrix33& matrix);
	void Normalize();
	void Unit();
	static void Unit(SMatrix33& matrix);
	void Negate();
	SMatrix33 Transposed() const;
	static SMatrix33 Transpose(const SMatrix33& matrix);
	static SMatrix33 GetAdjointMatrix(const SMatrix33& matrix);
	SMatrix33 Inverted() const;
	static SMatrix33 Invert(const SMatrix33& matrix);
	static SMatrix33 ScaleToMatrix(const SVector3& scale);
	static SMatrix33 NormalizedAxisAngleToMatrixEx(const SVector3& axis, const float angleSin, const float angleCos);
	static SMatrix33 NormalizedAxisAngleToMatrix(const SVector3& axis, const float angle);

	union
	{
		struct
		{
			SVector3 XAxis;
			SVector3 YAxis;
			SVector3 ZAxis;
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
		};

		float v[9];
		SVector3 r[3];
	};
};
