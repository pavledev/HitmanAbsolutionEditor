#pragma once

#include "../ZString.h"
#include "../Containers/TArray.h"
#include "../Containers/TPair.h"
#include "SEntityTemplatePropertyAlias.h"
#include "SEntityTemplateReference.h"
#include "SEntityTemplateEntitySubset.h"

struct STemplateSubEntityBlueprint
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer, unsigned int entityIndex);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static STemplateSubEntityBlueprint* DeserializeFromJson(const rapidjson::Value& object);

	int parentIndex;
	int entityTypeResourceIndex;
	ZString entityName;
	TArray<SEntityTemplatePropertyAlias> propertyAliases;
	TArray<TPair<ZString, SEntityTemplateReference>> exposedEntities;
	TArray<TPair<ZString, int>> exposedInterfaces;
	TArray<TPair<ZString, SEntityTemplateEntitySubset>> entitySubsets;
};
