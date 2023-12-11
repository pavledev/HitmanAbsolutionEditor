#pragma once

#include <cstdint>
#include <string.h>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

class ZBinarySerializer;

class ZString
{
public:
	ZString();
	ZString(const char* rhs);
	ZString(const ZString& other);
	ZString& operator=(const ZString& other);
	~ZString();
	const unsigned int Length() const;
	const char* ToCString() const;
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static ZString* DeserializeFromJson(const char* chars);
	bool operator==(const ZString& other) const;

private:
	unsigned int m_length;
	const char* m_chars;
};
