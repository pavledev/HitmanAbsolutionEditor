#pragma once

#include <string>
#include <unordered_map>
#include <map>

class EnumRegistry
{
public:
	static EnumRegistry& GetInstance();
	void Load();
	const std::map<int, std::string>& GetEnum(const std::string& typeName);
	const std::unordered_map<std::string, std::map<int, std::string>>& GetEnums() const;

private:
	EnumRegistry() = default;
	EnumRegistry(const EnumRegistry& other) = delete;
	EnumRegistry& operator=(const EnumRegistry& other) = delete;

	std::unordered_map<std::string, std::map<int, std::string>> enums;
};
