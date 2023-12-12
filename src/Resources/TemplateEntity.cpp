#include "Glacier/Serializer/ZBinaryDeserializer.h"
#include "Glacier/Entity/SEntityTemplateReference.h"
#include "Glacier/ZGameTime.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/ZSharedSensorDef.h"
#include "Glacier/ECameraState.h"
#include "Glacier/SColorRGB.h"
#include "Glacier/SSettingsParamMultiplier.h"
#include "Glacier/ZCurve.h"

#include "Resources/TemplateEntity.h"
#include "Utility/ResourceUtility.h"
#include "Registry/TypeRegistry.h"

void TemplateEntity::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	const std::vector<std::shared_ptr<Resource>>& references = GetReferences();
	void* templateEntity = binaryDeserializer.Deserialize(GetResourceData(), GetResourceDataSize(), &references);

	Parse(templateEntity);

	operator delete(templateEntity, std::align_val_t(binaryDeserializer.GetAlignment()));
}

void TemplateEntity::Parse(void* templateEntity)
{
	this->templateEntity = std::make_shared<STemplateEntity>();

	this->templateEntity->blueprintIndexInResourceHeader = *reinterpret_cast<unsigned int*>(templateEntity);
	this->templateEntity->rootEntityIndex = *(reinterpret_cast<unsigned int*>(templateEntity) + 1);

	const unsigned int entityTemplatesStartAddress = *(reinterpret_cast<unsigned int*>(templateEntity) + 2);
	const unsigned int entityTemplatesEndAddress = *(reinterpret_cast<unsigned int*>(templateEntity) + 3);
	const unsigned int entityTemplateCount = ResourceUtility::CalculateArrayElementsCount(entityTemplatesStartAddress, entityTemplatesEndAddress, 0x20); //0x20 is size of STemplateSubEntity

	this->templateEntity->entityTemplates.Resize(entityTemplateCount);

	for (unsigned int i = 0; i < entityTemplateCount; i++)
	{
		const unsigned int entityTemplateAddress = entityTemplatesStartAddress + 0x20 * i; //0x20 is size of STemplateSubEntity
		STemplateSubEntity* entityTemplate = ResourceUtility::Convert4ByteAddressTo8BytePointer<STemplateSubEntity>(templateEntity, entityTemplateAddress);

		this->templateEntity->entityTemplates[i].parentIndex = entityTemplate->parentIndex;
		this->templateEntity->entityTemplates[i].entityTypeResourceIndex = entityTemplate->entityTypeResourceIndex;

		const unsigned int propertyValuesStartAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 2);
		const unsigned int propertyValuesEndAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 3);
		
		ParseProperties(templateEntity, propertyValuesStartAddress, propertyValuesEndAddress, this->templateEntity->entityTemplates[i].propertyValues);

		const unsigned int postInitPropertyValuesStartAddress = *(reinterpret_cast<unsigned int*>(entityTemplate) + 5);
		const unsigned int postInitPropertyValuesEndAddres = *(reinterpret_cast<unsigned int*>(entityTemplate) + 6);

		ParseProperties(templateEntity, postInitPropertyValuesStartAddress, postInitPropertyValuesEndAddres, this->templateEntity->entityTemplates[i].postInitPropertyValues);
	}
}

