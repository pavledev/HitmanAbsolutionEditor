#include "Math/Ray.h"
#include "Math/Vector3.h"

Ray::Ray(const Vector3& start, const Vector3& direction)
{
    origin = start;
    this->direction = Vector3::Normalize(direction);
}

float Ray::HitDistance(const BoundingBox& box) const
{
    // If undefined, no hit (infinite distance)
    if (box == BoundingBox::Undefined)
    {
        return std::numeric_limits<float>::infinity();
    }

    // Check for ray origin being inside the box
    if (box.Intersects(origin) == Intersection::Inside)
    {
        return 0.0f;
    }

    float distance = std::numeric_limits<float>::infinity();

    // Check for intersecting in the X-direction
    if (origin.x < box.GetMin().x && direction.x > 0.0f)
    {
        const float x = (box.GetMin().x - origin.x) / direction.x;
        if (x < distance)
        {
            const Vector3 point = origin + x * direction;
            if (point.y >= box.GetMin().y && point.y <= box.GetMax().y && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                distance = x;
            }
        }
    }
    if (origin.x > box.GetMax().x && direction.x < 0.0f)
    {
        const float x = (box.GetMax().x - origin.x) / direction.x;
        if (x < distance)
        {
            const Vector3 point = origin + x * direction;
            if (point.y >= box.GetMin().y && point.y <= box.GetMax().y && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                distance = x;
            }
        }
    }

    // Check for intersecting in the Y-direction
    if (origin.y < box.GetMin().y && direction.y > 0.0f)
    {
        const float x = (box.GetMin().y - origin.y) / direction.y;
        if (x < distance)
        {
            const Vector3 point = origin + x * direction;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                distance = x;
            }
        }
    }
    if (origin.y > box.GetMax().y && direction.y < 0.0f)
    {
        const float x = (box.GetMax().y - origin.y) / direction.y;
        if (x < distance)
        {
            const Vector3 point = origin + x * direction;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.z >= box.GetMin().z && point.z <= box.GetMax().z)
            {
                distance = x;
            }
        }
    }

    // Check for intersecting in the Z-direction
    if (origin.z < box.GetMin().z && direction.z > 0.0f)
    {
        const float x = (box.GetMin().z - origin.z) / direction.z;
        if (x < distance)
        {
            const Vector3 point = origin + x * direction;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.y >= box.GetMin().y && point.y <= box.GetMax().y)
            {
                distance = x;
            }
        }
    }
    if (origin.z > box.GetMax().z && direction.z < 0.0f)
    {
        const float x = (box.GetMax().z - origin.z) / direction.z;
        if (x < distance)
        {
            const Vector3 point = origin + x * direction;
            if (point.x >= box.GetMin().x && point.x <= box.GetMax().x && point.y >= box.GetMin().y && point.y <= box.GetMax().y)
            {
                distance = x;
            }
        }
    }

    return distance;
}

float Ray::HitDistance(const Vector3& v1, const Vector3& v2, const Vector3& v3, Vector3* outNormal, Vector3* outBary) const
{
    // Based on Fast, Minimum Storage Ray/Triangle Intersection by Möller & Trumbore
    // http://www.graphics.cornell.edu/pubs/1997/MT97.pdf
    // Calculate edge vectors
    Vector3 edge1(v2 - v1);
    Vector3 edge2(v3 - v1);

    // Calculate determinant & check backfacing
    Vector3 p(direction.Cross(edge2));
    float det = edge1.Dot(p);

    if (det >= std::numeric_limits<float>::epsilon())
    {
        // Calculate u & v parameters and test
        Vector3 t(origin - v1);
        float u = t.Dot(p);
        if (u >= 0.0f && u <= det)
        {
            Vector3 q(t.Cross(edge1));
            float v = direction.Dot(q);
            if (v >= 0.0f && u + v <= det)
            {
                float distance = edge2.Dot(q) / det;

                // Discard hits behind the ray
                if (distance >= 0.0f)
                {
                    // There is an intersection, so calculate distance & optional normal
                    if (outNormal)
                        *outNormal = edge1.Cross(edge2);
                    if (outBary)
                        *outBary = Vector3(1 - (u / det) - (v / det), u / det, v / det);

                    return distance;
                }
            }
        }
    }

    return std::numeric_limits<float>::infinity();
}

float Ray::HitDistance(const Sphere& sphere) const
{
    Vector3 centeredOrigin = origin - sphere.center;
    float squaredRadius = sphere.radius * sphere.radius;

    // Check if ray originates inside the sphere
    if (centeredOrigin.SquaredLength() <= squaredRadius)
        return 0.0f;

    // Calculate intersection by quadratic equation
    float a = direction.Dot(direction);
    float b = 2.0f * centeredOrigin.Dot(direction);
    float c = centeredOrigin.Dot(centeredOrigin) - squaredRadius;
    float d = b * b - 4.0f * a * c;

    // No solution
    if (d < 0.0f)
        return std::numeric_limits<float>::infinity();

    // Get the nearer solution
    float dSqrt = sqrtf(d);
    float dist = (-b - dSqrt) / (2.0f * a);
    if (dist >= 0.0f)
        return dist;
    else
        return (-b + dSqrt) / (2.0f * a);
}

float Ray::Distance(const Vector3& point) const
{
    const Vector3 closestPoint = origin + (direction * (point - origin).Dot(direction));
    return (closestPoint - point).Length();
}

float Ray::Distance(const Vector3& point, Vector3& closestPoint) const
{
    closestPoint = origin + (direction * (point - origin).Dot(direction));
    return (closestPoint - point).Length();
}

Vector3 Ray::ClosestPoint(const Ray& ray) const
{
    // Algorithm based on http://paulbourke.net/geometry/lineline3d/
    Vector3 p13 = origin - ray.origin;
    Vector3 p43 = ray.direction;
    Vector3 p21 = direction;

    float d1343 = p13.Dot(p43);
    float d4321 = p43.Dot(p21);
    float d1321 = p13.Dot(p21);
    float d4343 = p43.Dot(p43);
    float d2121 = p21.Dot(p21);

    float d = d2121 * d4343 - d4321 * d4321;
    if (std::abs(d) < std::numeric_limits<float>::epsilon())
        return origin;

    float n = d1343 * d4321 - d1321 * d4343;
    float a = n / d;

    return origin + a * direction;
}

const Vector3& Ray::GetStart() const
{
    return origin;
}

const Vector3& Ray::GetDirection() const
{
    return direction;
}

bool Ray::IsDefined() const
{
    return origin != direction && direction != Vector3::Zero;
}
