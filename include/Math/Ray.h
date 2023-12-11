#pragma once

#include "BoundingBox.h"
#include "Sphere.h"

class Ray
{
public:
    Ray() = default;
    Ray(const Vector3& start, const Vector3& direction);
    ~Ray() = default;

    // Returns hit distance or infinity if there is no hit
    float HitDistance(const BoundingBox& box) const;
    float HitDistance(const Vector3& v1, const Vector3& v2, const Vector3& v3, Vector3* outNormal = nullptr, Vector3* outBary = nullptr) const;
    float HitDistance(const Sphere& sphere) const;

    float Distance(const Vector3& point) const;
    float Distance(const Vector3& point, Vector3& closestPoint) const;

    Vector3 ClosestPoint(const Ray& ray) const;

    const Vector3& GetStart() const;
    const Vector3& GetDirection() const;
    bool IsDefined() const;

private:
    Vector3 origin = Vector3::Zero;
    Vector3 direction = Vector3::Zero;
};
