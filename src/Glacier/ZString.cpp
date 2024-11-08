#include "Glacier/ZString.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

ZString::ZString()
{
	char* chars = new char[1];

	strncpy_s(chars, 1, "", 1);

	m_length = 0x80000000;
	m_chars = chars;
}

ZString::ZString(const char* rhs)
{
	const size_t length = strlen(rhs);
	char* chars = new char[length + 1];

	strncpy_s(chars, length + 1, rhs, length + 1);

	m_length = length | 0x80000000;
	m_chars = chars;
}

ZString::ZString(const ZString& other)
{
	const size_t length = strlen(other.m_chars);
	char* chars = new char[length + 1];

	strncpy_s(chars, length + 1, other.m_chars, length + 1);

	m_length = length | 0x80000000;
	m_chars = chars;
}

ZString& ZString::operator=(const ZString& other)
{
	if (this != &other)
	{
		if (m_chars)
		{
			delete[] m_chars;
		}

		const size_t length = strlen(other.m_chars);
		char* chars = new char[length + 1];

		strncpy_s(chars, length + 1, other.m_chars, length + 1);

		m_length = length | 0x80000000;
		m_chars = chars;
	}

	return *this;
}

ZString::~ZString()
{
	if (m_chars)
	{
		delete[] m_chars;

		m_chars = nullptr;
	}
}

const unsigned int ZString::Length() const
{
	return m_length & 0x3FFFFFFF;
}

const char* ZString::ToCString() const
{
	return m_chars;
}

void ZString::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.String(m_chars);
}

void ZString::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	binarySerializer.SetLayoutPointer(binarySerializer.GetAlignedLayoutPointer(binarySerializer.GetLayoutPointer(), alignof(ZString)));

	unsigned int length = Length() + 1;

	unsigned int lengthOffset = offset + offsetof(ZString, m_length);
	unsigned int charsOffset = offset + offsetof(ZString, m_chars);
	bool useMaxAlignment = true;

	if (binarySerializer.GetMaxAlignment() >= 8)
	{
		useMaxAlignment = false;
	}

	unsigned int charsOffset2 = binarySerializer.ReserveLayoutFor(length, sizeof(char), 1, 4, useMaxAlignment);

	binarySerializer.WriteToMemory(&length, sizeof(length), charsOffset2 - 4);
	binarySerializer.WriteToMemory(m_chars, length, charsOffset2);

	unsigned int length2 = Length() | 0x40000000;

	binarySerializer.WriteToMemory(&length2, sizeof(unsigned int), lengthOffset);
	binarySerializer.WriteToMemory(&charsOffset2, sizeof(unsigned int), charsOffset);

	binarySerializer.RecordOffsetForRebasing(charsOffset);
}

ZString* ZString::DeserializeFromJson(const char* chars)
{
	return new ZString(chars);
}

bool ZString::operator==(const ZString& other) const
{
	return Length() == other.Length() && strcmp(m_chars, other.m_chars) == 0;
}
