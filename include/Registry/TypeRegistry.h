#pragma once

#include <string>
#include <unordered_map>

#include "Glacier/TypeInfo/STypeID.h"

class TypeRegistry
{
public:
	~TypeRegistry();
	static TypeRegistry& GetInstance();
	void Load();
	STypeID* GetTypeID(const std::string& typeName) const;
	const std::unordered_map<std::string, STypeID*>& GetTypeIDs() const;
	void RegisterType(STypeID* typeID);

private:
	TypeRegistry() = default;
	TypeRegistry(const TypeRegistry& other) = delete;
	TypeRegistry& operator=(const TypeRegistry& other) = delete;

	std::unordered_map<std::string, STypeID*> typeIDs;
};
