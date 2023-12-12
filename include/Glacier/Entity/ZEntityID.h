#pragma once

#include "../ZString.h"

class ZEntityID
{
public:
	ZEntityID() = default;
	ZEntityID(const ZString& str);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

private:
	ZString m_sStr;
};
