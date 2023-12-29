#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/TemplateEntityBlueprint.h"
#include "Utility/ResourceUtility.h"
#include "Registry/TypeRegistry.h"

void TemplateEntityBlueprint::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* templateEntityBlueprint = binaryDeserializer.Deserialize(GetResourceData(), GetResourceDataSize(), &references);

	Parse(templateEntityBlueprint);

	operator delete(templateEntityBlueprint, std::align_val_t(binaryDeserializer.GetAlignment()));

	isResourceDeserialized = true;
}

void TemplateEntityBlueprint::Parse(void* templateEntityBlueprint)
{
	this->templateEntityBlueprint = std::make_shared<STemplateEntityBlueprint>();

	this->templateEntityBlueprint->rootEntityIndex = *reinterpret_cast<unsigned int*>(templateEntityBlueprint);

	const unsigned int entityTemplatesStartAddress = *(reinterpret_cast<unsigned int*>(templateEntityBlueprint) + 1);
	const unsigned int entityTemplatesEndAddress = *(reinterpret_cast<unsigned int*>(templateEntityBlueprint) + 2);
	const unsigned int entityTemplateCount = ResourceUtility::CalculateArrayElementsCount(entityTemplatesStartAddress, entityTemplatesEndAddress, 0x40); //0x40 is size of STemplateSubEntityBlueprint

	this->templateEntityBlueprint->entityTemplates.Resize(entityTemplateCount);

	for (unsigned int i = 0; i < entityTemplateCount; i++)
	{
		const unsigned int entityTemplateAddress = entityTemplatesStartAddress + 0x40 * i; //0x20 is size of STemplateSubEntityBlueprint
		STemplateSubEntityBlueprint* entityTemplate = ResourceUtility::Convert4ByteAddressTo8BytePointer<STemplateSubEntityBlueprint>(templateEntityBlueprint, entityTemplateAddress);

		this->templateEntityBlueprint->entityTemplates[i].parentIndex = entityTemplate->parentIndex;
		this->templateEntityBlueprint->entityTemplates[i].entityTypeResourceIndex = entityTemplate->entityTypeResourceIndex;

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 3);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress);

		this->templateEntityBlueprint->entityTemplates[i].entityName = ZString(chars);

		ParsePropertyAliases(templateEntityBlueprint, entityTemplate, this->templateEntityBlueprint->entityTemplates[i].propertyAliases);
		ParseExposedEntities(templateEntityBlueprint, entityTemplate, this->templateEntityBlueprint->entityTemplates[i].exposedEntities);
		ParseExposedInterfaces(templateEntityBlueprint, entityTemplate, this->templateEntityBlueprint->entityTemplates[i].exposedInterfaces);
		ParseEntitySubsets(templateEntityBlueprint, entityTemplate, this->templateEntityBlueprint->entityTemplates[i].entitySubsets);
	}
}

void TemplateEntityBlueprint::ParsePropertyAliases(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<SEntityTemplatePropertyAlias>& propertyAliases)
{
	const unsigned int propertyAliasesStartAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 4);
	const unsigned int propertyAliasesEndAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 5);
	const unsigned int propertyAliasCount = ResourceUtility::CalculateArrayElementsCount(propertyAliasesStartAddress, propertyAliasesEndAddress, 0x14); //0x14 is size of SEntityTemplatePropertyAlias

	propertyAliases.Resize(propertyAliasCount);

	for (unsigned int i = 0; i < propertyAliasCount; i++)
	{
		const unsigned int propertyAliasAddress = propertyAliasesStartAddress + 0x14 * i;
		SEntityTemplatePropertyAlias* entityTemplatePropertyAlias = ResourceUtility::Convert4ByteAddressTo8BytePointer<SEntityTemplatePropertyAlias>(templateEntityBlueprint, propertyAliasAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(entityTemplatePropertyAlias) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress);

		const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(entityTemplatePropertyAlias) + 4);
		const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress2);

		propertyAliases[i].sAliasName = ZString(chars);
		propertyAliases[i].entityID = *(reinterpret_cast<unsigned int*>(entityTemplatePropertyAlias) + 2);
		propertyAliases[i].sPropertyName = ZString(chars2);
	}
}

