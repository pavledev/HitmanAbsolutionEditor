#include "Glacier/Containers/ZBitArray.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void ZBitArray::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_aBytes");
	m_aBytes.SerializeToJson(writer);

	writer.String("m_nSize");
	writer.Uint(m_nSize);

	writer.EndObject();
}

void ZBitArray::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int bytesOffset = offset + offsetof(ZBitArray, m_aBytes);
	unsigned int sizeOffset = offset + offsetof(ZBitArray, m_nSize);

	m_aBytes.SerializeToMemory(binarySerializer, bytesOffset);
	binarySerializer.WriteToMemory(&m_nSize, sizeof(unsigned int), sizeOffset);
}

void ZBitArray::DeserializeFromJson(ZBitArray& bitArray, const rapidjson::Value& object)
{
	TArray<unsigned char>::DeserializeFromJson(bitArray.m_aBytes, object["m_aBytes"].GetArray());
	bitArray.m_nSize = object["m_nSize"].GetUint();
}
