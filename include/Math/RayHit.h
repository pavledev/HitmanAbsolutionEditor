#pragma once

#include "../Rendering/Scene/Entity.h"
#include "Math.h"
#include "Math/Vector3.h"

class RayHit
{
public:
    RayHit(std::shared_ptr<Entity> entity, const Vector3& position, float distance, bool isInside);

    std::shared_ptr<Entity> entity;
    Vector3 position;
    float distance;
    bool isInside;
};
