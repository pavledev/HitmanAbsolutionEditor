#include <format>

#include "Glacier/ZVariant.h"
#include "Glacier/Serializer/ZBinarySerializer.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector3.h"
#include "Glacier/Math/SMatrix43.h"
#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Entity/SEntityTemplateReference.h"
#include "Glacier/Resource/ZRuntimeResourceID.h"
#include "Glacier/Containers/TArray.h"
#include "Glacier/ZCurve.h"
#include "Glacier/ZGameTime.h"
#include "Glacier/ZSharedSensorDef.h"
#include "Glacier/Sound/ZSpeakerLevels.h"
#include "Glacier/ECameraState.h"
#include "Glacier/SBodyPartDamageMultipliers.h"
#include "Glacier/SSettingsParamMultiplier.h"
#include "Glacier/Entity/ZEntityReference.h"
#include "Registry/EnumRegistry.h"

ZVariant::ZVariant() : ZObjectRef()
{

}

ZVariant::ZVariant(const ZVariant& other)
{
	Set(other.m_TypeID, other.m_pData);
}

ZVariant::~ZVariant()
{
	if (m_TypeID->pTypeInfo->IsPrimitiveType() || m_TypeID->pTypeInfo->IsEnumType())
	{
		operator delete(m_pData);
	}
	else
	{
		switch (m_TypeID->typeNum)
		{
			case 12:
				delete static_cast<SVector2*>(m_pData);
				break;
			case 13:
				delete static_cast<SVector3*>(m_pData);
				break;
			case 14:
				delete static_cast<SMatrix43*>(m_pData);
				break;
			case 15:
				delete static_cast<SColorRGB*>(m_pData);
				break;
			case 16:
				delete static_cast<SColorRGBA*>(m_pData);
				break;
			case 17:
				delete static_cast<SEntityTemplateReference*>(m_pData);
				break;
			case 18:
				delete static_cast<SBodyPartDamageMultipliers*>(m_pData);
				break;
			case 19:
				delete static_cast<ZRuntimeResourceID*>(m_pData);
				break;
			case 20:
				delete static_cast<ZString*>(m_pData);
				break;
			case 21:
				delete static_cast<ZCurve*>(m_pData);
				break;
			case 22:
				delete static_cast<ZGameTime*>(m_pData);
				break;
			case 23:
				delete static_cast<ZSpeakerLevels*>(m_pData);
				break;
			case 25:
				delete static_cast<TArray<SEntityTemplateReference>*>(m_pData);
				break;
			case 26:
				delete static_cast<TArray<float>*>(m_pData);
				break;
			case 27:
				delete static_cast<TArray<ZGameTime>*>(m_pData);
				break;
			case 28:
				delete static_cast<TArray<SVector2>*>(m_pData);
				break;
			case 29:
				delete static_cast<TArray<bool>*>(m_pData);
				break;
			case 30:
				delete static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(m_pData);
				break;
			case 31:
				delete static_cast<TArray<ZString>*>(m_pData);
				break;
			case 32:
				delete static_cast<TArray<SSettingsParamMultiplier>*>(m_pData);
				break;
			case 33:
				delete static_cast<TArray<SColorRGB>*>(m_pData);
				break;
			case 34:
				delete static_cast<TArray<ECameraState>*>(m_pData);
				break;
		}
	}

	m_pData = nullptr;
}

