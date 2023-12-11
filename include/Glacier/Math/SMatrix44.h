#pragma once

#include "SVector4.h"
#include "SQuaternion.h"

struct SMatrix44
{
	SMatrix44();
	SMatrix44(float m11, float m12, float m13, float m14,
		float m21, float m22, float m23, float m24,
		float m31, float m32, float m33, float m34,
		float m41, float m42, float m43, float m44);
	SMatrix44(const SMatrix33& other);
	SMatrix44(const SVector3& translation, const SQuaternion& rotation, const SVector3& scale);
	const SVector4& operator[](const unsigned int row) const;
	SVector4& operator[](const unsigned int row);
	SMatrix44 operator*(const SMatrix44& matrixB) const;
	void operator*=(const SMatrix44& matrix);
	SVector3 operator*(const SVector3& vector) const;
	static float Determinant(const SMatrix44& matrix);
	static SMatrix44 GetAdjointMatrix(const SMatrix44& matrix);
	SMatrix44 Inverted() const;
	SMatrix44 Transposed() const;
	static SMatrix44 Invert(const SMatrix44& matrix);
	static SMatrix44 Transpose(const SMatrix44& matrix);
	void Decompose(SVector3& translation, SQuaternion& rotation, SVector3& scale) const;
	void SetIdentity();
	static void Unit(SMatrix44& matrix);
	static SMatrix44 CreateTranslation(const SVector3& translation);
	static SMatrix44 CreateRotation(const SQuaternion& rotation);
	void Translate(const SVector3& translation);
	void Rescale(const SVector3& scale);

	union
	{
		struct
		{
			float m11;
			float m12;
			float m13;
			float m14;
			float m21;
			float m22;
			float m23;
			float m24;
			float m31;
			float m32;
			float m33;
			float m34;
			float m41;
			float m42;
			float m43;
			float m44;
		};

		SVector4 r[4];
		float v[16];
	};

	static const SMatrix44 Identity;
};