void TemplateEntityBlueprint::ParseExposedEntities(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<TPair<ZString, SEntityTemplateReference>>& exposedEntities)
{
	const unsigned int exposedEntitiesStartAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 7);
	const unsigned int exposedEntitiesEndAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 8);
	const unsigned int exposedEntityCount = ResourceUtility::CalculateArrayElementsCount(exposedEntitiesStartAddress, exposedEntitiesEndAddress, 0x14); //0x14 is size of TPair<ZString, SEntityTemplateReference>

	exposedEntities.Resize(exposedEntityCount);

	for (unsigned int i = 0; i < exposedEntityCount; i++)
	{
		const unsigned int exposedEntityAddress = exposedEntitiesStartAddress + 0x14 * i;
		TPair<ZString, SEntityTemplateReference>* exposedEntity = ResourceUtility::Convert4ByteAddressTo8BytePointer<TPair<ZString, SEntityTemplateReference>>(templateEntityBlueprint, exposedEntityAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(exposedEntity) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress);

		const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(exposedEntity) + 4);
		const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress2);

		SEntityTemplateReference entityTemplateReference;

		entityTemplateReference.entityIndex = *(reinterpret_cast<unsigned int*>(exposedEntity) + 2);
		entityTemplateReference.exposedEntity = ZString(chars2);

		exposedEntities[i] = TPair<ZString, SEntityTemplateReference>(chars, entityTemplateReference);
	}
}

void TemplateEntityBlueprint::ParseExposedInterfaces(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<TPair<ZString, int>>& exposedInterfaces)
{
	const unsigned int exposedInterfacesStartAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 10);
	const unsigned int exposedInterfacesEndAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 11);
	const unsigned int exposedInterfaceCount = ResourceUtility::CalculateArrayElementsCount(exposedInterfacesStartAddress, exposedInterfacesEndAddress, 0xC); //0xC is size of TPair<ZString, int>

	exposedInterfaces.Resize(exposedInterfaceCount);

	for (unsigned int i = 0; i < exposedInterfaceCount; i++)
	{
		const unsigned int exposedInterfaceAddress = exposedInterfacesStartAddress + 0xC * i;
		TPair<ZString, int>* exposedInterface = ResourceUtility::Convert4ByteAddressTo8BytePointer<TPair<ZString, int>>(templateEntityBlueprint, exposedInterfaceAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(exposedInterface) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress);

		exposedInterfaces[i] = TPair<ZString, int>(chars, *(reinterpret_cast<unsigned int*>(exposedInterface) + 2));
	}
}

void TemplateEntityBlueprint::ParseEntitySubsets(void* templateEntityBlueprint, STemplateSubEntityBlueprint* entityTemplate, TArray<TPair<ZString, SEntityTemplateEntitySubset>>& entitySubsets)
{
	const unsigned int entitySubsetsStartAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 13);
	const unsigned int entitySubsetsEndAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 14);
	const unsigned int entitySubsetCount = ResourceUtility::CalculateArrayElementsCount(entitySubsetsStartAddress, entitySubsetsEndAddress, 0x18); //0x18 is size of TPair<ZString, SEntityTemplateEntitySubset>

	entitySubsets.Resize(entitySubsetCount);

	for (unsigned int i = 0; i < entitySubsetCount; i++)
	{
		const unsigned int entitySubsetAddress = entitySubsetsStartAddress + 0x18 * i;
		TPair<ZString, SEntityTemplateEntitySubset>* entitySubset = ResourceUtility::Convert4ByteAddressTo8BytePointer<TPair<ZString, SEntityTemplateEntitySubset>>(templateEntityBlueprint, entitySubsetAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(entitySubset) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntityBlueprint, charsAddress);

		const unsigned int typeIDAddress = *(reinterpret_cast<unsigned int*>(entitySubset) + 2);
		STypeID* typeID = ResourceUtility::Convert4ByteAddressTo8BytePointer<STypeID>(templateEntityBlueprint, typeIDAddress);
		const unsigned int typeAddress = reinterpret_cast<unsigned int>(typeID->pTypeInfo);
		IType* type = ResourceUtility::Convert4ByteAddressTo8BytePointer<IType>(templateEntityBlueprint, typeAddress);
		STypeID* typeID2 = TypeRegistry::GetInstance().GetTypeID(type->GetTypeName());
		SEntityTemplateEntitySubset entityTemplateEntitySubset;

		entityTemplateEntitySubset.subsetType = typeID2;

		const unsigned int entitiesStartAddress = *(reinterpret_cast<unsigned int*>(entitySubset) + 3);
		const unsigned int entitiesEndAddress = *(reinterpret_cast<unsigned int*>(entitySubset) + 4);
		const unsigned int entityCount = ResourceUtility::CalculateArrayElementsCount(entitiesStartAddress, entitiesEndAddress, sizeof(int));

		entityTemplateEntitySubset.entities.Resize(entityCount);

		for (unsigned int j = 0; j < entityCount; j++)
		{
			unsigned int intAddress = entitiesStartAddress + sizeof(int) * j;
			int* entity = ResourceUtility::Convert4ByteAddressTo8BytePointer<int>(templateEntityBlueprint, intAddress);

			entityTemplateEntitySubset.entities[j] = *entity;
		}

		entitySubsets[i] = TPair<ZString, SEntityTemplateEntitySubset>(chars, entityTemplateEntitySubset);
	}
}

std::shared_ptr<STemplateEntityBlueprint> TemplateEntityBlueprint::GetTemplateEntityBlueprint()
{
	return templateEntityBlueprint;
}
