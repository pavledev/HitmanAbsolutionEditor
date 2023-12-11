#pragma once

#include <Glacier/Entity/STemplateEntity.h>

#include "Resource.h"

class TemplateEntity : public Resource
{
public:
	void Deserialize();
	void Parse(void* templateEntity);
	void ParseProperties(void* templateEntity, const unsigned int propertyValuesStartAddress, const unsigned int propertyValuesEndAddress, TArray<SEntityTemplateProperty>& properties);
	std::shared_ptr<STemplateEntity> GetTemplateEntity();

private:
	std::shared_ptr<STemplateEntity> templateEntity;
};
