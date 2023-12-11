#include <cmath>

#include "Glacier/Math/SMatrix33.h"
#include "Glacier/Math/SMatrix44.h"

#include "Utility/Math.h"

SMatrix33::SMatrix33()
{
}

SMatrix33::SMatrix33(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
{
    this->m11 = m11; this->m12 = m12; this->m13 = m13;
    this->m21 = m21; this->m22 = m22; this->m23 = m23;
    this->m31 = m31; this->m32 = m32; this->m33 = m33;
}

SMatrix33::SMatrix33(const SMatrix44& matrix)
{
    m11 = matrix.m11;
    m12 = matrix.m12;
    m13 = matrix.m13;
    m21 = matrix.m21;
    m22 = matrix.m22;
    m23 = matrix.m23;
    m31 = matrix.m31;
    m32 = matrix.m32;
    m33 = matrix.m33;
}

SMatrix33::SMatrix33(const SMatrix33& other)
{
    XAxis = other.XAxis;
    YAxis = other.YAxis;
    ZAxis = other.ZAxis;
}

SMatrix33& SMatrix33::operator=(const SMatrix33& other)
{
    if (this != &other)
    {
        XAxis = other.XAxis;
        YAxis = other.YAxis;
        ZAxis = other.ZAxis;
    }

    return *this;
}

void SMatrix33::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("XAxis");
    XAxis.SerializeToJson(writer);

    writer.String("YAxis");
    YAxis.SerializeToJson(writer);

    writer.String("ZAxis");
    ZAxis.SerializeToJson(writer);

    writer.EndObject();
}

void SMatrix33::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int xAxisOffset = offset + offsetof(SMatrix33, XAxis);
    unsigned int yAxisOffset = offset + offsetof(SMatrix33, YAxis);
    unsigned int zAxisOffset = offset + offsetof(SMatrix33, ZAxis);

    XAxis.SerializeToMemory(binarySerializer, xAxisOffset);
    YAxis.SerializeToMemory(binarySerializer, yAxisOffset);
    ZAxis.SerializeToMemory(binarySerializer, zAxisOffset);
}

SMatrix33* SMatrix33::DeserializeFromJson(const rapidjson::Value& object)
{
    SMatrix33* matrix = new SMatrix33();

    matrix->XAxis = *SVector3::DeserializeFromJson(object["XAxis"].GetObj());
    matrix->YAxis = *SVector3::DeserializeFromJson(object["YAxis"].GetObj());
    matrix->ZAxis = *SVector3::DeserializeFromJson(object["ZAxis"].GetObj());

    return matrix;
}

bool SMatrix33::operator==(const SMatrix33& other) const
{
    return XAxis == other.XAxis && YAxis == other.YAxis && ZAxis == other.ZAxis;
}

const SVector3& SMatrix33::operator[](const unsigned int row) const
{
    return r[row];
}

SVector3& SMatrix33::operator[](const unsigned int row)
{
    return r[row];
}

SMatrix33 SMatrix33::operator*(const SMatrix33& matrixB) const
{
    const SMatrix33& matrixA = *this;

    return SMatrix33(
        matrixA[0][0] * matrixB[0][0] + matrixA[0][1] * matrixB[1][0] + matrixA[0][2] * matrixB[2][0],
        matrixA[0][0] * matrixB[0][1] + matrixA[0][1] * matrixB[1][1] + matrixA[0][2] * matrixB[2][1],
        matrixA[0][0] * matrixB[0][2] + matrixA[0][1] * matrixB[1][2] + matrixA[0][2] * matrixB[2][2],

        matrixA[1][0] * matrixB[0][0] + matrixA[1][1] * matrixB[1][0] + matrixA[1][2] * matrixB[2][0],
        matrixA[1][0] * matrixB[0][1] + matrixA[1][1] * matrixB[1][1] + matrixA[1][2] * matrixB[2][1],
        matrixA[1][0] * matrixB[0][2] + matrixA[1][1] * matrixB[1][2] + matrixA[1][2] * matrixB[2][2],

        matrixA[2][0] * matrixB[0][0] + matrixA[2][1] * matrixB[1][0] + matrixA[2][2] * matrixB[2][0],
        matrixA[2][0] * matrixB[0][1] + matrixA[2][1] * matrixB[1][1] + matrixA[2][2] * matrixB[2][1],
        matrixA[2][0] * matrixB[0][2] + matrixA[2][1] * matrixB[1][2] + matrixA[2][2] * matrixB[2][2]
    );
}

SVector3 SMatrix33::operator*(const SVector3& vector) const
{
    const SMatrix44& matrix = *this;
    SVector3 result;
    const float x = vector[0];
    const float y = vector[1];
    const float z = vector[2];

    result[0] = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z;
    result[1] = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z;
    result[2] = matrix[2][0] * x + matrix[2][1] * y + matrix[2][2] * z;

    return result;
}