void ZVariant::Allocate(STypeID* type)
{
	m_TypeID = type;

	if (m_TypeID->pTypeInfo->IsPrimitiveType() || m_TypeID->pTypeInfo->IsEnumType())
	{
		m_pData = operator new(m_TypeID->pTypeInfo->GetTypeSize());
	}
	else
	{
		switch (m_TypeID->typeNum)
		{
			case 12:
				m_pData = new SVector2();
				break;
			case 13:
				m_pData = new SVector3();
				break;
			case 14:
				m_pData = new SMatrix43();
				break;
			case 15:
				m_pData = new SColorRGB();
				break;
			case 16:
				m_pData = new SColorRGBA();
				break;
			case 17:
				m_pData = new SEntityTemplateReference();
				break;
			case 18:
				m_pData = new SBodyPartDamageMultipliers();
				break;
			case 19:
				m_pData = new ZRuntimeResourceID();
				break;
			case 20:
				m_pData = new ZString();
				break;
			case 21:
				m_pData = new ZCurve();
				break;
			case 22:
				m_pData = new ZGameTime();
				break;
			case 23:
				m_pData = new ZSpeakerLevels();
				break;
			case 25:
				m_pData = new TArray<SEntityTemplateReference>();
				break;
			case 26:
				m_pData = new TArray<float>();
				break;
			case 27:
				m_pData = new TArray<ZGameTime>();
				break;
			case 28:
				m_pData = new TArray<SVector2>();
				break;
			case 29:
				m_pData = new TArray<bool>();
				break;
			case 30:
				m_pData = new TArray<ZSharedSensorDef::SVisibilitySetting>();
				break;
			case 31:
				m_pData = new TArray<ZString>();
				break;
			case 32:
				m_pData = new TArray<SSettingsParamMultiplier>();
				break;
			case 33:
				m_pData = new TArray<SColorRGB>();
				break;
			case 34:
				m_pData = new TArray<ECameraState>();
				break;
		}
	}

	memset(m_pData, 0, m_TypeID->pTypeInfo->GetTypeSize());
}

void ZVariant::Set(STypeID* type, const void* pData)
{
	Allocate(type);

	if (m_TypeID->pTypeInfo->IsPrimitiveType() || m_TypeID->pTypeInfo->IsEnumType())
	{
		m_pData = operator new(m_TypeID->pTypeInfo->GetTypeSize());

		memcpy(m_pData, pData, m_TypeID->pTypeInfo->GetTypeSize());
	}
	else
	{
		switch (m_TypeID->typeNum)
		{
			case 12:
				*static_cast<SVector2*>(m_pData) = *static_cast<const SVector2*>(pData);
				break;
			case 13:
				*static_cast<SVector3*>(m_pData) = *static_cast<const SVector3*>(pData);
				break;
			case 14:
				*static_cast<SMatrix43*>(m_pData) = *static_cast<const SMatrix43*>(pData);
				break;
			case 15:
				*static_cast<SColorRGB*>(m_pData) = *static_cast<const SColorRGB*>(pData);
				break;
			case 16:
				*static_cast<SColorRGBA*>(m_pData) = *static_cast<const SColorRGBA*>(pData);
				break;
			case 17:
				*static_cast<SEntityTemplateReference*>(m_pData) = *static_cast<const SEntityTemplateReference*>(pData);
				break;
			case 18:
				*static_cast<SBodyPartDamageMultipliers*>(m_pData) = *static_cast<const SBodyPartDamageMultipliers*>(pData);
				break;
			case 19:
				*static_cast<ZRuntimeResourceID*>(m_pData) = *static_cast<const ZRuntimeResourceID*>(pData);
				break;
			case 20:
				*static_cast<ZString*>(m_pData) = *static_cast<const ZString*>(pData);
				break;
			case 21:
				*static_cast<ZCurve*>(m_pData) = *static_cast<const ZCurve*>(pData);
				break;
			case 22:
				*static_cast<ZGameTime*>(m_pData) = *static_cast<const ZGameTime*>(pData);
				break;
			case 23:
				*static_cast<ZSpeakerLevels*>(m_pData) = *static_cast<const ZSpeakerLevels*>(pData);
				break;
			case 25:
				*static_cast<TArray<SEntityTemplateReference>*>(m_pData) = *static_cast<const TArray<SEntityTemplateReference>*>(pData);
				break;
			case 26:
				*static_cast<TArray<float>*>(m_pData) = *static_cast<const TArray<float>*>(pData);
				break;
			case 27:
				*static_cast<TArray<ZGameTime>*>(m_pData) = *static_cast<const TArray<ZGameTime>*>(pData);
				break;
			case 28:
				*static_cast<TArray<SVector2>*>(m_pData) = *static_cast<const TArray<SVector2>*>(pData);
				break;
			case 29:
				*static_cast<TArray<bool>*>(m_pData) = *static_cast<const TArray<bool>*>(pData);
				break;
			case 30:
				*static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(m_pData) = *static_cast<const TArray<ZSharedSensorDef::SVisibilitySetting>*>(pData);
				break;
			case 31:
				*static_cast<TArray<ZString>*>(m_pData) = *static_cast<const TArray<ZString>*>(pData);
				break;
			case 32:
				*static_cast<TArray<SSettingsParamMultiplier>*>(m_pData) = *static_cast<const TArray<SSettingsParamMultiplier>*>(pData);
				break;
			case 33:
				*static_cast<TArray<SColorRGB>*>(m_pData) = *static_cast<const TArray<SColorRGB>*>(pData);
				break;
			case 34:
				*static_cast<TArray<ECameraState>*>(m_pData) = *static_cast<const TArray<ECameraState>*>(pData);
				break;
		}
	}
}

