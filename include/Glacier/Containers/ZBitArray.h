#pragma once

#include "TArray.h"

class ZBitArray
{
public:
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static void DeserializeFromJson(ZBitArray& bitArray,const rapidjson::Value& object);

private:
	TArray<unsigned char> m_aBytes;
	unsigned int m_nSize;
};
