#include <fstream>
#include <map>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include "Registry/PropertyRegistry.h"
#include "Logger.h"

PropertyRegistry& PropertyRegistry::GetInstance()
{
	static PropertyRegistry instance;

	return instance;
}

void PropertyRegistry::Load()
{
	std::ifstream inputFileStream = std::ifstream("assets/Properties.json");
	rapidjson::IStreamWrapper streamWrapper(inputFileStream);
	rapidjson::Document document;

	document.ParseStream(streamWrapper);

	const rapidjson::Value& properties2 = document["properties"];

	for (rapidjson::Value::ConstValueIterator it = properties2.Begin(); it != properties2.End(); ++it)
	{
		const rapidjson::Value& object = it->GetObj();
		Property property;

		const unsigned int hash = object["hash"].GetUint();
		property.name = object["name"].GetString();

		if (!object["description"].IsNull())
		{
			property.description = object["description"].GetString();
		}

		properties.insert(std::make_pair(hash, property));
	}

	Logger::GetInstance().Log(Logger::Level::Info, "Sucessfully loaded properties.");
}

const std::string& PropertyRegistry::GetPropertyName(const unsigned int propertyID) const
{
	auto iterator = properties.find(propertyID);

	if (iterator != properties.end())
	{
		return iterator->second.name;
	}

	static std::string propertyName;

	return propertyName;
}

const char* PropertyRegistry::GetPropertyDescription(const unsigned int propertyID) const
{
	auto iterator = properties.find(propertyID);

	if (iterator != properties.end())
	{
		return iterator->second.description.c_str();
	}

	return nullptr;
}

const std::unordered_map<unsigned int, PropertyRegistry::Property>& PropertyRegistry::GetProperties() const
{
	return properties;
}
