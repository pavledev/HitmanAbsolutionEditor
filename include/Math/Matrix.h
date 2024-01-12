#pragma once

#include "Glacier/Math/SMatrix44.h"

#include "Math.h"
#include "Quaternion.h"
#include "Vector4.h"

class Matrix
{
public:
    Matrix()
    {
        SetIdentity();
    }

    Matrix(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        this->m00 = m00; this->m01 = m01; this->m02 = m02; this->m03 = m03;
        this->m10 = m10; this->m11 = m11; this->m12 = m12; this->m13 = m13;
        this->m20 = m20; this->m21 = m21; this->m22 = m22; this->m23 = m23;
        this->m30 = m30; this->m31 = m31; this->m32 = m32; this->m33 = m33;
    }

    Matrix(const SMatrix44& other)
    {
        m00 = other.m11; m01 = other.m12; m02 = other.m13; m03 = other.m14;
        m10 = other.m21; m11 = other.m22; m12 = other.m23; m13 = other.m24;
        m20 = other.m31; m21 = other.m32; m22 = other.m33; m23 = other.m34;
        m30 = other.m41; m31 = other.m42; m32 = other.m43; m33 = other.m44;
    }

    Matrix(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
    {
        const Matrix rotationMatrix = CreateRotation(rotation);

        m00 = scale.x * rotationMatrix.m00;  m01 = scale.x * rotationMatrix.m01;  m02 = scale.x * rotationMatrix.m02;  m03 = 0.0f;
        m10 = scale.y * rotationMatrix.m10;  m11 = scale.y * rotationMatrix.m11;  m12 = scale.y * rotationMatrix.m12;  m13 = 0.0f;
        m20 = scale.z * rotationMatrix.m20;  m21 = scale.z * rotationMatrix.m21;  m22 = scale.z * rotationMatrix.m22;  m23 = 0.0f;
        m30 = translation.x;                 m31 = translation.y;                 m32 = translation.z;                 m33 = 1.0f;
    }

    static Matrix GenerateRowFirst(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
    {
        const Matrix rotationMatrix = CreateRotation(rotation).Transposed();

        return Matrix
        (
            scale.x * rotationMatrix.m00, scale.y * rotationMatrix.m01, scale.z * rotationMatrix.m02, position.x,
            scale.x * rotationMatrix.m10, scale.y * rotationMatrix.m11, scale.z * rotationMatrix.m12, position.y,
            scale.x * rotationMatrix.m20, scale.y * rotationMatrix.m21, scale.z * rotationMatrix.m22, position.z,
            0.f, 0.f, 0.f, 1.f
        );
    }

    Vector3 GetTranslation() const
    {
        return Vector3(m30, m31, m32);
    }

    static Matrix CreateTranslation(const Vector3& translation)
    {
        return Matrix(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            translation.x, translation.y, translation.z, 1.0f
        );
    }

    static Matrix CreateRotation(const Quaternion& rotation)
    {
        const float num9 = rotation.x * rotation.x;
        const float num8 = rotation.y * rotation.y;
        const float num7 = rotation.z * rotation.z;
        const float num6 = rotation.x * rotation.y;
        const float num5 = rotation.z * rotation.w;
        const float num4 = rotation.z * rotation.x;
        const float num3 = rotation.y * rotation.w;
        const float num2 = rotation.y * rotation.z;
        const float num = rotation.x * rotation.w;

        return Matrix(
            1.0f - (2.0f * (num8 + num7)),
            2.0f * (num6 + num5),
            2.0f * (num4 - num3),
            0.0f,
            2.0f * (num6 - num5),
            1.0f - (2.0f * (num7 + num9)),
            2.0f * (num2 + num),
            0.0f,
            2.0f * (num4 + num3),
            2.0f * (num2 - num),
            1.0f - (2.0f * (num8 + num9)),
            0.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f
        );
    }

    Quaternion GetRotation() const
    {
        const Vector3 scale = GetScale();

        // avoid division by zero (we'll divide to remove scaling)
        if (scale.x == 0.0f || scale.y == 0.0f || scale.z == 0.0f)
            return Quaternion::Identity;

        // extract rotation and remove scaling
        Matrix normalized;
        normalized.m00 = m00 / scale.x; normalized.m01 = m01 / scale.x; normalized.m02 = m02 / scale.x; normalized.m03 = 0.0f;
        normalized.m10 = m10 / scale.y; normalized.m11 = m11 / scale.y; normalized.m12 = m12 / scale.y; normalized.m13 = 0.0f;
        normalized.m20 = m20 / scale.z; normalized.m21 = m21 / scale.z; normalized.m22 = m22 / scale.z; normalized.m23 = 0.0f;
        normalized.m30 = 0;             normalized.m31 = 0;             normalized.m32 = 0;             normalized.m33 = 1.0f;

        return RotationMatrixToQuaternion(normalized);
    }

    static Quaternion RotationMatrixToQuaternion(const Matrix& mRot)
    {
        Quaternion quaternion;
        float sqrt;
        float half;
        const float scale = mRot.m00 + mRot.m11 + mRot.m22;

        if (scale > 0.0f)
        {
            sqrt = Math::Sqrt(scale + 1.0f);
            quaternion.w = sqrt * 0.5f;
            sqrt = 0.5f / sqrt;

            quaternion.x = (mRot.m12 - mRot.m21) * sqrt;
            quaternion.y = (mRot.m20 - mRot.m02) * sqrt;
            quaternion.z = (mRot.m01 - mRot.m10) * sqrt;

            return quaternion;
        }

        if ((mRot.m00 >= mRot.m11) && (mRot.m00 >= mRot.m22))
        {
            sqrt = Math::Sqrt(1.0f + mRot.m00 - mRot.m11 - mRot.m22);
            half = 0.5f / sqrt;

            quaternion.x = 0.5f * sqrt;
            quaternion.y = (mRot.m01 + mRot.m10) * half;
            quaternion.z = (mRot.m02 + mRot.m20) * half;
            quaternion.w = (mRot.m12 - mRot.m21) * half;

            return quaternion;
        }

        if (mRot.m11 > mRot.m22)
        {
            sqrt = Math::Sqrt(1.0f + mRot.m11 - mRot.m00 - mRot.m22);
            half = 0.5f / sqrt;

            quaternion.x = (mRot.m10 + mRot.m01) * half;
            quaternion.y = 0.5f * sqrt;
            quaternion.z = (mRot.m21 + mRot.m12) * half;
            quaternion.w = (mRot.m20 - mRot.m02) * half;

            return quaternion;
        }

        sqrt = Math::Sqrt(1.0f + mRot.m22 - mRot.m00 - mRot.m11);
        half = 0.5f / sqrt;

        quaternion.x = (mRot.m20 + mRot.m02) * half;
        quaternion.y = (mRot.m21 + mRot.m12) * half;
        quaternion.z = 0.5f * sqrt;
        quaternion.w = (mRot.m01 - mRot.m10) * half;

        return quaternion;
    }

    Vector3 GetScale() const
    {
        const int xs = (Math::Sign(m00 * m01 * m02 * m03) < 0) ? -1 : 1;
        const int ys = (Math::Sign(m10 * m11 * m12 * m13) < 0) ? -1 : 1;
        const int zs = (Math::Sign(m20 * m21 * m22 * m23) < 0) ? -1 : 1;

        return Vector3(
            static_cast<float>(xs) * Math::Sqrt(m00 * m00 + m01 * m01 + m02 * m02),
            static_cast<float>(ys) * Math::Sqrt(m10 * m10 + m11 * m11 + m12 * m12),
            static_cast<float>(zs) * Math::Sqrt(m20 * m20 + m21 * m21 + m22 * m22)
        );
    }

    static Matrix CreateScale(float scale)
    {
        return CreateScale(scale, scale, scale);
    }

    static Matrix CreateScale(const Vector3& scale)
    {
        return CreateScale(scale.x, scale.y, scale.z);
    }

    static Matrix CreateScale(float scaleX, float scaleY, float ScaleZ)
    {
        return Matrix(
            scaleX, 0, 0, 0,
            0, scaleY, 0, 0,
            0, 0, ScaleZ, 0,
            0, 0, 0, 1
        );
    }

    static Matrix CreateLookAtRH(const Vector3& cameraPosition, const Vector3& target, const Vector3& up)
    {
        Vector3 const f((target - cameraPosition).Normalized());
        Vector3 const s(Vector3::Normalize(Vector3::Cross(f, up)));
        Vector3 const u(Vector3::Cross(s, f));

        return Matrix(
            s.x, u.x, -f.x, 0.f,
            s.y, u.y, -f.y, 0.f,
            s.z, u.z, -f.z, 0.f,
            -Vector3::Dot(s, cameraPosition), -Vector3::Dot(u, cameraPosition), Vector3::Dot(f, cameraPosition), 1.f
        );
    }

    static Matrix CreatePerspectiveFieldOfViewRH(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
    {
        float const tanHalfFovy = std::tan(fieldOfView / 2.f);

        return Matrix(
            1.f / (aspectRatio * tanHalfFovy), 0.f, 0.f, 0.f,
            0.f, 1.f / (tanHalfFovy), 0.f, 0.f,
            0.f, 0.f, farPlane / (nearPlane - farPlane), -1.f,
            0.f, 0.f, -(farPlane * nearPlane) / (farPlane - nearPlane), 0.f
        );
    }

    Matrix Transposed() const
    {
        return Transpose(*this);
    }

    void Transpose()
    {
        *this = Transpose(*this);
    }

    static Matrix Transpose(const Matrix& matrix)
    {
        return Matrix(
            matrix.m00, matrix.m10, matrix.m20, matrix.m30,
            matrix.m01, matrix.m11, matrix.m21, matrix.m31,
            matrix.m02, matrix.m12, matrix.m22, matrix.m32,
            matrix.m03, matrix.m13, matrix.m23, matrix.m33
        );
    }

    Matrix Inverted() const
    {
        return Invert(*this);
    }

    static Matrix Invert(const Matrix& matrix)
    {
        float v0 = matrix.m20 * matrix.m31 - matrix.m21 * matrix.m30;
        float v1 = matrix.m20 * matrix.m32 - matrix.m22 * matrix.m30;
        float v2 = matrix.m20 * matrix.m33 - matrix.m23 * matrix.m30;
        float v3 = matrix.m21 * matrix.m32 - matrix.m22 * matrix.m31;
        float v4 = matrix.m21 * matrix.m33 - matrix.m23 * matrix.m31;
        float v5 = matrix.m22 * matrix.m33 - matrix.m23 * matrix.m32;

        float i00 = (v5 * matrix.m11 - v4 * matrix.m12 + v3 * matrix.m13);
        float i10 = -(v5 * matrix.m10 - v2 * matrix.m12 + v1 * matrix.m13);
        float i20 = (v4 * matrix.m10 - v2 * matrix.m11 + v0 * matrix.m13);
        float i30 = -(v3 * matrix.m10 - v1 * matrix.m11 + v0 * matrix.m12);

        const float invDet = 1.0f / (i00 * matrix.m00 + i10 * matrix.m01 + i20 * matrix.m02 + i30 * matrix.m03);

        i00 *= invDet;
        i10 *= invDet;
        i20 *= invDet;
        i30 *= invDet;

        const float i01 = -(v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
        const float i11 = (v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
        const float i21 = -(v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
        const float i31 = (v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

        v0 = matrix.m10 * matrix.m31 - matrix.m11 * matrix.m30;
        v1 = matrix.m10 * matrix.m32 - matrix.m12 * matrix.m30;
        v2 = matrix.m10 * matrix.m33 - matrix.m13 * matrix.m30;
        v3 = matrix.m11 * matrix.m32 - matrix.m12 * matrix.m31;
        v4 = matrix.m11 * matrix.m33 - matrix.m13 * matrix.m31;
        v5 = matrix.m12 * matrix.m33 - matrix.m13 * matrix.m32;

        const float i02 = (v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
        const float i12 = -(v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
        const float i22 = (v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
        const float i32 = -(v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

        v0 = matrix.m21 * matrix.m10 - matrix.m20 * matrix.m11;
        v1 = matrix.m22 * matrix.m10 - matrix.m20 * matrix.m12;
        v2 = matrix.m23 * matrix.m10 - matrix.m20 * matrix.m13;
        v3 = matrix.m22 * matrix.m11 - matrix.m21 * matrix.m12;
        v4 = matrix.m23 * matrix.m11 - matrix.m21 * matrix.m13;
        v5 = matrix.m23 * matrix.m12 - matrix.m22 * matrix.m13;

        const float i03 = -(v5 * matrix.m01 - v4 * matrix.m02 + v3 * matrix.m03) * invDet;
        const float i13 = (v5 * matrix.m00 - v2 * matrix.m02 + v1 * matrix.m03) * invDet;
        const float i23 = -(v4 * matrix.m00 - v2 * matrix.m01 + v0 * matrix.m03) * invDet;
        const float i33 = (v3 * matrix.m00 - v1 * matrix.m01 + v0 * matrix.m02) * invDet;

        return Matrix(
            i00, i01, i02, i03,
            i10, i11, i12, i13,
            i20, i21, i22, i23,
            i30, i31, i32, i33);
    }

    void Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const
    {
        translation = GetTranslation();
        scale = GetScale();
        rotation = GetRotation();
    }

    void SetIdentity()
    {
        m00 = 1; m01 = 0; m02 = 0; m03 = 0;
        m10 = 0; m11 = 1; m12 = 0; m13 = 0;
        m20 = 0; m21 = 0; m22 = 1; m23 = 0;
        m30 = 0; m31 = 0; m32 = 0; m33 = 1;
    }

    Matrix operator*(const Matrix& rhs) const
    {
        return Matrix(
            m00 * rhs.m00 + m01 * rhs.m10 + m02 * rhs.m20 + m03 * rhs.m30,
            m00 * rhs.m01 + m01 * rhs.m11 + m02 * rhs.m21 + m03 * rhs.m31,
            m00 * rhs.m02 + m01 * rhs.m12 + m02 * rhs.m22 + m03 * rhs.m32,
            m00 * rhs.m03 + m01 * rhs.m13 + m02 * rhs.m23 + m03 * rhs.m33,
            m10 * rhs.m00 + m11 * rhs.m10 + m12 * rhs.m20 + m13 * rhs.m30,
            m10 * rhs.m01 + m11 * rhs.m11 + m12 * rhs.m21 + m13 * rhs.m31,
            m10 * rhs.m02 + m11 * rhs.m12 + m12 * rhs.m22 + m13 * rhs.m32,
            m10 * rhs.m03 + m11 * rhs.m13 + m12 * rhs.m23 + m13 * rhs.m33,
            m20 * rhs.m00 + m21 * rhs.m10 + m22 * rhs.m20 + m23 * rhs.m30,
            m20 * rhs.m01 + m21 * rhs.m11 + m22 * rhs.m21 + m23 * rhs.m31,
            m20 * rhs.m02 + m21 * rhs.m12 + m22 * rhs.m22 + m23 * rhs.m32,
            m20 * rhs.m03 + m21 * rhs.m13 + m22 * rhs.m23 + m23 * rhs.m33,
            m30 * rhs.m00 + m31 * rhs.m10 + m32 * rhs.m20 + m33 * rhs.m30,
            m30 * rhs.m01 + m31 * rhs.m11 + m32 * rhs.m21 + m33 * rhs.m31,
            m30 * rhs.m02 + m31 * rhs.m12 + m32 * rhs.m22 + m33 * rhs.m32,
            m30 * rhs.m03 + m31 * rhs.m13 + m32 * rhs.m23 + m33 * rhs.m33
        );
    }

    void operator*=(const Matrix& rhs)
    {
        (*this) = (*this) * rhs;
    }

    Vector3 operator*(const Vector3& rhs) const
    {
        Vector4 vWorking;

        vWorking.x = (rhs.x * m00) + (rhs.y * m10) + (rhs.z * m20) + m30;
        vWorking.y = (rhs.x * m01) + (rhs.y * m11) + (rhs.z * m21) + m31;
        vWorking.z = (rhs.x * m02) + (rhs.y * m12) + (rhs.z * m22) + m32;
        vWorking.w = 1 / ((rhs.x * m03) + (rhs.y * m13) + (rhs.z * m23) + m33);

        return Vector3(vWorking.x * vWorking.w, vWorking.y * vWorking.w, vWorking.z * vWorking.w);
    }

    Vector4 operator*(const Vector4& rhs) const
    {
        return Vector4
        (
            (rhs.x * m00) + (rhs.y * m10) + (rhs.z * m20) + (rhs.w * m30),
            (rhs.x * m01) + (rhs.y * m11) + (rhs.z * m21) + (rhs.w * m31),
            (rhs.x * m02) + (rhs.y * m12) + (rhs.z * m22) + (rhs.w * m32),
            (rhs.x * m03) + (rhs.y * m13) + (rhs.z * m23) + (rhs.w * m33)
        );
    }

    bool operator==(const Matrix& rhs) const
    {
        const float* leftData = Data();
        const float* rightData = rhs.Data();

        for (unsigned i = 0; i < 16; ++i)
        {
            if (leftData[i] != rightData[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const Matrix& rhs) const
    {
        return !(*this == rhs);
    }

    bool Equals(const Matrix& rhs)
    {
        const float* leftData = Data();
        const float* rightData = rhs.Data();

        for (unsigned i = 0; i < 16; ++i)
        {
            if (!Math::Equals(leftData[i], rightData[i]))
            {
                return false;
            }
        }

        return true;
    }

    const float* Data() const
    {
        return &m00;
    }

    union
    {
        struct
        {
            float m00, m10, m20, m30;
            float m01, m11, m21, m31;
            float m02, m12, m22, m32;
            float m03, m13, m23, m33;
        };

        Vector4 m[4];
    };

    static const Matrix Identity;
};

// Reverse order operators
inline Vector3 operator*(const Vector3& lhs, const Matrix& other)
{
    return other * lhs;
}

inline Vector4 operator*(const Vector4& lhs, const Matrix& other)
{
    return other * lhs;
}
