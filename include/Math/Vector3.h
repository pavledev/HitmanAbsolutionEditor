#pragma once

#undef min
#undef max

#include "Foundation/NxVec3.h"

#include "Glacier/Math/SVector3.h"

#include "Utility/Math.h"

class Vector4;

class Vector3
{
public:
    Vector3()
    {
        x = 0;
        y = 0;
        z = 0;
    }

    Vector3(const float x, const float y, const float z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Vector3(const Vector3& other) = default;

    Vector3(const Vector4& other);

    Vector3(const SVector3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    Vector3(const NxVec3& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }

    Vector3(const float value)
    {
        x = value;
        y = value;
        z = value;
    }

    void Normalize()
    {
        float length;

        Normalize(length);
    }

    void Normalize(float& length)
    {
        Vector3& vector = *this;
        float squaredLength = Dot(vector, vector);

        //A larger value causes normalize errors in a scaled down models with camera extreme close.
        if (squaredLength > 1.0e-35f)
        {
            length = Math::Sqrt(squaredLength);
            vector *= 1.f / length;
        }
        else
        {
            //Either the vector is small or one of it's values contained `nan`.
            length = 0.f;
        }
    }

    Vector3 Normalized() const
    {
        Vector3 vector = *this;

        vector.Normalize();

        return vector;
    }

    Vector3 Normalized(float& length) const
    {
        Vector3 vector = *this;

        vector.Normalize(length);

        return vector;
    }

    static Vector3 Normalize(const Vector3& vector)
    {
        return vector.Normalized();
    }

    static Vector3 Normalize(const Vector3& vector, float& length)
    {
        return vector.Normalized(length);
    }

    bool IsNormalized() const
    {
        const float testUnit = SquaredLength();

        return fabs(testUnit - 1.f) < UNIT_EPSILON || fabs(testUnit) < UNIT_EPSILON;
    }

    float Dot(const Vector3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }

    static float Dot(const Vector3& vector, const Vector3& vector2)
    {
        return vector.x * vector2.x + vector.y * vector2.y + vector.z * vector2.z;
    }

    static Vector3 Cross(const Vector3& vector, const Vector3& vector2)
    {
        Vector3 result;

        result.x = vector.y * vector2.z - vector.z * vector2.y;
        result.y = vector.z * vector2.x - vector.x * vector2.z;
        result.z = vector.x * vector2.y - vector.y * vector2.x;

        return result;
    }

    Vector3 Cross(const Vector3& other) const
    {
        return Cross(*this, other);
    }

    float Length() const
    {
        return Math::Sqrt(x * x + y * y + z * z);
    }

    static float Length(const Vector3& vector, const Vector3& vector2)
    {
        return (vector2 - vector).Length();
    }

    float SquaredLength() const
    {
        return x * x + y * y + z * z;
    }

    Vector3 operator*(const Vector3& other) const
    {
        return Vector3(
            x * other.x,
            y * other.y,
            z * other.z
        );
    }

    void operator*=(const Vector3& other)
    {
        x *= other.x;
        y *= other.y;
        z *= other.z;
    }

    template <typename U = float>
    Vector3 operator*(const U value) const
    {
        return Vector3(
            x * value,
            y * value,
            z * value
        );
    }

    template <typename U = float>
    void operator*=(const U value)
    {
        x *= value;
        y *= value;
        z *= value;
    }

    Vector3 operator+(const Vector3& other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    template <typename U = float>
    Vector3 operator+(const U value) const
    {
        return Vector3(x + value, y + value, z + value);
    }

    void operator+=(const Vector3& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    template <typename U = float>
    void operator+=(const U value)
    {
        x += value;
        y += value;
        z += value;
    }

    Vector3 operator-(const Vector3& other) const
    {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    template <typename U = float>
    Vector3 operator-(const U value) const
    {
        return Vector3(x - value, y - value, z - value);
    }

    void operator-=(const Vector3& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
    }

    Vector3 operator/(const Vector3& other) const
    {
        return Vector3(x / other.x, y / other.y, z / other.z);
    }

    Vector3 operator/(const float other) const
    {
        return Vector3(x / other, y / other, z / other);
    }

    void operator/=(const Vector3& other)
    {
        x /= other.x;
        y /= other.y;
        z /= other.z;
    }

    bool operator==(const Vector3& other) const
    {
        return x == other.x && y == other.y && z == other.z;
    }

    bool operator!=(const Vector3& other) const
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

    static const unsigned int ComponentCount()
    {
        return 3;
    }

    static float NormalizedAngle(const Vector3& vector, const Vector3& vector2)
    {
        /* double check they are normalized */
        assert(vector.IsNormalized());
        assert(vector2.IsNormalized());

        /* this is the same as acos(Dot(v1, v2)), but more accurate */
        if (Dot(vector, vector2) >= 0.0f)
        {
            return 2.0f * Math::ASin(Length(vector, vector2) / 2.0f);
        }

        return static_cast<float>(M_PI) - 2.0f * Math::ASin(Length(vector, Negate(vector2)) / 2.0f);
    }

    static Vector3 Ortho(const Vector3& vector)
    {
        Vector3 result = {};
        const int axis = DominantAxis(vector);

        assert(result != vector);

        switch (axis)
        {
            case 0:
                result.x = -vector.y - vector.z;
                result.y = vector.x;
                result.z = vector.x;
                break;
            case 1:
                result.x = vector.y;
                result.y = -vector.x - vector.z;
                result.z = vector.y;
                break;
            case 2:
                result.x = vector.z;
                result.y = vector.z;
                result.z = -vector.x - vector.y;
                break;
        }

        return result;
    }

    static int DominantAxis(const Vector3& vector)
    {
        const float x = fabsf(vector.x);
        const float y = fabsf(vector.y);
        const float z = fabsf(vector.z);

        return ((x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2));
    }

    static Vector3 ProjectNormalized(const Vector3& p, const Vector3& proj)
    {
        return proj * Dot(p, proj);
    }

    static float Angle(const Vector3& vector, const Vector3& vector2)
    {
        const Vector3 normalizedVector = Vector3::Normalize(vector);
        const Vector3 normalizedVector2 = Vector3::Normalize(vector2);

        return NormalizedAngle(normalizedVector, normalizedVector2);
    }

    static Vector3 Negate(const Vector3& vector)
    {
        return Vector3(-vector.x, -vector.y, -vector.z);
    }

    void Negate()
    {
        x = -x;
        y = -y;
        z = -z;
    }

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

    static const Vector3 Zero;
    static const Vector3 Left;
    static const Vector3 Right;
    static const Vector3 Up;
    static const Vector3 Down;
    static const Vector3 Forward;
    static const Vector3 Backward;
    static const Vector3 One;
    static const Vector3 Infinity;
    static const Vector3 InfinityNeg;
};

// Reverse order operators
inline Vector3 operator*(float lhs, const Vector3& rhs)
{
    return rhs * lhs;
}
