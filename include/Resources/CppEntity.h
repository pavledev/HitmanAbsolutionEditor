#pragma once

#include <Glacier/Entity/SCppEntity.h>

#include "Resource.h"

class CppEntity : public Resource
{
public:
	void Deserialize() override;
	void Parse(void* cppEntity);
	void ParseProperties(void* cppEntity, const unsigned int propertyValuesStartAddress, const unsigned int propertyValuesEndAddress, TArray<SEntityTemplateProperty>& properties);
	std::shared_ptr<SCppEntity> GetCppEntity();

private:
	std::shared_ptr<SCppEntity> cppEntity;
};
