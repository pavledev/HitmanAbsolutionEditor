#pragma once

#include "imgui.h"

#include "Utility/Math.h"

class Vector2
{
public:
    Vector2()
    {
        x = 0;
        y = 0;
    }

    Vector2(const Vector2& other) = default;

    Vector2(const float x, const float y)
    {
        this->x = x;
        this->y = y;
    }

    Vector2(const int x, const int y)
    {
        this->x = static_cast<float>(x);
        this->y = static_cast<float>(y);
    }

    Vector2(const unsigned int x, const unsigned int y)
    {
        this->x = static_cast<float>(x);
        this->y = static_cast<float>(y);
    }

    Vector2(const float x)
    {
        this->x = x;
        this->y = x;
    }

    ~Vector2() = default;

    Vector2 operator+(const Vector2& b) const
    {
        return Vector2(x + b.x, y + b.y);
    }

    void operator+=(const Vector2& b)
    {
        x += b.x;
        y += b.y;
    }

    Vector2 operator*(const Vector2& b) const
    {
        return Vector2(x * b.x, y * b.y);
    }

    void operator*=(const Vector2& b)
    {
        x *= b.x;
        y *= b.y;
    }

    Vector2 operator*(const float value) const
    {
        return Vector2(x * value, y * value);
    }

    void operator*=(const float value)
    {
        x *= value;
        y *= value;
    }

    Vector2 operator-(const Vector2& b) const
    {
        return Vector2(x - b.x, y - b.y);
    }

    Vector2 operator-(const float value) const
    {
        return Vector2(x - value, y - value);
    }

    void operator-=(const Vector2& rhs)
    {
        x -= rhs.x;
        y -= rhs.y;
    }

    Vector2 operator/(const Vector2& rhs) const
    {
        return Vector2(x / rhs.x, y / rhs.y);
    }

    Vector2 operator/(const float rhs) const
    {
        return Vector2(x / rhs, y / rhs);
    }

    void operator/=(const Vector2& rhs)
    {
        x /= rhs.x;
        y /= rhs.y;
    }

    float Length() const
    {
        return Math::Sqrt(x * x + y * y);
    }

    float SquaredLength() const
    {
        return x * x + y * y;
    }

    static float Distance(const Vector2& a, const Vector2& b)
    {
        return (b - a).Length();
    }

    static float SquaredDistance(const Vector2& a, const Vector2& b)
    {
        return (b - a).SquaredLength();
    }

    bool operator==(const Vector2& b) const
    {
        return x == b.x && y == b.y;
    }

    bool operator!=(const Vector2& b) const
    {
        return x != b.x || y != b.y;
    }

    operator ImVec2() const
    {
        return ImVec2(x, y);
    }

    union
    {
        struct
        {
            float x;
            float y;
        };

        float v[2];
    };

    static const Vector2 Zero;
    static const Vector2 One;
};
