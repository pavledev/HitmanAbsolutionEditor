#pragma once

#include <string>

#include "IType.h"

class ZBinarySerializer;

struct STypeID
{
	STypeID();
	~STypeID();
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static STypeID* DeserializeFromJson(const char* typeName);

	unsigned short flags;
	unsigned short typeNum;
	IType* pTypeInfo;
	STypeID* pSourceType;
};