void TemplateEntity::ParseProperties(void* templateEntity, const unsigned int propertyValuesStartAddress, const unsigned int propertyValuesEndAddress, TArray<SEntityTemplateProperty>& properties)
{
	const unsigned int propertyValueCount = ResourceUtility::CalculateArrayElementsCount(propertyValuesStartAddress, propertyValuesEndAddress, 0xC); //0xC is size of SEntityTemplateProperty

	properties.Resize(propertyValueCount);

	for (unsigned int i = 0; i < propertyValueCount; i++)
	{
		const unsigned int entityTemplatePropertyAddress = propertyValuesStartAddress + 0xC * i;
		SEntityTemplateProperty* entityTemplateProperty = ResourceUtility::Convert4ByteAddressTo8BytePointer<SEntityTemplateProperty>(templateEntity, entityTemplatePropertyAddress);
		const unsigned int typeIDAddress = *(reinterpret_cast<unsigned int*>(entityTemplateProperty) + 1);
		STypeID* typeID = ResourceUtility::Convert4ByteAddressTo8BytePointer<STypeID>(templateEntity, typeIDAddress);
		const unsigned int typeAddress = reinterpret_cast<unsigned int>(typeID->pTypeInfo);
		IType* type = ResourceUtility::Convert4ByteAddressTo8BytePointer<IType>(templateEntity, typeAddress);
		const unsigned int dataAddress = *(reinterpret_cast<unsigned int*>(entityTemplateProperty) + 2);
		void* data = ResourceUtility::Convert4ByteAddressTo8BytePointer<void>(templateEntity, dataAddress);
		STypeID* typeID2 = TypeRegistry::GetInstance().GetTypeID(type->GetTypeName());

		properties[i].nPropertyID = entityTemplateProperty->nPropertyID;
		properties[i].value.SetTypeID(typeID2);

		const char* typeName = typeID2->pTypeInfo->GetTypeName();

		if (strcmp(typeName, "SEntityTemplateReference") == 0)
		{
			const unsigned int entityIndex = *reinterpret_cast<unsigned int*>(data);
			const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(data) + 2);
			const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntity, charsAddress);
			SEntityTemplateReference* entityTemplateReference = new SEntityTemplateReference();

			entityTemplateReference->entityIndex = entityIndex;
			entityTemplateReference->exposedEntity = ZString(chars);

			properties[i].value.SetData(entityTemplateReference);
		}
		else if (strcmp(typeName, "ZString") == 0)
		{
			const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntity, charsAddress);
			ZString* string = new ZString(chars);

			properties[i].value.SetData(string);
		}
		else if (strcmp(typeName, "ZCurve") == 0)
		{
			const unsigned int arrayStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int arrayEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int arrayCount = ResourceUtility::CalculateArrayElementsCount(arrayStartAddress, arrayEndAddress, 0x8 * sizeof(float));
			ZCurve* curve = new ZCurve();
			TArray<TFixedArray<float, 8>>& data2 = curve->GetData();

			data2.Resize(arrayCount);

			for (unsigned int k = 0; k < arrayCount; k++)
			{
				const unsigned int fixedArrayAddress = arrayStartAddress + 0x8 * sizeof(float) * k;
				TFixedArray<float, 8>* fixedArray = ResourceUtility::Convert4ByteAddressTo8BytePointer<TFixedArray<float, 8>>(templateEntity, fixedArrayAddress);

				for (unsigned int l = 0; l < fixedArray->Size(); ++l)
				{
					data2[k].m_pStart[l] = (*fixedArray)[l];
				}
			}

			properties[i].value.SetData(curve);
		}
		else if (strcmp(typeName, "ZSpeakerLevels") == 0)
		{
			const unsigned int floatsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int floatsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int floatCount = ResourceUtility::CalculateArrayElementsCount(floatsStartAddress, floatsEndAddress, 0x4);
			TArray<float>* floats = new TArray<float>(floatCount);

			for (unsigned int k = 0; k < floatCount; k++)
			{
				const unsigned int floatAddress = floatsStartAddress + 0x4 * k;
				float* value = ResourceUtility::Convert4ByteAddressTo8BytePointer<float>(templateEntity, floatAddress);

				(*floats)[k] = *value;
			}

			properties[i].value.SetData(floats);
		}
		else if (strcmp(typeName, "TArray<SEntityTemplateReference>") == 0)
		{
			const unsigned int entityTemplateReferencesStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int entityTemplateReferencesEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int entityTemplateReferenceCount = ResourceUtility::CalculateArrayElementsCount(entityTemplateReferencesStartAddress, entityTemplateReferencesEndAddress, 0xC); //0x20 is size of STemplateSubEntity
			TArray<SEntityTemplateReference>* entityTemplateReferences = new TArray<SEntityTemplateReference>(entityTemplateReferenceCount);

			for (unsigned int k = 0; k < entityTemplateReferenceCount; k++)
			{
				unsigned int entityTemplateReferenceAddress = entityTemplateReferencesStartAddress + 0xC * k; //0xC is size of SEntityTemplateReference
				SEntityTemplateReference* entityTemplateReference = ResourceUtility::Convert4ByteAddressTo8BytePointer<SEntityTemplateReference>(templateEntity, entityTemplateReferenceAddress);

				unsigned int entityIndex = *reinterpret_cast<unsigned int*>(entityTemplateReference);
				unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(entityTemplateReference) + 2);
				const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntity, charsAddress);
				SEntityTemplateReference entityTemplateReference2;

				entityTemplateReference2.entityIndex = entityIndex;
				entityTemplateReference2.exposedEntity = ZString(chars);

				(*entityTemplateReferences)[k] = entityTemplateReference2;
			}

			properties[i].value.SetData(entityTemplateReferences);
		}
		else if (strcmp(typeName, "TArray<float32>") == 0)
		{
			const unsigned int floatsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int floatsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int floatCount = ResourceUtility::CalculateArrayElementsCount(floatsStartAddress, floatsEndAddress, 0x4);
			TArray<float>* floats = new TArray<float>(floatCount);

			for (unsigned int k = 0; k < floatCount; k++)
			{
				unsigned int floatAddress = floatsStartAddress + 0x4 * k;
				float* value = ResourceUtility::Convert4ByteAddressTo8BytePointer<float>(templateEntity, floatAddress);

				(*floats)[k] = *value;
			}

			properties[i].value.SetData(floats);
		}
		else if (strcmp(typeName, "TArray<ZGameTime>") == 0)
		{
			const unsigned int gameTimesStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int gameTimesEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int gameTimeCount = ResourceUtility::CalculateArrayElementsCount(gameTimesStartAddress, gameTimesEndAddress, 0x8);
			TArray<ZGameTime>* gameTimes = new TArray<ZGameTime>(gameTimeCount);

			for (unsigned int k = 0; k < gameTimeCount; k++)
			{
				unsigned int gameTimeAddress = gameTimesStartAddress + 0x8 * k;
				ZGameTime* gameTime = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZGameTime>(templateEntity, gameTimeAddress);

				(*gameTimes)[k] = *gameTime;
			}

			properties[i].value.SetData(gameTimes);
		}
		else if (strcmp(typeName, "TArray<SVector2>") == 0)
		{
			const unsigned int vectorsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int vectorsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int vectorCount = ResourceUtility::CalculateArrayElementsCount(vectorsStartAddress, vectorsEndAddress, 0x8);
			TArray<SVector2>* vectors = new TArray<SVector2>(vectorCount);

			for (unsigned int k = 0; k < vectorCount; k++)
			{
				const unsigned int vectorAddress = vectorsStartAddress + 0x8 * k;
				SVector2* vector = ResourceUtility::Convert4ByteAddressTo8BytePointer<SVector2>(templateEntity, vectorAddress);

				(*vectors)[k] = *vector;
			}

			properties[i].value.SetData(vectors);
		}
		else if (strcmp(typeName, "TArray<bool>") == 0)
		{
			const unsigned int boolsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int boolsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int boolCount = ResourceUtility::CalculateArrayElementsCount(boolsStartAddress, boolsEndAddress, 0x1);
			TArray<bool>* bools = new TArray<bool>(boolCount);

			for (unsigned int k = 0; k < boolCount; k++)
			{
				const unsigned int boolAddress = boolsStartAddress + 0x1 * k;
				bool* value = ResourceUtility::Convert4ByteAddressTo8BytePointer<bool>(templateEntity, boolAddress);

				(*bools)[k] = (*value);
			}

			properties[i].value.SetData(bools);
		}
		else if (strcmp(typeName, "TArray<ZSharedSensorDef.SVisibilitySetting>") == 0)
		{
			const unsigned int visibilitySettingsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int visibilitySettingsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int visibilitySettingCount = ResourceUtility::CalculateArrayElementsCount(visibilitySettingsStartAddress, visibilitySettingsEndAddress, 0x24);
			TArray<ZSharedSensorDef::SVisibilitySetting>* visibilitySettings = new TArray<ZSharedSensorDef::SVisibilitySetting>(visibilitySettingCount);

			for (unsigned int k = 0; k < visibilitySettingCount; k++)
			{
				const unsigned int visibilitySettingAddress = visibilitySettingsStartAddress + 0x24 * k;
				ZSharedSensorDef::SVisibilitySetting* visibilitySetting = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZSharedSensorDef::SVisibilitySetting>(templateEntity, visibilitySettingAddress);

				(*visibilitySettings)[k] = *visibilitySetting;
			}

			properties[i].value.SetData(visibilitySettings);
		}
		else if (strcmp(typeName, "TArray<ZString>") == 0)
		{
			const unsigned int stringsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int stringsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int stringCount = ResourceUtility::CalculateArrayElementsCount(stringsStartAddress, stringsEndAddress, 0x8);
			TArray<ZString>* strings = new TArray<ZString>(stringCount);

			for (unsigned int k = 0; k < stringCount; k++)
			{
				const unsigned int stringAddress = stringsStartAddress + 0x8 * k;
				ZString* string = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZString>(templateEntity, stringAddress);

				const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(string) + 1);
				const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(templateEntity, charsAddress);

				(*strings)[k] = ZString(chars);
			}

			properties[i].value.SetData(strings);
		}
		else if (strcmp(typeName, "TArray<SSettingsParamMultiplier>") == 0)
		{
			const unsigned int settingsParamMultipliersStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int settingsParamMultipliersEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int settingsParamMultiplierCount = ResourceUtility::CalculateArrayElementsCount(settingsParamMultipliersStartAddress, settingsParamMultipliersEndAddress, 0x8);
			TArray<SSettingsParamMultiplier>* settingsParamMultipliers = new TArray<SSettingsParamMultiplier>(settingsParamMultiplierCount);

			for (unsigned int k = 0; k < settingsParamMultiplierCount; k++)
			{
				const unsigned int settingsParamMultiplierAddress = settingsParamMultipliersStartAddress + 0x8 * k;
				SSettingsParamMultiplier* settingsParamMultiplier = ResourceUtility::Convert4ByteAddressTo8BytePointer<SSettingsParamMultiplier>(templateEntity, settingsParamMultiplierAddress);

				(*settingsParamMultipliers)[k] = *settingsParamMultiplier;
			}

			properties[i].value.SetData(settingsParamMultipliers);
		}
		else if (strcmp(typeName, "TArray<SColorRGB>") == 0)
		{
			const unsigned int colorsStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int colorsEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int colorCount = ResourceUtility::CalculateArrayElementsCount(colorsStartAddress, colorsEndAddress, 0xC);
			TArray<SColorRGB>* colors = new TArray<SColorRGB>(colorCount);

			for (unsigned int k = 0; k < colorCount; k++)
			{
				const unsigned int colorAddress = colorsStartAddress + 0xC * k;
				SColorRGB* colorRGB = ResourceUtility::Convert4ByteAddressTo8BytePointer<SColorRGB>(templateEntity, colorAddress);

				(*colors)[k] = *colorRGB;
			}

			properties[i].value.SetData(colors);
		}
		else if (strcmp(typeName, "TArray<ECameraState>") == 0)
		{
			const unsigned int cameraStatesStartAddress = *reinterpret_cast<unsigned int*>(data);
			const unsigned int cameraStatesEndAddress = *(reinterpret_cast<unsigned int*>(data) + 1);
			const unsigned int cameraStateCount = ResourceUtility::CalculateArrayElementsCount(cameraStatesStartAddress, cameraStatesEndAddress, 0x4);
			TArray<ECameraState>* cameraStates = new TArray<ECameraState>(cameraStateCount);

			for (unsigned int k = 0; k < cameraStateCount; k++)
			{
				const unsigned int cameraStateAddress = cameraStatesStartAddress + 0x4 * k;
				ECameraState* cameraState = ResourceUtility::Convert4ByteAddressTo8BytePointer<ECameraState>(templateEntity, cameraStateAddress);

				(*cameraStates)[k] = *cameraState;
			}

			properties[i].value.SetData(cameraStates);
		}
		else
		{
			void* data2 = operator new(type->GetTypeSize());

			memcpy(data2, data, type->GetTypeSize());

			properties[i].value.SetData(data2);
		}
	}
}

std::shared_ptr<STemplateEntity> TemplateEntity::GetTemplateEntity()
{
	return templateEntity;
}
