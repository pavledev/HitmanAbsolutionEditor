#include "Rendering/Scene/Component.h"
#include "Rendering/Scene/Model.h"
#include "Rendering/Scene/Camera.h"
#include "Rendering/Scene/PointLight.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/Scene/Grid.h"
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
