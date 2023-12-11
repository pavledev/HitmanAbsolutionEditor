#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include <typeinfo>

#include "Component.h"

class Entity : public std::enable_shared_from_this<Entity>
{
public:
	Entity(const std::string& name);
	void Initialize();
	const std::string& GetName() const;
	const bool GetIsVisible() const;
	const std::vector<std::shared_ptr<Entity>>& GetChildren() const;
	const std::vector<std::shared_ptr<Component>>& GetComponents() const;
	const bool HasChildren() const;
	void SetIsVisible(const bool isVisible);
	void AddChild(std::shared_ptr<Entity> entity);
	void Render();
	void RenderComponents();

	template <typename T>
	std::shared_ptr<T> GetComponent() const
	{
		for (size_t i = 0; i < components.size(); ++i)
		{
			if (!components[i])
			{
				continue;
			}

			if (typeid(T) == typeid(*components[i].get()))
			{
				return std::static_pointer_cast<T>(components[i]);
			}
		}

		return nullptr;
	}

	template <typename T>
	std::shared_ptr<T> GetComponent()
	{
		for (size_t i = 0; i < components.size(); ++i)
		{
			if (typeid(T) == typeid(*components[i].get()))
			{
				return std::static_pointer_cast<T>(components[i]);
			}
		}

		return nullptr;
	}

	template <typename T>
	void AddComponent(const char* name, const char* icon)
	{
		std::shared_ptr<T> component = std::make_shared<T>(name, icon, this->shared_from_this());

		components.push_back(component);
	}

private:
	std::string name;
	bool isVisible;
	std::vector<std::shared_ptr<Entity>> children;
	std::vector<std::shared_ptr<Component>> components;
};
