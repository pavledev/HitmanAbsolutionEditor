#pragma once

#include <string>
#include <type_traits>
#include <memory>

class Entity;
class Transform;

class Component
{
public:
    Component() = default;
    Component(const char* name, const char* icon, std::weak_ptr<Entity> entity);
    virtual ~Component() = default;
    const std::string& GetName() const;
    std::shared_ptr<Transform> GetTransform() const;

    virtual void Tick()
    {
    }

    virtual void Render() = 0;
    virtual void RenderProperties() = 0;

protected:
    std::string name;
    std::weak_ptr<Entity> entity;
};