SVector3 SMatrix33::ConvertRotationMatrixToEulerAngles()
{
    SVector3 result;

    result.y = std::asin(Math::Clamp(m13, -1.f, 1.f));

    if (std::abs(m13) < 0.99999)
    {
        result.x = std::atan2(-m23, m33);
        result.z = std::atan2(-m12, m11);
    }
    else
    {
        result.x = std::atan2(m32, m22);
        result.z = 0;
    }

    return result;
}

SMatrix33 SMatrix33::ConvertEulerAnglesToRotationMatrix(const SVector3& vector)
{
    SMatrix33 result;

    float x = vector.x, y = vector.y, z = vector.z;
    float a = std::cos(x), b = std::sin(x);
    float c = std::cos(y), d = std::sin(y);
    float e = std::cos(z), f = std::sin(z);

    float ae = a * e, af = a * f, be = b * e, bf = b * f;

    result.m11 = c * e;
    result.m12 = -c * f;
    result.m13 = d;

    result.m21 = af + be * d;
    result.m22 = ae - bf * d;
    result.m23 = -b * c;

    result.m31 = bf - ae * d;
    result.m32 = be + af * d;
    result.m33 = a * c;

    return result;
}

SQuaternion SMatrix33::MatrixToQuaternion()
{
    Normalize();

    return NormalizedMatrixToQuaternionWithChecks();
}

float SMatrix33::Determinant() const
{
    return Determinant(*this);
}

float SMatrix33::Determinant(const SMatrix33& matrix)
{
    return (matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
        matrix[1][0] * (matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1]) +
        matrix[2][0] * (matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1]));
}

void SMatrix33::Normalize()
{
    for (unsigned int i = 0; i < 3; ++i)
    {
        r[i].Normalize();
    }
}

void SMatrix33::Unit()
{
    Unit(*this);
}

void SMatrix33::Unit(SMatrix33& matrix)
{
    matrix.m11 = 1; matrix.m12 = 0; matrix.m13 = 0;
    matrix.m21 = 0; matrix.m22 = 1; matrix.m23 = 0;
    matrix.m31 = 0; matrix.m32 = 0; matrix.m33 = 1;
}

void SMatrix33::Negate()
{
    for (unsigned int i = 0; i < 3; ++i)
    {
        r[i].x = -1.f;
        r[i].y = -1.f;
        r[i].z = -1.f;
    }
}

SMatrix33 SMatrix33::Transposed() const
{
    return Transpose(*this);
}

SMatrix33 SMatrix33::Transpose(const SMatrix33& matrix)
{
    SMatrix33 result;

    result.m11 = matrix.m11;
    result.m12 = matrix.m21;
    result.m13 = matrix.m31;
    result.m21 = matrix.m12;
    result.m22 = matrix.m22;
    result.m23 = matrix.m32;
    result.m31 = matrix.m13;
    result.m32 = matrix.m23;
    result.m33 = matrix.m33;

    return result;
}

SMatrix33 SMatrix33::GetAdjointMatrix(const SMatrix33& matrix)
{
    return SMatrix33(
        matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1],
        -matrix[0][1] * matrix[2][2] + matrix[0][2] * matrix[2][1],
        matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1],

        -matrix[1][0] * matrix[2][2] + matrix[1][2] * matrix[2][0],
        matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0],
        -matrix[0][0] * matrix[1][2] + matrix[0][2] * matrix[1][0],

        matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0],
        -matrix[0][0] * matrix[2][1] + matrix[0][1] * matrix[2][0],
        matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]
    );
}

SMatrix33 SMatrix33::Inverted() const
{
    return Invert(*this);
}

SMatrix33 SMatrix33::Invert(const SMatrix33& matrix)
{
    float determinant = Determinant(matrix);
    SMatrix33 adjointMatrix;

    if (determinant == 0.f)
    {
        const float eps = PSEUDOINVERSE_EPSILON;
        SMatrix33 matrix2 = matrix;

        matrix2[0][0] += eps;
        matrix2[1][1] += eps;
        matrix2[2][2] += eps;

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

    SMatrix33 result;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            result[i][j] = adjointMatrix[i][j] / determinant;
        }
    }

    return result;
}

SMatrix33 SMatrix33::ScaleToMatrix(const SVector3& scale)
{
    SMatrix33 result;

    result[0][0] = scale[0];
    result[0][1] = 0.0f;
    result[0][2] = 0.0f;
    result[1][1] = scale[1];
    result[1][0] = 0.0f;
    result[1][2] = 0.0f;
    result[2][2] = scale[2];
    result[2][1] = 0.0f;
    result[2][0] = 0.0f;

    return result;
}