void ZVariant::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("typeName");
	m_TypeID->SerializeToJson(writer);

	writer.String("value");

	std::string typeName = m_TypeID->pTypeInfo->GetTypeName();

	if (!SerializeData(writer))
	{
		throw std::invalid_argument(std::format("Type {} isn't supported", typeName));
	}

	writer.EndObject();
}

void ZVariant::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int typeIDOffset = offset + offsetof(ZVariant, m_TypeID);
	unsigned int dataOffset = offset + offsetof(ZVariant, m_pData);
	unsigned int nullPointer = -1;

	binarySerializer.RecordOffsetForTypeIDReindexing(typeIDOffset);

	if (m_TypeID)
	{
		unsigned int typeIndex = binarySerializer.TypeIdToIndex(m_TypeID);

		binarySerializer.WriteToMemory(&typeIndex, sizeof(unsigned int), typeIDOffset);

		if (m_pData)
		{
			std::optional<unsigned int> result = binarySerializer.GetDataOffset(this);
			unsigned int dataOffset2;

			if (result.has_value())
			{
				dataOffset2 = result.value();
			}
			else
			{
				unsigned int typeSize = m_TypeID->pTypeInfo->GetTypeSize();
				unsigned int typeAlignment = m_TypeID->pTypeInfo->GetTypeAlignment();

				dataOffset2 = binarySerializer.GetAlignedOffset(typeSize, typeAlignment);

				std::string typeName = m_TypeID->pTypeInfo->GetTypeName();

				if (m_TypeID->pTypeInfo->IsPrimitiveType() || m_TypeID->pTypeInfo->IsEnumType())
				{
					binarySerializer.WriteToMemory(m_pData, typeSize, dataOffset2);
				}
				else if (typeName == "SVector2")
				{
					static_cast<SVector2*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "SVector3")
				{
					static_cast<SVector3*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "SMatrix43")
				{
					static_cast<SMatrix43*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "SColorRGB")
				{
					static_cast<SColorRGB*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "SColorRGBA")
				{
					static_cast<SColorRGBA*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "SEntityTemplateReference")
				{
					static_cast<SEntityTemplateReference*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "SBodyPartDamageMultipliers")
				{
					static_cast<SBodyPartDamageMultipliers*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "ZRuntimeResourceID")
				{
					static_cast<ZRuntimeResourceID*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "ZString")
				{
					static_cast<ZString*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "ZCurve")
				{
					static_cast<ZCurve*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "ZGameTime")
				{
					static_cast<ZGameTime*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "ZSpeakerLevels")
				{
					static_cast<ZSpeakerLevels*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<SEntityTemplateReference>")
				{
					static_cast<TArray<SEntityTemplateReference>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<float32>")
				{
					static_cast<TArray<float>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<ZGameTime>")
				{
					static_cast<TArray<ZGameTime>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<SVector2>")
				{
					static_cast<TArray<SVector2>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<bool>")
				{
					static_cast<TArray<bool>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
				{
					static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<ZString>")
				{
					static_cast<TArray<ZString>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<SSettingsParamMultiplier>")
				{
					static_cast<TArray<SSettingsParamMultiplier>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<SColorRGB>")
				{
					static_cast<TArray<SColorRGB>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}
				else if (typeName == "TArray<ECameraState>")
				{
					static_cast<TArray<ECameraState>*>(m_pData)->SerializeToMemory(binarySerializer, dataOffset2);
				}

				binarySerializer.SetDataOffset(this, dataOffset2);
			}

			binarySerializer.WriteToMemory(&dataOffset2, sizeof(unsigned int), dataOffset);
		}
		else
		{
			binarySerializer.WriteToMemory(&nullPointer, sizeof(unsigned int), dataOffset);
		}
	}
	else
	{
		binarySerializer.WriteToMemory(&nullPointer, sizeof(unsigned int), typeIDOffset);
		binarySerializer.WriteToMemory(&nullPointer, sizeof(unsigned int), dataOffset);
	}

	binarySerializer.RecordOffsetForRebasing(dataOffset);
}

bool ZVariant::SerializeData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	std::string typeName = m_TypeID->pTypeInfo->GetTypeName();

	if (typeName == "bool")
	{
		writer.Bool(*static_cast<bool*>(m_pData));
	}
	else if (typeName == "int8" || typeName == "char")
	{
		writer.Int(*static_cast<char*>(m_pData));
	}
	else if (typeName == "uint8")
	{
		writer.Uint(*static_cast<unsigned char*>(m_pData));
	}
	else if (typeName == "int16")
	{
		writer.Int(*static_cast<short*>(m_pData));
	}
	else if (typeName == "uint16")
	{
		writer.Uint(*static_cast<unsigned short*>(m_pData));
	}
	else if (typeName == "int32")
	{
		writer.Int(*static_cast<int*>(m_pData));
	}
	else if (typeName == "uint32")
	{
		writer.Uint(*static_cast<unsigned int*>(m_pData));
	}
	else if (typeName == "int64")
	{
		writer.Int64(*static_cast<long long*>(m_pData));
	}
	else if (typeName == "uint64")
	{
		writer.Uint64(*static_cast<unsigned long long*>(m_pData));
	}
	else if (typeName == "float32")
	{
		writer.Double(*static_cast<float*>(m_pData));
	}
	else if (typeName == "float64")
	{
		writer.Double(*static_cast<double*>(m_pData));
	}
	else if (typeName == "SVector2")
	{
		static_cast<SVector2*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "SVector3")
	{
		static_cast<SVector3*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "SMatrix43")
	{
		static_cast<SMatrix43*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "SColorRGB")
	{
		static_cast<SColorRGB*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "SColorRGBA")
	{
		static_cast<SColorRGBA*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "SEntityTemplateReference")
	{
		static_cast<SEntityTemplateReference*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "SBodyPartDamageMultipliers")
	{
		static_cast<SBodyPartDamageMultipliers*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "ZRuntimeResourceID")
	{
		static_cast<ZRuntimeResourceID*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "ZString")
	{
		static_cast<ZString*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "ZCurve")
	{
		static_cast<ZCurve*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "ZGameTime")
	{
		static_cast<ZGameTime*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "ZSpeakerLevels")
	{
		static_cast<ZSpeakerLevels*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "ZEntityReference")
	{
		static_cast<ZEntityReference*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<SEntityTemplateReference>")
	{
		static_cast<TArray<SEntityTemplateReference>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<float32>")
	{
		static_cast<TArray<float>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<ZGameTime>")
	{
		static_cast<TArray<ZGameTime>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<SVector2>")
	{
		static_cast<TArray<SVector2>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<bool>")
	{
		static_cast<TArray<bool>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
	{
		static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<ZString>")
	{
		static_cast<TArray<ZString>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<SSettingsParamMultiplier>")
	{
		static_cast<TArray<SSettingsParamMultiplier>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<SColorRGB>")
	{
		static_cast<TArray<SColorRGB>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<ECameraState>")
	{
		static_cast<TArray<ECameraState>*>(m_pData)->SerializeToJson(writer);
	}
	else if (typeName == "TArray<ZEntityReference>")
	{
		static_cast<TArray<ZEntityReference>*>(m_pData)->SerializeToJson(writer);
	}
	else if (m_TypeID->pTypeInfo->IsEnumType())
	{
		const std::map<int, std::string>& items = EnumRegistry::GetInstance().GetEnum(typeName);

		if (items.size() == 0)
		{
			return false;
		}

		int value = *static_cast<int*>(m_pData);

		for (auto it = items.begin(); it != items.end(); it++)
		{
			if (it->first == value)
			{
				writer.String(it->second.c_str());

				break;
			}
		}
	}
	else
	{
		return false;
	}

	return true;
}

ZVariant* ZVariant::DeserializeFromJson(const rapidjson::Value& object)
{
	ZVariant* variant = new ZVariant();
	std::string typeName = object["typeName"].GetString();
	static std::unordered_map<STypeID*, std::vector<void*>> values;

	variant->m_TypeID = STypeID::DeserializeFromJson(object["typeName"].GetString());

	if (typeName == "bool")
	{
		bool value2 = object["value"].GetBool();

		variant->m_pData = operator new(sizeof(bool));

		memcpy(variant->m_pData, &value2, sizeof(bool));
	}
	else if (typeName == "int8" || typeName == "char")
	{
		char value = object["value"].GetInt();

		variant->m_pData = operator new(sizeof(char));

		memcpy(variant->m_pData, &value, sizeof(char));
	}
	else if (typeName == "uint8")
	{
		unsigned char value = object["value"].GetUint();

		variant->m_pData = operator new(sizeof(unsigned char));

		memcpy(variant->m_pData, &value, sizeof(unsigned char));
	}
	else if (typeName == "int16")
	{
		short value = object["value"].GetInt();

		variant->m_pData = operator new(sizeof(short));

		memcpy(variant->m_pData, &value, sizeof(short));
	}
	else if (typeName == "uint16")
	{
		unsigned short value = object["value"].GetUint();

		variant->m_pData = operator new(sizeof(unsigned short));

		memcpy(variant->m_pData, &value, sizeof(unsigned short));
	}
	else if (typeName == "int32")
	{
		int value = object["value"].GetInt();

		variant->m_pData = operator new(sizeof(int));

		memcpy(variant->m_pData, &value, sizeof(int));
	}
	else if (typeName == "uint32")
	{
		unsigned int value = object["value"].GetUint();

		variant->m_pData = operator new(sizeof(unsigned int));

		memcpy(variant->m_pData, &value, sizeof(unsigned int));
	}
	else if (typeName == "int64")
	{
		long long value = object["value"].GetInt64();

		variant->m_pData = operator new(sizeof(long long));

		memcpy(variant->m_pData, &value, sizeof(long long));
	}
	else if (typeName == "uint64")
	{
		unsigned long long value2 = object["value"].GetUint64();

		variant->m_pData = operator new(sizeof(unsigned long long));

		memcpy(variant->m_pData, &value2, sizeof(unsigned long long));
	}
	else if (typeName == "float32")
	{
		float value2 = object["value"].GetFloat();

		variant->m_pData = operator new(sizeof(float));

		memcpy(variant->m_pData, &value2, sizeof(float));
	}
	else if (typeName == "float64")
	{
		double value2 = object["value"].GetDouble();

		variant->m_pData = operator new(sizeof(double));

		memcpy(variant->m_pData, &value2, sizeof(double));
	}
	else if (typeName == "SVector2")
	{
		variant->m_pData = SVector2::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "SVector3")
	{
		variant->m_pData = SVector3::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "SMatrix43")
	{
		variant->m_pData = SMatrix43::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "SColorRGB")
	{
		variant->m_pData = SColorRGB::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "SColorRGBA")
	{
		variant->m_pData = SColorRGBA::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "SEntityTemplateReference")
	{
		variant->m_pData = SEntityTemplateReference::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "SBodyPartDamageMultipliers")
	{
		variant->m_pData = SBodyPartDamageMultipliers::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "ZRuntimeResourceID")
	{
		variant->m_pData = ZRuntimeResourceID::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "ZString")
	{
		variant->m_pData = ZString::DeserializeFromJson(object["value"].GetString());
	}
	else if (typeName == "ZCurve")
	{
		variant->m_pData = ZCurve::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "ZGameTime")
	{
		variant->m_pData = ZGameTime::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "ZSpeakerLevels")
	{
		variant->m_pData = ZSpeakerLevels::DeserializeFromJson(object["value"].GetObj());
	}
	else if (typeName == "TArray<float32>")
	{
		variant->m_pData = TArray<float>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<SEntityTemplateReference>")
	{
		variant->m_pData = TArray<SEntityTemplateReference>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<ZGameTime>")
	{
		variant->m_pData = TArray<ZGameTime>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<SVector2>")
	{
		variant->m_pData = TArray<SVector2>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<bool>")
	{
		variant->m_pData = TArray<bool>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
	{
		variant->m_pData = TArray<ZSharedSensorDef::SVisibilitySetting>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<ZString>")
	{
		variant->m_pData = TArray<ZString>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<SSettingsParamMultiplier>")
	{
		variant->m_pData = TArray<SSettingsParamMultiplier>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<SColorRGB>")
	{
		variant->m_pData = TArray<SColorRGB>::DeserializeFromJson(object["value"].GetArray());
	}
	else if (typeName == "TArray<ECameraState>")
	{
		variant->m_pData = TArray<ECameraState>::DeserializeFromJson(object["value"].GetArray());
	}
	else
	{
		std::string value = object["value"].GetString();
		const std::map<int, std::string>& items = EnumRegistry::GetInstance().GetEnum(typeName);

		variant->m_pData = operator new(sizeof(unsigned int));

		for (auto it = items.begin(); it != items.end(); it++)
		{
			if (it->second == value)
			{
				memcpy(variant->m_pData, &it->first, sizeof(unsigned int));

				break;
			}
		}
	}

	//Check whether SSettingsParamMultiplier, TArray<ZSharedSensorDef::SVisibilitySetting>, TArray<ECameraState>, ZEntityReference and ZEntityID should be compared because of enum
	void* value = GetValue(variant, values);

	if (value)
	{
		variant->m_pData = value;
	}
	else
	{
		//SEntityTemplateReference objects, TArray and enums aren't duplicated
		if (typeName != "SEntityTemplateReference" &&
			!typeName.starts_with("TArray") &&
			!variant->m_TypeID->pTypeInfo->IsEnumType())
		{
			values[variant->m_TypeID].push_back(variant->m_pData);
		}
	}

	return variant;
}

void* ZVariant::GetValue(const ZVariant* variant, const std::unordered_map<STypeID*, std::vector<void*>>& values)
{
	auto it = values.find(variant->m_TypeID);
	void* result = nullptr;

	if (it != values.end())
	{
		std::string typeName = variant->m_TypeID->pTypeInfo->GetTypeName();
		bool found = false;
		int index = 0;

		for (unsigned int i = 0; i < it->second.size(); ++i)
		{
			if ((typeName == "bool") &&
				*static_cast<bool*>(variant->m_pData) == *static_cast<bool*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "int8" || typeName == "char") &&
				*static_cast<char*>(variant->m_pData) == *static_cast<char*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "uint8") &&
				*static_cast<unsigned char*>(variant->m_pData) == *static_cast<unsigned char*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "int16") &&
				*static_cast<short*>(variant->m_pData) == *static_cast<short*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "uint16") &&
				*static_cast<unsigned short*>(variant->m_pData) == *static_cast<unsigned short*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "int32") &&
				*static_cast<int*>(variant->m_pData) == *static_cast<int*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "uint32") &&
				*static_cast<unsigned int*>(variant->m_pData) == *static_cast<unsigned int*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "int64") &&
				*static_cast<long long*>(variant->m_pData) == *static_cast<long long*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "uint64") &&
				*static_cast<unsigned long long*>(variant->m_pData) == *static_cast<unsigned long long*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "float32") &&
				*static_cast<float*>(variant->m_pData) == *static_cast<float*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "float64") &&
				*static_cast<double*>(variant->m_pData) == *static_cast<double*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "SVector2") &&
				*static_cast<SVector2*>(variant->m_pData) == *static_cast<SVector2*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "SVector3") &&
				*static_cast<SVector3*>(variant->m_pData) == *static_cast<SVector3*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "SMatrix43") &&
				*static_cast<SMatrix43*>(variant->m_pData) == *static_cast<SMatrix43*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "SColorRGB") &&
				*static_cast<SColorRGB*>(variant->m_pData) == *static_cast<SColorRGB*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "SColorRGBA") &&
				*static_cast<SColorRGBA*>(variant->m_pData) == *static_cast<SColorRGBA*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			/*else if ((typeName == "SEntityTemplateReference") &&
				*static_cast<SEntityTemplateReference*>(variant->m_pData) == *static_cast<SEntityTemplateReference*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			else if ((typeName == "SBodyPartDamageMultipliers") &&
				*static_cast<SBodyPartDamageMultipliers*>(variant->m_pData) == *static_cast<SBodyPartDamageMultipliers*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "ZRuntimeResourceID") &&
				*static_cast<ZRuntimeResourceID*>(variant->m_pData) == *static_cast<ZRuntimeResourceID*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "ZString") &&
				*static_cast<ZString*>(variant->m_pData) == *static_cast<ZString*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "ZCurve") &&
				*static_cast<ZCurve*>(variant->m_pData) == *static_cast<ZCurve*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "ZGameTime") &&
				*static_cast<ZGameTime*>(variant->m_pData) == *static_cast<ZGameTime*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			else if ((typeName == "ZSpeakerLevels") &&
				*static_cast<ZSpeakerLevels*>(variant->m_pData) == *static_cast<ZSpeakerLevels*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}
			/*else if ((typeName == "TArray<SEntityTemplateReference>") &&
				*static_cast<TArray<SEntityTemplateReference>*>(variant->m_pData) == *static_cast<TArray<SEntityTemplateReference>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<float32>") &&
				*static_cast<TArray<float>*>(variant->m_pData) == *static_cast<TArray<float>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<ZGameTime>") &&
				*static_cast<TArray<ZGameTime>*>(variant->m_pData) == *static_cast<TArray<ZGameTime>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<SVector2>") &&
				*static_cast<TArray<SVector2>*>(variant->m_pData) == *static_cast<TArray<SVector2>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<bool>") &&
				*static_cast<TArray<bool>*>(variant->m_pData) == *static_cast<TArray<bool>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>") &&
				*static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(variant->m_pData) == *static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<ZString>") &&
				*static_cast<TArray<ZString>*>(variant->m_pData) == *static_cast<TArray<ZString>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
			/*else if ((typeName == "TArray<ECameraState>") &&
				*static_cast<TArray<ECameraState>*>(variant->m_pData) == *static_cast<TArray<ECameraState>*>(it->second[i]))
			{
				found = true;
				index = i;

				break;
			}*/
		}

		if (found)
		{
			result = it->second[index];
		}
	}

	return result;
}
