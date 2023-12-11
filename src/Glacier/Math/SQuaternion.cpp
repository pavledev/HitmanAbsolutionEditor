#include "Glacier/Math/SQuaternion.h"
#include "Glacier/Math/SMatrix33.h"

#include "Utility/Math.h"
#include "Math/Quaternion.h"
#include "Math/Vector4.h"

SQuaternion::SQuaternion()
{
    this->w128.m.m128_f32[0] = 0.f;
    this->w128.m.m128_f32[1] = 0.f;
    this->w128.m.m128_f32[2] = 0.f;
    this->w128.m.m128_f32[3] = 1.f;
}

SQuaternion::SQuaternion(float x, float y, float z, float w)
{
    this->w128.m.m128_f32[0] = x;
    this->w128.m.m128_f32[1] = y;
    this->w128.m.m128_f32[2] = z;
    this->w128.m.m128_f32[3] = w;
}

SQuaternion SQuaternion::Negate(const SQuaternion& quaternion) const
{
    SQuaternion quaternion2;

    quaternion2.w128.m.m128_f32[0] = -quaternion.w128.m.m128_f32[0];
    quaternion2.w128.m.m128_f32[1] = -quaternion.w128.m.m128_f32[1];
    quaternion2.w128.m.m128_f32[2] = -quaternion.w128.m.m128_f32[2];
    quaternion2.w128.m.m128_f32[3] = -quaternion.w128.m.m128_f32[3];

    return quaternion;
}

SQuaternion::SQuaternion(const Quaternion& quaternion)
{
    w128.m.m128_f32[0] = quaternion.x;
    w128.m.m128_f32[1] = quaternion.y;
    w128.m.m128_f32[2] = quaternion.z;
    w128.m.m128_f32[3] = quaternion.w;
}

const float& SQuaternion::operator[](const unsigned int index) const
{
    return w128.m.m128_f32[index];
}

float& SQuaternion::operator[](const unsigned int index)
{
    return w128.m.m128_f32[index];
}

void SQuaternion::Negate()
{
    w128.m.m128_f32[0] = -w128.m.m128_f32[0];
	w128.m.m128_f32[1] = -w128.m.m128_f32[1];
	w128.m.m128_f32[2] = -w128.m.m128_f32[2];
	w128.m.m128_f32[3] = -w128.m.m128_f32[3];
}

float SQuaternion::LengthSquared() const
{
    float x = w128.m.m128_f32[0];
    float y = w128.m.m128_f32[1];
    float z = w128.m.m128_f32[2];
    float w = w128.m.m128_f32[3];

    return (x * x) + (y * y) + (z * z) + (w * w);
}

float SQuaternion::Dot(const SQuaternion& rhs) const
{
    return w128.m.m128_f32[3] * rhs.w128.m.m128_f32[3] +
        w128.m.m128_f32[0] * rhs.w128.m.m128_f32[0] +
        w128.m.m128_f32[1] * rhs.w128.m.m128_f32[1] +
        w128.m.m128_f32[2] * rhs.w128.m.m128_f32[2];
}

float SQuaternion::Dot(const SQuaternion& a, const SQuaternion& b)
{
    return a.w128.m.m128_f32[3] * b.w128.m.m128_f32[3] +
        a.w128.m.m128_f32[0] * b.w128.m.m128_f32[0] +
        a.w128.m.m128_f32[1] * b.w128.m.m128_f32[1] +
        a.w128.m.m128_f32[2] * b.w128.m.m128_f32[2];
}