SQuaternion SMatrix33::NormalizedMatrixToQuaternionWithChecks()
{
    const float determinant = Determinant();

    if (!isfinite(determinant))
    {
        Unit();
    }
    else if (determinant < 0.f)
    {
        Negate();
    }

    return NormalizedMatrixToQuaternion();
}

SQuaternion SMatrix33::NormalizedMatrixToQuaternion()
{
    const SMatrix33& matrix = *this;
    SQuaternion quaternion;
    const float trace = matrix[0][0] + matrix[1][1] + matrix[2][2];

    if (trace > 0)
    {
        float s = 2.0f * sqrtf(1.0f + trace);

        quaternion.w128.m.m128_f32[3] = 0.25f * s;

        s = 1.0f / s;

        quaternion.w128.m.m128_f32[0] = (matrix[1][2] - matrix[2][1]) * s;
        quaternion.w128.m.m128_f32[1] = (matrix[2][0] - matrix[0][2]) * s;
        quaternion.w128.m.m128_f32[2] = (matrix[0][1] - matrix[1][0]) * s;
    }
    else
    {
        /* Find the biggest diagonal element to choose the best formula.
         * Here trace should also be always >= 0, avoiding bad precision. */
        if (matrix[0][0] > matrix[1][1] && matrix[0][0] > matrix[2][2])
        {
            float s = 2.0f * sqrtf(1.0f + matrix[0][0] - matrix[1][1] - matrix[2][2]);

            quaternion.w128.m.m128_f32[0] = 0.25f * s;

            s = 1.0f / s;

            quaternion.w128.m.m128_f32[3] = (matrix[1][2] - matrix[2][1]) * s;
            quaternion.w128.m.m128_f32[1] = (matrix[1][0] + matrix[0][1]) * s;
            quaternion.w128.m.m128_f32[2] = (matrix[2][0] + matrix[0][2]) * s;
        }
        else if (matrix[1][1] > matrix[2][2])
        {
            float s = 2.0f * sqrtf(1.0f + matrix[1][1] - matrix[0][0] - matrix[2][2]);

            quaternion.w128.m.m128_f32[1] = 0.25f * s;

            s = 1.0f / s;

            quaternion.w128.m.m128_f32[3] = (matrix[2][0] - matrix[0][2]) * s;
            quaternion.w128.m.m128_f32[0] = (matrix[1][0] + matrix[0][1]) * s;
            quaternion.w128.m.m128_f32[2] = (matrix[2][1] + matrix[1][2]) * s;
        }
        else
        {
            float s = 2.0f * sqrtf(1.0f + matrix[2][2] - matrix[0][0] - matrix[1][1]);

            quaternion.w128.m.m128_f32[2] = 0.25f * s;

            s = 1.0f / s;

            quaternion.w128.m.m128_f32[3] = (matrix[0][1] - matrix[1][0]) * s;
            quaternion.w128.m.m128_f32[0] = (matrix[2][0] + matrix[0][2]) * s;
            quaternion.w128.m.m128_f32[1] = (matrix[2][1] + matrix[1][2]) * s;
        }

        /* Make sure W is non-negative for a canonical result. */
        if (quaternion.w128.m.m128_f32[3] < 0)
        {
            quaternion.Negate();
        }
    }

    SQuaternion::Normalize(quaternion);

    return quaternion;
}

SMatrix33 SMatrix33::NormalizedAxisAngleToMatrixEx(const SVector3& axis, const float angleSin, const float angleCos)
{
    SVector3 nsi;
    float ico;
    float n00, n01, n11, n02, n12, n22;

    ico = 1.0f - angleCos;
    nsi[0] = axis[0] * angleSin;
    nsi[1] = axis[1] * angleSin;
    nsi[2] = axis[2] * angleSin;

    n00 = (axis[0] * axis[0]) * ico;
    n01 = (axis[0] * axis[1]) * ico;
    n11 = (axis[1] * axis[1]) * ico;
    n02 = (axis[0] * axis[2]) * ico;
    n12 = (axis[1] * axis[2]) * ico;
    n22 = (axis[2] * axis[2]) * ico;

    return SMatrix33(
        n00 + angleCos,
        n01 + nsi[2],
        n02 - nsi[1],
        n01 - nsi[2],
        n11 + angleCos,
        n12 + nsi[0],
        n02 + nsi[1],
        n12 - nsi[0],
        n22 + angleCos
    );
}

SMatrix33 SMatrix33::NormalizedAxisAngleToMatrix(const SVector3& axis, const float angle)
{
    return NormalizedAxisAngleToMatrixEx(axis, sinf(angle), cosf(angle));
}
