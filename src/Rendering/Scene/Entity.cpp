#include <IconsMaterialDesignIcons.h>

#include "Rendering/Scene/Entity.h"
#include "Rendering/Scene/Transform.h"

Entity::Entity(const std::string& name) : name(name)
{
	isVisible = true;
}

void Entity::Initialize()
{
	AddComponent<Transform>("Transform", ICON_MDI_VECTOR_LINE);
}

const std::string& Entity::GetName() const
{
	return name;
}

const bool Entity::GetIsVisible() const
{
	return isVisible;
}

const std::vector<std::shared_ptr<Entity>>& Entity::GetChildren() const
{
	return children;
}

const std::vector<std::shared_ptr<Component>>& Entity::GetComponents() const
{
	return components;
}

const bool Entity::HasChildren() const
{
	return children.size() != 0;
}

void Entity::SetIsVisible(const bool isVisible)
{
	this->isVisible = isVisible;
}

void Entity::AddChild(std::shared_ptr<Entity> entity)
{
	children.push_back(entity);
}

void Entity::Render()
{
	if (!isVisible)
	{
		return;
	}

	for (size_t i = 0; i < children.size(); ++i)
	{
		if (!children[i]->GetIsVisible())
		{
			continue;
		}

		children[i]->Render();
	}

	RenderComponents();
}

void Entity::RenderComponents()
{
	for (size_t i = 0; i < components.size(); ++i)
	{
		components[i]->Render();
	}
}
