#include <complex>

#include "Glacier/Math/SMatrix44.h"
#include "Glacier/Math/SMatrix33.h"

#include "Utility/Math.h"

const SMatrix44 SMatrix44::Identity
(
    1.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 0.f,
    0.f, 0.f, 1.f, 0.f,
    0.f, 0.f, 0.f, 1.f
);

SMatrix44::SMatrix44()
{
    m11 = 0.0f;
    m12 = 0.0f;
    m13 = 0.0f;
    m14 = 0.0f;
    m21 = 0.0f;
    m22 = 0.0f;
    m23 = 0.0f;
    m24 = 0.0f;
    m31 = 0.0f;
    m32 = 0.0f;
    m33 = 0.0f;
    m34 = 0.0f;
    m41 = 0.0f;
    m42 = 0.0f;
    m43 = 0.0f;
    m44 = 0.0f;
}

SMatrix44::SMatrix44(float m11, float m12, float m13, float m14,
    float m21, float m22, float m23, float m24,
    float m31, float m32, float m33, float m34,
    float m41, float m42, float m43, float m44)
{
    this->m11 = m11;
    this->m12 = m12;
    this->m13 = m13;
    this->m14 = m14;
    this->m21 = m21;
    this->m22 = m22;
    this->m23 = m23;
    this->m24 = m24;
    this->m31 = m31;
    this->m32 = m32;
    this->m33 = m33;
    this->m34 = m34;
    this->m41 = m41;
    this->m42 = m42;
    this->m43 = m43;
    this->m44 = m44;
}

SMatrix44::SMatrix44(const SMatrix33& other)
{
    SetIdentity();

    this->m11 = other.m11; this->m12 = other.m12; this->m13 = other.m13;
    this->m21 = other.m21; this->m22 = other.m22; this->m23 = other.m23;
    this->m31 = other.m31; this->m32 = other.m32; this->m33 = other.m33;
}

SMatrix44::SMatrix44(const SVector3& translation, const SQuaternion& rotation, const SVector3& scale)
{
    const SMatrix44 rotationMatrix = CreateRotation(rotation);

    m11 = scale.x * rotationMatrix.m11;  m12 = scale.x * rotationMatrix.m12;  m13 = scale.x * rotationMatrix.m13;  m14 = 0.0f;
    m21 = scale.y * rotationMatrix.m21;  m22 = scale.y * rotationMatrix.m22;  m23 = scale.y * rotationMatrix.m23;  m24 = 0.0f;
    m31 = scale.z * rotationMatrix.m31;  m32 = scale.z * rotationMatrix.m32;  m33 = scale.z * rotationMatrix.m33;  m34 = 0.0f;
    m41 = translation.x;                 m42 = translation.y;                 m43 = translation.z;                 m44 = 1.0f;
}

const SVector4& SMatrix44::operator[](const unsigned int row) const
{
    return r[row];
}

SVector4& SMatrix44::operator[](const unsigned int row)
{
    return r[row];
}

SMatrix44 SMatrix44::operator*(const SMatrix44& matrixB) const
{
    const SMatrix44& matrixA = *this;

    return SMatrix44(
        matrixA[0][0] * matrixB[0][0] + matrixA[0][1] * matrixB[1][0] + matrixA[0][2] * matrixB[2][0] + matrixA[0][3] * matrixB[3][0],
        matrixA[0][0] * matrixB[0][1] + matrixA[0][1] * matrixB[1][1] + matrixA[0][2] * matrixB[2][1] + matrixA[0][3] * matrixB[3][1],
        matrixA[0][0] * matrixB[0][2] + matrixA[0][1] * matrixB[1][2] + matrixA[0][2] * matrixB[2][2] + matrixA[0][3] * matrixB[3][2],
        matrixA[0][0] * matrixB[0][3] + matrixA[0][1] * matrixB[1][3] + matrixA[0][2] * matrixB[2][3] + matrixA[0][3] * matrixB[3][3],

        matrixA[1][0] * matrixB[0][0] + matrixA[1][1] * matrixB[1][0] + matrixA[1][2] * matrixB[2][0] + matrixA[1][3] * matrixB[3][0],
        matrixA[1][0] * matrixB[0][1] + matrixA[1][1] * matrixB[1][1] + matrixA[1][2] * matrixB[2][1] + matrixA[1][3] * matrixB[3][1],
        matrixA[1][0] * matrixB[0][2] + matrixA[1][1] * matrixB[1][2] + matrixA[1][2] * matrixB[2][2] + matrixA[1][3] * matrixB[3][2],
        matrixA[1][0] * matrixB[0][3] + matrixA[1][1] * matrixB[1][3] + matrixA[1][2] * matrixB[2][3] + matrixA[1][3] * matrixB[3][3],

        matrixA[2][0] * matrixB[0][0] + matrixA[2][1] * matrixB[1][0] + matrixA[2][2] * matrixB[2][0] + matrixA[2][3] * matrixB[3][0],
        matrixA[2][0] * matrixB[0][1] + matrixA[2][1] * matrixB[1][1] + matrixA[2][2] * matrixB[2][1] + matrixA[2][3] * matrixB[3][1],
        matrixA[2][0] * matrixB[0][2] + matrixA[2][1] * matrixB[1][2] + matrixA[2][2] * matrixB[2][2] + matrixA[2][3] * matrixB[3][2],
        matrixA[2][0] * matrixB[0][3] + matrixA[2][1] * matrixB[1][3] + matrixA[2][2] * matrixB[2][3] + matrixA[2][3] * matrixB[3][3],

        matrixA[3][0] * matrixB[0][0] + matrixA[3][1] * matrixB[1][0] + matrixA[3][2] * matrixB[2][0] + matrixA[3][3] * matrixB[3][0],
        matrixA[3][0] * matrixB[0][1] + matrixA[3][1] * matrixB[1][1] + matrixA[3][2] * matrixB[2][1] + matrixA[3][3] * matrixB[3][1],
        matrixA[3][0] * matrixB[0][2] + matrixA[3][1] * matrixB[1][2] + matrixA[3][2] * matrixB[2][2] + matrixA[3][3] * matrixB[3][2],
        matrixA[3][0] * matrixB[0][3] + matrixA[3][1] * matrixB[1][3] + matrixA[3][2] * matrixB[2][3] + matrixA[3][3] * matrixB[3][3]
    );
}

