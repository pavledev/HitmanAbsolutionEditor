#pragma once

#undef min
#undef max

#include "Foundation/NxQuat.h"

#include "Glacier/Math/SQuaternion.h"

#include "Math.h"
#include "Math/Vector3.h"
#include "Math/Matrix33.h"

class Quaternion
{
public:
    Quaternion()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 1;
    }

    Quaternion(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Quaternion(const SQuaternion& quaternion)
    {
        x = quaternion.w128.m.m128_f32[0];
        y = quaternion.w128.m.m128_f32[1];
        z = quaternion.w128.m.m128_f32[2];
        w = quaternion.w128.m.m128_f32[3];
    }

    Quaternion(const NxQuat& quaternion)
    {
        x = quaternion.x;
        y = quaternion.y;
        z = quaternion.z;
        w = quaternion.w;
    }

    //Creates a new Quaternion from the specified yaw, pitch and roll angles.
    //Yaw around the y axis in radians.
    //Pitch around the x axis in radians.
    //Roll around the z axis in radians.
    static Quaternion FromYawPitchRoll(float yaw, float pitch, float roll)
    {
        const float halfRoll = roll * 0.5f;
        const float halfPitch = pitch * 0.5f;
        const float halfYaw = yaw * 0.5f;

        const float sinRoll = sin(halfRoll);
        const float cosRoll = cos(halfRoll);
        const float sinPitch = sin(halfPitch);
        const float cosPitch = cos(halfPitch);
        const float sinYaw = sin(halfYaw);
        const float cosYaw = cos(halfYaw);

        return Quaternion(
            cosYaw * sinPitch * cosRoll + sinYaw * cosPitch * sinRoll,
            sinYaw * cosPitch * cosRoll - cosYaw * sinPitch * sinRoll,
            cosYaw * cosPitch * sinRoll - sinYaw * sinPitch * cosRoll,
            cosYaw * cosPitch * cosRoll + sinYaw * sinPitch * sinRoll
        );
    }

    static Quaternion Multiply(const Quaternion& quaternionA, const Quaternion& quaternionB)
    {
        Quaternion result;

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

    Quaternion Conjugated() const
    {
        return Conjugate(*this);
    }

    static Quaternion Conjugate(const Quaternion& quaternion)
    {
        return Quaternion(-quaternion.x, -quaternion.y, -quaternion.z, quaternion.w);
    }

    void Conjugate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

    void Negate()
    {
        x = -x;
        y = -y;
        z = -z;
        w = -w;
    }

    float SquaredLength() const
    {
        return x * x + y * y + z * z + w * w;
    }

    void Normalize()
    {
        float length;

        Normalize(length);
    }

    void Normalize(float& length)
    {
        Quaternion& quaternion = *this;
        length = sqrtf(Dot(quaternion, quaternion));

        if (length != 0.0f)
        {
            quaternion *= (1.0f / length);
        }
        else
        {
            quaternion.x = 0.f;
            quaternion.y = 0.f;
            quaternion.z = 0.f;
            quaternion.w = 1.f;
        }
    }

    Quaternion Normalized() const
    {
        Quaternion quaternion = *this;

        quaternion.Normalize();

        return quaternion;
    }

    Quaternion Normalized(float& length) const
    {
        Quaternion quaternion = *this;

        quaternion.Normalize(length);

        return quaternion;
    }

    static Quaternion Normalize(const Quaternion& quaternion)
    {
        return quaternion.Normalized();
    }

    static Quaternion Normalize(const Quaternion& quaternion, float& length)
    {
        return quaternion.Normalized(length);
    }

    Quaternion Inverse() const
    {
        const float squaredLength = SquaredLength();

        if (squaredLength == 1.0f)
        {
            return Conjugate(*this);
        }
        else if (squaredLength >= std::numeric_limits<float>::epsilon())
        {
            return Conjugate(*this) * (1.0f / squaredLength);
        }

        return Identity;
    }

    Vector3 ToEulerAngles() const
    {
        // Derivation from http://www.geometrictools.com/Documentation/EulerAngles.pdf
        // Order of rotations: Z first, then X, then Y
        const float check = 2.0f * (-y * z + w * x);

        if (check < -0.995f)
        {
            return Vector3
            (
                -90.0f,
                0.0f,
                DirectX::XMConvertToDegrees(-atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)))
            );
        }

        if (check > 0.995f)
        {
            return Vector3
            (
                90.0f,
                0.0f,
                DirectX::XMConvertToDegrees(atan2f(2.0f * (x * z - w * y), 1.0f - 2.0f * (y * y + z * z)))
            );
        }

        return Vector3
        (
            DirectX::XMConvertToDegrees(asinf(check)),
            DirectX::XMConvertToDegrees(atan2f(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + y * y))),
            DirectX::XMConvertToDegrees(atan2f(2.0f * (x * y + w * z), 1.0f - 2.0f * (x * x + z * z)))
        );
    }

    static Quaternion FromEulerAngles(const Vector3& rotation)
    {
        return FromYawPitchRoll(DirectX::XMConvertToRadians(rotation.y), DirectX::XMConvertToRadians(rotation.x), DirectX::XMConvertToRadians(rotation.z));
    }

    static Quaternion FromEulerAngles(float rotationX, float rotationY, float rotationZ)
    {
        return FromYawPitchRoll(DirectX::XMConvertToRadians(rotationY), DirectX::XMConvertToRadians(rotationX), DirectX::XMConvertToRadians(rotationZ));
    }

    float Dot(const Quaternion& other) const
    {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }

    static float Dot(const Quaternion& a, const Quaternion& b)
    {
        return a.w * b.w + a.x * b.x + a.y * b.y + a.z * b.z;
    }

    Quaternion operator+(const Quaternion& other) const
    {
        return Quaternion(x + other.x, y + other.y, z + other.z, w + other.w);
    }

    Quaternion operator-(const Quaternion& other) const
    {
        return Quaternion(x - other.x, y - other.y, z - other.z, w - other.w);
    }

    Quaternion operator-() const
    {
        return Quaternion(-x, -y, -z, -w);
    }

    Quaternion operator*(const Quaternion& other) const
    {
        return Multiply(*this, other);
    }

    void operator*=(const Quaternion& other)
    {
        *this = Multiply(*this, other);
    }

    Vector3 operator*(const Vector3& other) const
    {
        const Vector3 qVec(x, y, z);
        const Vector3 cross1(qVec.Cross(other));
        const Vector3 cross2(qVec.Cross(cross1));

        return other + 2.0f * (cross1 * w + cross2);
    }

    Quaternion& operator*=(float other)
    {
        x *= other;
        y *= other;
        z *= other;
        w *= other;

        return *this;
    }

    Quaternion operator*(float other) const
    {
        return Quaternion(x * other, y * other, z * other, w * other);
    }

    bool operator==(const Quaternion& other) const
    {
        return x == other.x && y == other.y && z == other.z && w == other.w;
    }

    bool operator!=(const Quaternion& other) const
    {
        return !(*this == other);
    }

    const float& operator[](const unsigned int index) const
    {
        return v[index];
    }

    float& operator[](const unsigned int index)
    {
        return v[index];
    }

    static Quaternion RotationBetweenVectorsToQuaternion(const Vector3& vector, const Vector3& vector2)
    {
        Quaternion quaternion;
        Vector3 axis = Vector3::Cross(vector, vector2);
        float length = 0.f;

        axis.Normalize(length);

        if (length > FLT_EPSILON)
        {
            float angle = Vector3::NormalizedAngle(vector, vector2);
            quaternion = NormalizedAxisAngleToQuaternion(axis, angle);
        }
        else
        {
            /* degenerate case */

            if (Vector3::Dot(vector, vector2) > 0.0f)
            {
                /* Same vectors, zero rotation... */
                quaternion = Unit();
            }
            else
            {
                /* Colinear but opposed vectors, 180 rotation... */
                axis = Vector3::Ortho(vector);
                quaternion = AxisAngleToQuaternion(axis, static_cast<float>(M_PI));
            }
        }

        return quaternion;
    }

    static Quaternion AxisAngleToQuaternion(const Vector3& axis, const float angle)
    {
        Quaternion quaternion;
        float length = 0.f;
        Vector3 normalizedAxis = Vector3::Normalize(axis, length);

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

    static Quaternion NormalizedAxisAngleToQuaternion(const Vector3& axis, const float angle)
    {
        Quaternion quaternion;
        const float phi = 0.5f * angle;
        const float si = sinf(phi);
        const float co = cosf(phi);

        quaternion[3] = co;
        quaternion[0] = axis.x * si;
        quaternion[1] = axis.y * si;
        quaternion[2] = axis.z * si;

        return quaternion;
    }

    static Quaternion Unit()
    {
        Quaternion result;

        result.w = 1.0f;
        result.x = result.y = result.z = 0.0f;

        return result;
    }

    static Quaternion RotationDifference(const Vector3& vector, const Vector3& vector2)
    {
        const Vector3 vector3 = Vector3::Normalize(vector);
        const Vector3 vector4 = Vector3::Normalize(vector2);

        return RotationBetweenVectorsToQuaternion(vector3, vector4);
    }

    Matrix33 ToMatrix() const
    {
        Matrix33 result;
        double q0, q1, q2, q3, qda, qdb, qdc, qaa, qab, qac, qbb, qbc, qcc;

        q0 = M_SQRT2 * (double)w;
        q1 = M_SQRT2 * (double)x;
        q2 = M_SQRT2 * (double)y;
        q3 = M_SQRT2 * (double)z;

        qda = q0 * q1;
        qdb = q0 * q2;
        qdc = q0 * q3;
        qaa = q1 * q1;
        qab = q1 * q2;
        qac = q1 * q3;
        qbb = q2 * q2;
        qbc = q2 * q3;
        qcc = q3 * q3;

        return Matrix33(
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

    union
    {
        struct
        {
            float x;
            float y;
            float z;
            float w;
        };

        float v[4];
    };

    static const Quaternion Identity;
};

// Reverse order operators
inline Vector3 operator*(const Vector3& lhs, const Quaternion& other)
{
    return other * lhs;
}

inline Quaternion operator*(float lhs, const Quaternion& other)
{
    return other * lhs;
}
