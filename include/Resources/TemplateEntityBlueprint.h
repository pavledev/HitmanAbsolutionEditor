#pragma once

#include <Glacier/Entity/STemplateEntityBlueprint.h>

#include "Resource.h"

class TemplateEntityBlueprint : public Resource
{
public:
	void Deserialize();
	void Parse(void* templateEntityBlueprint);
	void ParsePropertyAliases(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<SEntityTemplatePropertyAlias>& propertyAliases);
	void ParseExposedEntities(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<TPair<ZString, SEntityTemplateReference>>& exposedEntities);
	void ParseExposedInterfaces(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<TPair<ZString, int>>& exposedInterfaces);
	void ParseEntitySubsets(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<TPair<ZString, SEntityTemplateEntitySubset>>& entitySubsets);
	std::shared_ptr<STemplateEntityBlueprint> GetTemplateEntityBlueprint();

private:
	std::shared_ptr<STemplateEntityBlueprint> templateEntityBlueprint;
};