void SMatrix44::operator*=(const SMatrix44& matrix)
{
    (*this) = (*this) * matrix;
}

SVector3 SMatrix44::operator*(const SVector3& vector) const
{
    const SMatrix44& matrix = *this;
    SVector3 result;
    const float x = vector[0];
    const float y = vector[1];
    const float z = vector[2];

    result[0] = x * matrix[0][0] + y * matrix[0][1] + matrix[0][2] * z + matrix[0][3];
    result[1] = x * matrix[1][0] + y * matrix[1][1] + matrix[1][2] * z + matrix[1][3];
    result[2] = x * matrix[2][0] + y * matrix[2][1] + matrix[2][2] * z + matrix[2][3];

    return result;
}

float SMatrix44::Determinant(const SMatrix44& matrix)
{
    float a1 = matrix[0][0];
    float b1 = matrix[0][1];
    float c1 = matrix[0][2];
    float d1 = matrix[0][3];

    float a2 = matrix[1][0];
    float b2 = matrix[1][1];
    float c2 = matrix[1][2];
    float d2 = matrix[1][3];

    float a3 = matrix[2][0];
    float b3 = matrix[2][1];
    float c3 = matrix[2][2];
    float d3 = matrix[2][3];

    float a4 = matrix[3][0];
    float b4 = matrix[3][1];
    float c4 = matrix[3][2];
    float d4 = matrix[3][3];

    float determinant = a1 * SMatrix33(b2, b3, b4, c2, c3, c4, d2, d3, d4).Determinant() -
        b1 * SMatrix33(a2, a3, a4, c2, c3, c4, d2, d3, d4).Determinant() +
        c1 * SMatrix33(a2, a3, a4, b2, b3, b4, d2, d3, d4).Determinant() -
        d1 * SMatrix33(a2, a3, a4, b2, b3, b4, c2, c3, c4).Determinant();

    return determinant;

}

SMatrix44 SMatrix44::GetAdjointMatrix(const SMatrix44& matrix)
{
    float a1 = matrix[0][0];
    float b1 = matrix[0][1];
    float c1 = matrix[0][2];
    float d1 = matrix[0][3];

    float a2 = matrix[1][0];
    float b2 = matrix[1][1];
    float c2 = matrix[1][2];
    float d2 = matrix[1][3];

    float a3 = matrix[2][0];
    float b3 = matrix[2][1];
    float c3 = matrix[2][2];
    float d3 = matrix[2][3];

    float a4 = matrix[3][0];
    float b4 = matrix[3][1];
    float c4 = matrix[3][2];
    float d4 = matrix[3][3];

    return SMatrix44(
        SMatrix33(b2, b3, b4, c2, c3, c4, d2, d3, d4).Determinant(),
        -SMatrix33(b1, b3, b4, c1, c3, c4, d1, d3, d4).Determinant(),
        SMatrix33(b1, b2, b4, c1, c2, c4, d1, d2, d4).Determinant(),
        -SMatrix33(b1, b2, b3, c1, c2, c3, d1, d2, d3).Determinant(),

        -SMatrix33(a2, a3, a4, c2, c3, c4, d2, d3, d4).Determinant(),
        SMatrix33(a1, a3, a4, c1, c3, c4, d1, d3, d4).Determinant(),
        -SMatrix33(a1, a2, a4, c1, c2, c4, d1, d2, d4).Determinant(),
        SMatrix33(a1, a2, a3, c1, c2, c3, d1, d2, d3).Determinant(),

        SMatrix33(a2, a3, a4, b2, b3, b4, d2, d3, d4).Determinant(),
        -SMatrix33(a1, a3, a4, b1, b3, b4, d1, d3, d4).Determinant(),
        SMatrix33(a1, a2, a4, b1, b2, b4, d1, d2, d4).Determinant(),
        -SMatrix33(a1, a2, a3, b1, b2, b3, d1, d2, d3).Determinant(),

        -SMatrix33(a2, a3, a4, b2, b3, b4, c2, c3, c4).Determinant(),
        SMatrix33(a1, a3, a4, b1, b3, b4, c1, c3, c4).Determinant(),
        -SMatrix33(a1, a2, a4, b1, b2, b4, c1, c2, c4).Determinant(),
        SMatrix33(a1, a2, a3, b1, b2, b3, c1, c2, c3).Determinant()
    );
}

