#include "Math/RayHit.h"

RayHit::RayHit(std::shared_ptr<Entity> entity, const Vector3& position, float distance, bool isInside)
{
    this->entity = entity;
    this->position = position;
    this->distance = distance;
    this->isInside = isInside;
}
