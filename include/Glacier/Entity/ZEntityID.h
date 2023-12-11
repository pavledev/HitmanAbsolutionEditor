#pragma once

#include "../ZString.h"

class ZEntityID
{
public:
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static ZEntityID* DeserializeFromJson(const rapidjson::Value& object);

private:
	ZString m_sStr;
};
