#pragma once

#include <vector>

#include "Math.h"
#include "Intersection.h"
#include "Rendering/Vertex.h"
#include "Matrix44.h"
#include "Utility/Math.h"

class BoundingBox
{
public:
    // Construct with zero size.
    BoundingBox();

    // Construct from minimum and maximum vectors.
    BoundingBox(const Vector3& min, const Vector3& max);

    // Construct from points
    BoundingBox(const Vector3* points, const unsigned int pointCount);

    // Construct from vertices
    template <typename T>
    BoundingBox(const std::vector<T>& vertices)
    {
        min = Vector3::Infinity;
        max = Vector3::InfinityNeg;

        for (unsigned int i = 0; i < vertices.size(); i++)
        {
            max.x = Math::Max(max.x, vertices[i].position.x);
            max.y = Math::Max(max.y, vertices[i].position.y);
            max.z = Math::Max(max.z, vertices[i].position.z);

            min.x = Math::Min(min.x, vertices[i].position.x);
            min.y = Math::Min(min.y, vertices[i].position.y);
            min.z = Math::Min(min.z, vertices[i].position.z);
        }
    }

    ~BoundingBox() = default;

    // Assign from bounding box
    BoundingBox& operator=(const BoundingBox& rhs) = default;
    bool operator==(const BoundingBox& other) const;

    // Returns the center
    Vector3 GetCenter() const;

    // Returns the size
    Vector3 GetSize() const;

    // Returns extents
    Vector3 GetExtents() const;

    // Test if a point is inside
    Intersection Intersects(const Vector3& point) const;

    // Test if a bounding box is inside
    Intersection Intersects(const BoundingBox& box) const;

    // Returns a transformed bounding box
    BoundingBox Transform(const Matrix44& transform) const;

    // Merge with another bounding box
    void Merge(const BoundingBox& box);

    const Vector3& GetMin() const;
    const Vector3& GetMax() const;

    static const BoundingBox Undefined;

private:
    Vector3 min;
    Vector3 max;
};
