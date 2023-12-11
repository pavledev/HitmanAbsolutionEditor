#include <Windows.h>
#include <format>

#include "Glacier/TypeInfo/STypeID.h"
#include "Glacier/Serializer/ZBinarySerializer.h"
#include "Registry/TypeRegistry.h"

STypeID::STypeID()
{
	flags = 0;
	typeNum = 0;
	pTypeInfo = nullptr;
	pSourceType = nullptr;
}

STypeID::~STypeID()
{
	delete pTypeInfo;
	delete pSourceType;
}

void STypeID::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.String(pTypeInfo->GetTypeName());
}

void STypeID::SerializeToMemory(ZBinarySerializer& binarySerializer, unsigned int offset)
{
	unsigned int typeIndex = binarySerializer.TypeIdToIndex(this);

	binarySerializer.WriteToMemory(&typeIndex, sizeof(unsigned int), offset);

	binarySerializer.RecordOffsetForTypeIDReindexing(offset);
}

STypeID* STypeID::DeserializeFromJson(const char* typeName)
{
	return TypeRegistry::GetInstance().GetTypeID(typeName);
}
