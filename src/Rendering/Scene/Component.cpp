#include "Rendering/Scene/Component.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/Scene/Entity.h"
#include "Utility/StringUtility.h"

Component::Component(const char* name, const char* icon, std::weak_ptr<Entity> entity) : entity(entity)
{
    this->name = std::format("{} {}", icon, name);
}

const std::string& Component::GetName() const
{
    return name;
}

std::shared_ptr<Transform> Component::GetTransform() const
{
    return entity.lock().get()->GetComponent<Transform>();
}
