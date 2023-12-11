#pragma once

#include "Math.h"
#include "Vector3.h"

class Sphere
{
public:
    Sphere() = default;
    Sphere(const Vector3& center, const float radius);
    ~Sphere() = default;

    Vector3 center;
    float radius;
};