SQuaternion SQuaternion::Multiply(const SQuaternion& quaternionA, const SQuaternion& quaternionB)
{
    SQuaternion result;

    /*result[0] = quaternionA[3] * quaternionB[0] + quaternionA[0] * quaternionB[3] + quaternionA[1] * quaternionB[2] - quaternionA[2] * quaternionB[1];
    result[1] = quaternionA[3] * quaternionB[1] - quaternionA[0] * quaternionB[2] + quaternionA[1] * quaternionB[3] + quaternionA[2] * quaternionB[0];
    result[2] = quaternionA[3] * quaternionB[2] + quaternionA[0] * quaternionB[1] - quaternionA[1] * quaternionB[0] + quaternionA[2] * quaternionB[3];
    result[3] = quaternionA[3] * quaternionB[3] - quaternionA[0] * quaternionB[0] - quaternionA[1] * quaternionB[1] - quaternionA[2] * quaternionB[2];*/

    result[0] = quaternionA[3] * quaternionB[0] + quaternionA[0] * quaternionB[3] + quaternionA[1] * quaternionB[2] - quaternionA[2] * quaternionB[1];
    result[1] = quaternionA[3] * quaternionB[1] + quaternionA[1] * quaternionB[3] + quaternionA[2] * quaternionB[0] - quaternionA[0] * quaternionB[2];
    result[2] = quaternionA[3] * quaternionB[2] + quaternionA[2] * quaternionB[3] + quaternionA[0] * quaternionB[1] - quaternionA[1] * quaternionB[0];
    result[3] = quaternionA[3] * quaternionB[3] - quaternionA[0] * quaternionB[0] - quaternionA[1] * quaternionB[1] - quaternionA[2] * quaternionB[2];

    return result;
}

SQuaternion SQuaternion::Normalized()
{
    float length = 0.f;

    return Normalized(length);
}

SQuaternion SQuaternion::Normalized(float& length)
{
    SQuaternion& quaternion = *this;
    length = sqrtf(Dot(quaternion, quaternion));

    if (length != 0.0f)
    {
        quaternion *= (1.0f / length);
    }
    else
    {
        w128.m.m128_f32[1] = 1.f;
        w128.m.m128_f32[0] = 0.f;
        w128.m.m128_f32[2] = 0.f;
        w128.m.m128_f32[3] = 0.f;
    }

    return quaternion;
}

SQuaternion SQuaternion::Normalize(const SQuaternion& quaternion)
{
    SQuaternion result = quaternion;

    Normalize(result);

    return result;
}

void SQuaternion::Normalize(SQuaternion& quaternion)
{
    const float length = sqrtf(Dot(quaternion, quaternion));

    if (length != 0.0f)
    {
        quaternion *= (1.0f / length);
    }
    else
    {
        quaternion.w128.m.m128_f32[1] = 1.f;
        quaternion.w128.m.m128_f32[0] = 0.f;
        quaternion.w128.m.m128_f32[2] = 0.f;
        quaternion.w128.m.m128_f32[3] = 0.f;
    }
}

SMatrix33 SQuaternion::ToMatrix() const
{
    SMatrix33 result;
    double q0, q1, q2, q3, qda, qdb, qdc, qaa, qab, qac, qbb, qbc, qcc;

    q0 = M_SQRT2 * (double)w128.m.m128_f32[3];
    q1 = M_SQRT2 * (double)w128.m.m128_f32[0];
    q2 = M_SQRT2 * (double)w128.m.m128_f32[1];
    q3 = M_SQRT2 * (double)w128.m.m128_f32[2];

    qda = q0 * q1;
    qdb = q0 * q2;
    qdc = q0 * q3;
    qaa = q1 * q1;
    qab = q1 * q2;
    qac = q1 * q3;
    qbb = q2 * q2;
    qbc = q2 * q3;
    qcc = q3 * q3;

    return SMatrix33(
        (float)(1.0 - qbb - qcc),
        (float)(qdc + qab),
        (float)(-qdb + qac),

        (float)(-qdc + qab),
        (float)(1.0 - qaa - qcc),
        (float)(qda + qbc),

        (float)(qdb + qac),
        (float)(-qda + qbc),
        (float)(1.0 - qaa - qbb)
    );
}

void SQuaternion::Conjugate()
{
    w128.m.m128_f32[0] = -w128.m.m128_f32[0];
    w128.m.m128_f32[1] = -w128.m.m128_f32[1];
    w128.m.m128_f32[2] = -w128.m.m128_f32[2];
}

SQuaternion SQuaternion::Conjugated() const
{
    return Conjugate(*this);
}

