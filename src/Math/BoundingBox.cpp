#include "Math/BoundingBox.h"

const BoundingBox BoundingBox::Undefined(Vector3::Infinity, Vector3::InfinityNeg);

BoundingBox::BoundingBox()
{
    min = Vector3::Infinity;
    max = Vector3::InfinityNeg;
}

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
{
    this->min = min;
    this->max = max;
}

BoundingBox::BoundingBox(const Vector3* points, const unsigned int pointCount)
{
    min = Vector3::Infinity;
    max = Vector3::InfinityNeg;

    for (unsigned int i = 0; i < pointCount; i++)
    {
        max.x = Math::Max(max.x, points[i].x);
        max.y = Math::Max(max.y, points[i].y);
        max.z = Math::Max(max.z, points[i].z);

        min.x = Math::Min(min.x, points[i].x);
        min.y = Math::Min(min.y, points[i].y);
        min.z = Math::Min(min.z, points[i].z);
    }
}

bool BoundingBox::operator==(const BoundingBox& other) const
{
    return GetMin() == other.GetMin() && GetMax() == other.GetMax();
}

Vector3 BoundingBox::GetCenter() const
{
    return (max + min) * 0.5f;
}

// Returns the size
Vector3 BoundingBox::GetSize() const
{
    return max - min;
}

// Returns extents
Vector3 BoundingBox::GetExtents() const
{
    return (max - min) * 0.5f;
}

Intersection BoundingBox::Intersects(const Vector3& point) const
{
    if (point.x < min.x || point.x > max.x ||
        point.y < min.y || point.y > max.y ||
        point.z < min.z || point.z > max.z)
    {
        return Intersection::Outside;
    }
    else
    {
        return Intersection::Inside;
    }
}

Intersection BoundingBox::Intersects(const BoundingBox& box) const
{
    if (box.max.x < min.x || box.min.x > max.x ||
        box.max.y < min.y || box.min.y > max.y ||
        box.max.z < min.z || box.min.z > max.z)
    {
        return Intersection::Outside;
    }
    else if (
        box.min.x < min.x || box.max.x > max.x ||
        box.min.y < min.y || box.max.y > max.y ||
        box.min.z < min.z || box.max.z > max.z)
    {
        return Intersection::Intersects;
    }
    else
    {
        return Intersection::Inside;
    }
}

BoundingBox BoundingBox::Transform(const Matrix44& transform) const
{
    const Vector3 centerNew = GetCenter() * transform;
    const Vector3 extentOld = GetExtents();
    const Vector3 extendNew = Vector3
    (
        std::abs(transform.m00) * extentOld.x + std::abs(transform.m10) * extentOld.y + std::abs(transform.m20) * extentOld.z,
        std::abs(transform.m01) * extentOld.x + std::abs(transform.m11) * extentOld.y + std::abs(transform.m21) * extentOld.z,
        std::abs(transform.m02) * extentOld.x + std::abs(transform.m12) * extentOld.y + std::abs(transform.m22) * extentOld.z
    );

    return BoundingBox(centerNew - extendNew, centerNew + extendNew);
}

void BoundingBox::Merge(const BoundingBox& box)
{
    min.x = Math::Min(min.x, box.min.x);
    min.y = Math::Min(min.y, box.min.y);
    min.z = Math::Min(min.z, box.min.z);
    max.x = Math::Max(max.x, box.max.x);
    max.y = Math::Max(max.x, box.max.x);
    max.z = Math::Max(max.x, box.max.x);
}

const Vector3& BoundingBox::GetMin() const
{
    return min;
}

const Vector3& BoundingBox::GetMax() const
{
    return max;
}
