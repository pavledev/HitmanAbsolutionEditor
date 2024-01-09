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
        const float squaredLength = SquaredLength();

        if (!Math::Equals(squaredLength, 1.0f) && squaredLength > 0.0f)
        {
            const float invertedLength = 1.0f / Math::Sqrt(squaredLength);

            x *= invertedLength;
            y *= invertedLength;
            z *= invertedLength;
        }
    }

    Vector3 Normalized() const
    {
        Vector3 v = *this;

        v.Normalize();

        return v;
    }

    static Vector3 Normalize(const Vector3& v)
    {
        return v.Normalized();
    }

    bool IsNormalized() const
    {
        static constexpr float normalizedVectorThreshold = 0.01f;

        return (Math::Abs(1.f - SquaredLength()) < normalizedVectorThreshold);
    }

    static float Dot(const Vector3& v1, const Vector3& v2)
    {
        return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    }

    float Dot(const Vector3& rhs) const
    {
        return x * rhs.x + y * rhs.y + z * rhs.z;
    }

    static Vector3 Cross(const Vector3& v1, const Vector3& v2)
    {
        return Vector3(
            v1.y * v2.z - v2.y * v1.z,
            -(v1.x * v2.z - v2.x * v1.z),
            v1.x * v2.y - v2.x * v1.y
        );
    }

    Vector3 Cross(const Vector3& v2) const
    {
        return Cross(*this, v2);
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
