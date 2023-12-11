#pragma once

#include "Glacier/Math/SMatrix44.h"

#include "Math.h"
#include "Quaternion.h"
#include "Vector4.h"
#include "Matrix33.h"

class Matrix44
{
public:
    Matrix44()
    {
        SetIdentity();
    }

    Matrix44(
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

    Matrix44(const Matrix33& other)
    {
        SetIdentity();

        m00 = other.m00; m01 = other.m01; m02 = other.m02;
        m10 = other.m10; m11 = other.m11; m12 = other.m12;
        m20 = other.m20; m21 = other.m21; m22 = other.m22;
    }

    Matrix44(const SMatrix44& other)
    {
        m00 = other.m11; m01 = other.m12; m02 = other.m13; m03 = other.m14;
        m10 = other.m21; m11 = other.m22; m12 = other.m23; m13 = other.m24;
        m20 = other.m31; m21 = other.m32; m22 = other.m33; m23 = other.m34;
        m30 = other.m41; m31 = other.m42; m32 = other.m43; m33 = other.m44;
    }

    Matrix44(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
    {
        const Matrix44 rotationMatrix = CreateRotation(rotation);

        m00 = scale.x * rotationMatrix.m00;  m01 = scale.x * rotationMatrix.m01;  m02 = scale.x * rotationMatrix.m02;  m03 = 0.0f;
        m10 = scale.y * rotationMatrix.m10;  m11 = scale.y * rotationMatrix.m11;  m12 = scale.y * rotationMatrix.m12;  m13 = 0.0f;
        m20 = scale.z * rotationMatrix.m20;  m21 = scale.z * rotationMatrix.m21;  m22 = scale.z * rotationMatrix.m22;  m23 = 0.0f;
        m30 = translation.x;                 m31 = translation.y;                 m32 = translation.z;                 m33 = 1.0f;
    }

    const Vector4& operator[](const unsigned int index) const
    {
        return m[index];
    }

    Vector4& operator[](const unsigned int index)
    {
        return m[index];
    }

    static Matrix44 GenerateRowFirst(const Vector3& position, const Quaternion& rotation, const Vector3& scale)
    {
        const Matrix44 rotationMatrix = CreateRotation(rotation).Transposed();

        return Matrix44
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

    static Matrix44 CreateTranslation(const Vector3& translation)
    {
        return Matrix44(
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            translation.x, translation.y, translation.z, 1.0f
        );
    }

    static Matrix44 CreateRotation(const Quaternion& rotation)
    {
        const double q0 = M_SQRT2 * static_cast<double>(rotation[3]);
        const double q1 = M_SQRT2 * static_cast<double>(rotation[0]);
        const double q2 = M_SQRT2 * static_cast<double>(rotation[1]);
        const double q3 = M_SQRT2 * static_cast<double>(rotation[2]);

        const double qda = q0 * q1;
        const double qdb = q0 * q2;
        const double qdc = q0 * q3;
        const double qaa = q1 * q1;
        const double qab = q1 * q2;
        const double qac = q1 * q3;
        const double qbb = q2 * q2;
        const double qbc = q2 * q3;
        const double qcc = q3 * q3;

        return Matrix44
        (
            static_cast<float>(1.0 - qbb - qcc),
            static_cast<float>(qdc + qab),
            static_cast<float>(-qdb + qac),
            0.f,

            static_cast<float>(-qdc + qab),
            static_cast<float>(1.0 - qaa - qcc),
            static_cast<float>(qda + qbc),
            0.f,

            static_cast<float>(qdb + qac),
            static_cast<float>(-qda + qbc),
            static_cast<float>(1.0 - qaa - qbb),
            0.f,

            0.f,
            0.f,
            0.f,
            1.f
        );
    }

    Quaternion GetRotation() const
    {
        Vector3 scale;

        return GetRotation(scale);
    }

    Quaternion GetRotation(Vector3& scale) const
    {
        const Matrix44& matrix = *this;
        Matrix33 rotationMatrix = Matrix33(matrix);

        rotationMatrix[0] = rotationMatrix[0].Normalized(scale.x);
        rotationMatrix[1] = rotationMatrix[1].Normalized(scale.y);
        rotationMatrix[2] = rotationMatrix[2].Normalized(scale.z);

        return rotationMatrix.NormalizedMatrixToQuaternionWithChecks();
    }

    Vector3 GetScale() const
    {
        const Matrix44& matrix = *this;
        Matrix33 rotationMatrix = Matrix33(matrix);
        Vector3 scale;

        rotationMatrix[0].Normalized(scale.x);
        rotationMatrix[1].Normalized(scale.y);
        rotationMatrix[2].Normalized(scale.z);

        return scale;
    }

    static Matrix44 CreateScale(float scale)
    {
        return CreateScale(scale, scale, scale);
    }

    static Matrix44 CreateScale(const Vector3& scale)
    {
        return CreateScale(scale.x, scale.y, scale.z);
    }

    static Matrix44 CreateScale(float scaleX, float scaleY, float ScaleZ)
    {
        return Matrix44(
            scaleX, 0, 0, 0,
            0, scaleY, 0, 0,
            0, 0, ScaleZ, 0,
            0, 0, 0, 1
        );
    }

    static Matrix44 CreateLookAtRH(const Vector3& cameraPosition, const Vector3& target, const Vector3& up)
    {
        Vector3 const f((target - cameraPosition).Normalized());
        Vector3 const s(Vector3::Normalize(Vector3::Cross(f, up)));
        Vector3 const u(Vector3::Cross(s, f));

        return Matrix44(
            s.x, u.x, -f.x, 0.f,
            s.y, u.y, -f.y, 0.f,
            s.z, u.z, -f.z, 0.f,
            -Vector3::Dot(s, cameraPosition), -Vector3::Dot(u, cameraPosition), Vector3::Dot(f, cameraPosition), 1.f
        );
    }

    static Matrix44 CreatePerspectiveFieldOfViewRH(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
    {
        float const tanHalfFovy = std::tan(fieldOfView / 2.f);

        return Matrix44(
            1.f / (aspectRatio * tanHalfFovy), 0.f, 0.f, 0.f,
            0.f, 1.f / (tanHalfFovy), 0.f, 0.f,
            0.f, 0.f, farPlane / (nearPlane - farPlane), -1.f,
            0.f, 0.f, -(farPlane * nearPlane) / (farPlane - nearPlane), 0.f
        );
    }

    Matrix44 Transposed() const
    {
        return Transpose(*this);
    }

    void Transpose()
    {
        *this = Transpose(*this);
    }

    static Matrix44 Transpose(const Matrix44& matrix)
    {
        return Matrix44(
            matrix.m00, matrix.m10, matrix.m20, matrix.m30,
            matrix.m01, matrix.m11, matrix.m21, matrix.m31,
            matrix.m02, matrix.m12, matrix.m22, matrix.m32,
            matrix.m03, matrix.m13, matrix.m23, matrix.m33
        );
    }

    void Unit()
    {
        Unit(*this);
    }

    static void Unit(Matrix44& matrix)
    {
        matrix.SetIdentity();
    }

    float Determinant() const
    {
        return Determinant(*this);
    }

    static float Determinant(const Matrix44& matrix)
    {
        float a1 = matrix.m00;
        float b1 = matrix.m01;
        float c1 = matrix.m02;
        float d1 = matrix.m03;

        float a2 = matrix.m10;
        float b2 = matrix.m11;
        float c2 = matrix.m12;
        float d2 = matrix.m13;

        float a3 = matrix.m20;
        float b3 = matrix.m21;
        float c3 = matrix.m22;
        float d3 = matrix.m23;

        float a4 = matrix.m30;
        float b4 = matrix.m31;
        float c4 = matrix.m32;
        float d4 = matrix.m33;

        float determinant = a1 * Matrix33(b2, b3, b4, c2, c3, c4, d2, d3, d4).Determinant() -
            b1 * Matrix33(a2, a3, a4, c2, c3, c4, d2, d3, d4).Determinant() +
            c1 * Matrix33(a2, a3, a4, b2, b3, b4, d2, d3, d4).Determinant() -
            d1 * Matrix33(a2, a3, a4, b2, b3, b4, c2, c3, c4).Determinant();

        return determinant;

    }

    static Matrix44 GetAdjointMatrix(const Matrix44& matrix)
    {
        float a1 = matrix.m00;
        float b1 = matrix.m01;
        float c1 = matrix.m02;
        float d1 = matrix.m03;

        float a2 = matrix.m10;
        float b2 = matrix.m11;
        float c2 = matrix.m12;
        float d2 = matrix.m13;

        float a3 = matrix.m20;
        float b3 = matrix.m21;
        float c3 = matrix.m22;
        float d3 = matrix.m23;

        float a4 = matrix.m30;
        float b4 = matrix.m31;
        float c4 = matrix.m32;
        float d4 = matrix.m33;

        return Matrix44(
            Matrix33(b2, b3, b4, c2, c3, c4, d2, d3, d4).Determinant(),
            -Matrix33(b1, b3, b4, c1, c3, c4, d1, d3, d4).Determinant(),
            Matrix33(b1, b2, b4, c1, c2, c4, d1, d2, d4).Determinant(),
            -Matrix33(b1, b2, b3, c1, c2, c3, d1, d2, d3).Determinant(),

            -Matrix33(a2, a3, a4, c2, c3, c4, d2, d3, d4).Determinant(),
            Matrix33(a1, a3, a4, c1, c3, c4, d1, d3, d4).Determinant(),
            -Matrix33(a1, a2, a4, c1, c2, c4, d1, d2, d4).Determinant(),
            Matrix33(a1, a2, a3, c1, c2, c3, d1, d2, d3).Determinant(),

            Matrix33(a2, a3, a4, b2, b3, b4, d2, d3, d4).Determinant(),
            -Matrix33(a1, a3, a4, b1, b3, b4, d1, d3, d4).Determinant(),
            Matrix33(a1, a2, a4, b1, b2, b4, d1, d2, d4).Determinant(),
            -Matrix33(a1, a2, a3, b1, b2, b3, d1, d2, d3).Determinant(),

            -Matrix33(a2, a3, a4, b2, b3, b4, c2, c3, c4).Determinant(),
            Matrix33(a1, a3, a4, b1, b3, b4, c1, c3, c4).Determinant(),
            -Matrix33(a1, a2, a4, b1, b2, b4, c1, c2, c4).Determinant(),
            Matrix33(a1, a2, a3, b1, b2, b3, c1, c2, c3).Determinant()
        );
    }

    Matrix44 Inverted() const
    {
        return Invert(*this);
    }

    static Matrix44 Invert(const Matrix44& matrix)
    {
        float determinant = Determinant(matrix);
        Matrix44 adjointMatrix;

        if (determinant == 0.f)
        {
            const float eps = PSEUDOINVERSE_EPSILON;
            Matrix44 matrix2 = matrix;

            matrix2[0][0] += eps;
            matrix2[1][1] += eps;
            matrix2[2][2] += eps;
            matrix2[3][3] += eps;

            determinant = Determinant(matrix);

            if (determinant == 0.f)
            {
                Unit(matrix2);
                determinant = 1.0f;
            }

            adjointMatrix = GetAdjointMatrix(matrix2);
        }
        else
        {
            adjointMatrix = GetAdjointMatrix(matrix);
        }

        Matrix44 result;

        for (int i = 0; i < 4; ++i)
        {
            for (int j = 0; j < 4; ++j)
            {
                result[j][i] = adjointMatrix[i][j] / determinant;
            }
        }

        return result;
    }

    void Translate(const Vector3& translation)
    {
        Matrix44& matrix = *this;

        matrix.m30 += (translation.x * matrix.m00 + translation.y * matrix.m10 + translation.z * matrix.m20);
        matrix.m31 += (translation.x * matrix.m01 + translation.y * matrix.m11 + translation.z * matrix.m21);
        matrix.m32 += (translation.x * matrix.m02 + translation.y * matrix.m12 + translation.z * matrix.m22);
    }

    void Rescale(const Vector3& scale)
    {
        Matrix44& matrix = *this;

        matrix[0] = matrix[0] * scale.x;
        matrix[1] = matrix[1] * scale.y;
        matrix[2] = matrix[2] * scale.z;
    }

    void Decompose(Vector3& translation, Quaternion& rotation, Vector3& scale) const
    {
        translation = GetTranslation();
        rotation = GetRotation(scale);
    }

    void SetIdentity()
    {
        m00 = 1; m01 = 0; m02 = 0; m03 = 0;
        m10 = 0; m11 = 1; m12 = 0; m13 = 0;
        m20 = 0; m21 = 0; m22 = 1; m23 = 0;
        m30 = 0; m31 = 0; m32 = 0; m33 = 1;
    }

    Matrix44 operator*(const Matrix44& other) const
    {
        return Matrix44(
            m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30,
            m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31,
            m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32,
            m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33,
            m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30,
            m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31,
            m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32,
            m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33,
            m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30,
            m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31,
            m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32,
            m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33,
            m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30,
            m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31,
            m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32,
            m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33
        );
    }

    void operator*=(const Matrix44& other)
    {
        (*this) = (*this) * other;
    }

    Vector3 operator*(const Vector3& other) const
    {
        return Vector3(
            other.x * m00 + other.y * m10 + other.z * m20 + m30,
            other.x * m01 + other.y * m11 + other.z * m21 + m31,
            other.x * m02 + other.y * m12 + other.z * m22 + m32
        );
    }

    Vector4 operator*(const Vector4& other) const
    {
        return Vector4(
            other.x * m00 + other.y * m10 + other.z * m20 + other.w * m30,
            other.x * m01 + other.y * m11 + other.z * m21 + other.w * m31,
            other.x * m02 + other.y * m12 + other.z * m22 + other.w * m32,
            other.x * m03 + other.y * m13 + other.z * m23 + other.w * m33
        );
    }

    bool operator==(const Matrix44& other) const
    {
        const float* leftData = Data();
        const float* rightData = other.Data();

        for (unsigned int i = 0; i < 16; ++i)
        {
            if (leftData[i] != rightData[i])
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const Matrix44& other) const
    {
        return !(*this == other);
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

    static const Matrix44 Identity;
};

// Reverse order operators
inline Vector3 operator*(const Vector3& lhs, const Matrix44& other)
{
    return other * lhs;
}

inline Vector4 operator*(const Vector4& lhs, const Matrix44& other)
{
    return other * lhs;
}
