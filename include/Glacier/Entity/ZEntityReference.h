#pragma once

#include "ZEntityID.h"

class ZEntityReference
{
public:
	ZEntityReference() = default;
	ZEntityReference(const ZEntityID& entityID, const ZString& exposedEntity);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

private:
	ZEntityID m_EntityID;
	ZString m_sExposedEntity;
};