SMatrix44 SMatrix44::Inverted() const
{
    return Invert(*this);
}

SMatrix44 SMatrix44::Transposed() const
{
    return Transpose(*this);
}

SMatrix44 SMatrix44::Invert(const SMatrix44& matrix)
{
    float determinant = Determinant(matrix);
    SMatrix44 adjointMatrix;

    if (determinant == 0.f)
    {
        const float eps = PSEUDOINVERSE_EPSILON;
        SMatrix44 matrix2 = matrix;

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

    SMatrix44 result;

    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            result[i][j] = adjointMatrix[i][j] / determinant;
        }
    }

    return result;
}

SMatrix44 SMatrix44::Transpose(const SMatrix44& matrix)
{
    SMatrix44 result;

    result.m11 = matrix.m11;
    result.m12 = matrix.m21;
    result.m13 = matrix.m31;
    result.m14 = matrix.m41;
    result.m21 = matrix.m12;
    result.m22 = matrix.m22;
    result.m23 = matrix.m32;
    result.m24 = matrix.m42;
    result.m31 = matrix.m13;
    result.m32 = matrix.m23;
    result.m33 = matrix.m33;
    result.m34 = matrix.m43;
    result.m41 = matrix.m14;
    result.m42 = matrix.m24;
    result.m43 = matrix.m34;
    result.m44 = matrix.m44;

    return result;
}

void SMatrix44::Decompose(SVector3& translation, SQuaternion& rotation, SVector3& scale) const
{
    const SMatrix44& matrix = *this;
    SMatrix33 rotationMatrix = SMatrix33(matrix);

    rotationMatrix[0] = rotationMatrix[0].Normalize(scale.x);
    rotationMatrix[1] = rotationMatrix[1].Normalize(scale.y);
    rotationMatrix[2] = rotationMatrix[2].Normalize(scale.z);

    //translation = SVector3(matrix.m14, matrix.m24, matrix.m34);
    translation = matrix[3];
    rotation = rotationMatrix.NormalizedMatrixToQuaternionWithChecks();
}

void SMatrix44::SetIdentity()
{
    m11 = 1; m12 = 0; m13 = 0; m14 = 0;
    m21 = 0; m22 = 1; m23 = 0; m24 = 0;
    m31 = 0; m32 = 0; m33 = 1; m34 = 0;
    m41 = 0; m42 = 0; m43 = 0; m44 = 1;
}

void SMatrix44::Unit(SMatrix44& matrix)
{
    matrix.SetIdentity();
}

SMatrix44 SMatrix44::CreateTranslation(const SVector3& translation)
{
    return SMatrix44(
        1.f, 0.f, 0.f, 0.f,
        0.f, 1.f, 0.f, 0.f,
        0.f, 0.f, 1.f, 0.f,
        translation.x, translation.y, translation.z, 1.f
    );
}

SMatrix44 SMatrix44::CreateRotation(const SQuaternion& rotation)
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

    return SMatrix44
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

void SMatrix44::Translate(const SVector3& translation)
{
    SMatrix44& matrix = *this;

    matrix[3][0] += (translation.x * matrix[0][0] + translation.y * matrix[1][0] + translation.z * matrix[2][0]);
    matrix[3][1] += (translation.x * matrix[0][1] + translation.y * matrix[1][1] + translation.z * matrix[2][1]);
    matrix[3][2] += (translation.x * matrix[0][2] + translation.y * matrix[1][2] + translation.z * matrix[2][2]);
}

void SMatrix44::Rescale(const SVector3& scale)
{
    SMatrix44& matrix = *this;

    matrix[0] = matrix[0] * scale.x;
    matrix[1] = matrix[1] * scale.y;
    matrix[2] = matrix[2] * scale.z;
}