SQuaternion SQuaternion::Conjugate(const SQuaternion& quaternion)
{
    return SQuaternion(-quaternion.w128.m.m128_f32[0], -quaternion.w128.m.m128_f32[1], -quaternion.w128.m.m128_f32[2], quaternion.w128.m.m128_f32[3]);
}

SQuaternion SQuaternion::operator*(const SQuaternion& rhs) const
{
    return Multiply(*this, rhs);
}

SVector3 SQuaternion::operator*(const SVector3& rhs) const
{
    SQuaternion quaternion = (*this) * SQuaternion(rhs.x, rhs.y, rhs.z, 0.f) * Conjugated();

    return SVector3(quaternion[0], quaternion[1], quaternion[2]);
}

SQuaternion SQuaternion::operator*(float rhs) const
{
    return SQuaternion(w128.m.m128_f32[0] * rhs, w128.m.m128_f32[1] * rhs, w128.m.m128_f32[2] * rhs, w128.m.m128_f32[3] * rhs);
}

SQuaternion& SQuaternion::operator*=(float rhs)
{
    w128.m.m128_f32[0] *= rhs;
    w128.m.m128_f32[1] *= rhs;
    w128.m.m128_f32[2] *= rhs;
    w128.m.m128_f32[3] *= rhs;

    return *this;
}

SQuaternion SQuaternion::RotationBetweenVectorsToQuaternion(const SVector3& vector, const SVector3& vector2)
{
    SQuaternion quaternion;
    SVector3 axis = SVector3::Cross(vector, vector2);
    float length = 0.f;

    axis.Normalize(length);

    if (length > FLT_EPSILON)
    {
        float angle = SVector3::NormalizedAngle(vector, vector2);
        quaternion = NormalizedAxisAngleToQuaternion(axis, angle);
    }
    else
    {
        /* degenerate case */

        if (SVector3::Dot(vector, vector2) > 0.0f)
        {
            /* Same vectors, zero rotation... */
            quaternion = Unit();
        }
        else
        {
            /* Colinear but opposed vectors, 180 rotation... */
            axis = SVector3::Ortho(vector);
            quaternion = AxisAngleToQuaternion(axis, static_cast<float>(M_PI));
        }
    }

    return quaternion;
}

SQuaternion SQuaternion::AxisAngleToQuaternion(const SVector3& axis, const float angle)
{
    SQuaternion quaternion;
    float length = 0.f;
    SVector3 normalizedAxis = SVector3::Normalize(axis, length);

    if (length != 0.0f)
    {
        quaternion = NormalizedAxisAngleToQuaternion(normalizedAxis, angle);
    }
    else
    {
        quaternion = Unit();
    }

    return quaternion;
}

SQuaternion SQuaternion::NormalizedAxisAngleToQuaternion(const SVector3& axis, const float angle)
{
    SQuaternion quaternion;
    const float phi = 0.5f * angle;
    const float si = sinf(phi);
    const float co = cosf(phi);

    assert(axis.IsNormalized());

    quaternion.w128.m.m128_f32[3] = co;
    quaternion.w128.m.m128_f32[0] = axis.x * si;
    quaternion.w128.m.m128_f32[1] = axis.y * si;
    quaternion.w128.m.m128_f32[2] = axis.z * si;

    return quaternion;
}

SQuaternion SQuaternion::Unit()
{
    SQuaternion result;

    result.w128.m.m128_f32[3] = 1.0f;
    result.w128.m.m128_f32[0] = result.w128.m.m128_f32[1] = result.w128.m.m128_f32[2] = 0.0f;

    return result;
}

SQuaternion SQuaternion::RotationDifference(const SVector3& vector, const SVector3& vector2)
{
    const SVector3 vector3 = SVector3::Normalize(vector);
    const SVector3 vector4 = SVector3::Normalize(vector2);

    return RotationBetweenVectorsToQuaternion(vector3, vector4);
}

void SQuaternion::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("w128");
	w128.SerializeToJson(writer);

	writer.EndObject();
}
