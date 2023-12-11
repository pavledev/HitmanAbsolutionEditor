#pragma once

#include <unordered_map>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"

#include "ZObjectRef.h"

class ZBinarySerializer;

class ZVariant : public ZObjectRef
{
public:
	ZVariant();
	ZVariant(const ZVariant& other);
	~ZVariant();
	void Allocate(STypeID* type);
	void Set(STypeID* type, const void* pData);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	bool SerializeData(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static ZVariant* DeserializeFromJson(const rapidjson::Value& object);
	static void* GetValue(const ZVariant* variant, const std::unordered_map<STypeID*, std::vector<void*>>& values);

	template <typename T>
	T& Get()
	{
		return *static_cast<T*>(m_pData);
	}
};
