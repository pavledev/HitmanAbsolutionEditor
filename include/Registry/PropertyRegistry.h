#pragma once

#include <string>
#include <unordered_map>

class PropertyRegistry
{
public:
	struct Property
	{
		std::string name;
		std::string description;
	};

	static PropertyRegistry& GetInstance();
	void Load();
	const std::string& GetPropertyName(const unsigned int propertyID) const;
	const char* GetPropertyDescription(const unsigned int propertyID) const;
	const std::unordered_map<unsigned int, Property>& GetProperties() const;

private:
	PropertyRegistry() = default;
	PropertyRegistry(const PropertyRegistry& other) = delete;
	PropertyRegistry& operator=(const PropertyRegistry& other) = delete;

	std::unordered_map<unsigned int, Property> properties;
};
