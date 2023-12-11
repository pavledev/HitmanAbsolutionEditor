#pragma once

#include "ZEntityID.h"

class ZEntityReference
{
public:
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static ZEntityReference* DeserializeFromJson(const rapidjson::Value& object);
	bool operator==(const ZEntityReference& other);

private:
	ZEntityID m_EntityID;
	ZString m_sExposedEntity;
};
