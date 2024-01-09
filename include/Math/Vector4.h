#pragma once

#include "Math/Vector3.h"
#include "Utility/Math.h"

class Vector4
{
public:
    Vector4()
    {
        x = 0;
        y = 0;
        z = 0;
        w = 0;
    }

    Vector4(float x, float y, float z, float w)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = w;
    }

    Vector4(const Vector4& other) = default;

    Vector4(const float value)
    {
        this->x = value;
        this->y = value;
        this->z = value;
        this->w = value;
    }

    Vector4(const Vector3& value, float w)
    {
        this->x = value.x;
        this->y = value.y;
        this->z = value.z;
        this->w = w;
    }

    Vector4(const Vector3& value)
    {
        this->x = value.x;
        this->y = value.y;
        this->z = value.z;
        this->w = 0.0f;
    }

    bool operator ==(const Vector4& rhs) const
    {
        return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w;
    }

    bool operator !=(const Vector4& rhs) const
    {
        return !(*this == rhs);
    }

    Vector4 operator*(const float value) const
    {
        return Vector4(
            x * value,
            y * value,
            z * value,
            w * value
        );
    }

    void operator*=(const float value)
    {
        x *= value;
        y *= value;
        z *= value;
        w *= value;
    }

    Vector4 operator/(const float rhs) const
    {
        return Vector4(x / rhs, y / rhs, z / rhs, w / rhs);
    }

    Vector4 operator+(const Vector4& other) const
    {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector4 operator+(const float value) const
    {
        return Vector3(x + value, y + value, z + value);
    }

    void operator+=(const Vector4& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
    }

    void operator+=(const float value)
    {
        x += value;
        y += value;
        z += value;
    }

    const float& operator[](const unsigned int index) const
    {
        return v[index];
    }

    float& operator[](const unsigned int index)
    {
        return v[index];
    }

    float Length() const
    {
        return Math::Sqrt(x * x + y * y + z * z + w * w);
    }

    float SquaredLength() const
    {
        return x * x + y * y + z * z + w * w;
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
            w *= invertedLength;
        }
    }

    Vector4 Normalized() const
    {
        const float squaredLength = SquaredLength();

        if (!Math::Equals(squaredLength, 1.0f) && squaredLength > 0.0f)
        {
            const float invertedLength = 1.0f / Math::Sqrt(squaredLength);

            return (*this) * invertedLength;
        }
        else
        {
            return *this;
        }
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

    static const Vector4 One;
    static const Vector4 Zero;
    static const Vector4 Infinity;
    static const Vector4 InfinityNeg;
};
