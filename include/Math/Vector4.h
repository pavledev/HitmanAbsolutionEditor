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
        float length;

        Normalize(length);
    }

    void Normalize(float& length)
    {
        Vector4& vector = *this;
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

    Vector4 Normalized() const
    {
        Vector4 vector = *this;

        vector.Normalize();

        return vector;
    }

    Vector4 Normalized(float& length) const
    {
        Vector4 vector = *this;

        vector.Normalize(length);

        return vector;
    }

    static Vector4 Normalize(const Vector4& vector)
    {
        return vector.Normalized();
    }

    static Vector4 Normalize(const Vector4& vector, float& length)
    {
        return vector.Normalized(length);
    }

    bool IsNormalized() const
    {
        const float testUnit = SquaredLength();

        return fabs(testUnit - 1.f) < UNIT_EPSILON || fabs(testUnit) < UNIT_EPSILON;
    }

    float Dot(const Vector4& other) const
    {
        return x * other.x + y * other.y + z * other.z; //Fix
    }

    static float Dot(const Vector4& vector, const Vector4& vector2)
    {
        return vector.x * vector2.x + vector.y * vector2.y + vector.z * vector2.z; //Fix
    }

    static const unsigned int ComponentCount()
    {
        return 4;
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
