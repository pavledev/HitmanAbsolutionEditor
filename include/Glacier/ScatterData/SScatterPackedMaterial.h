#pragma once

#include <vector>

#include "../Resource/ZRuntimeResourceID.h"
#include "SScatterPackedMaterialBase.h"
#include "SScatterPackedInstance.h"

struct SScatterPackedMaterial
{
	void SerializeToJson(std::vector<SScatterPackedInstance>& instances, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZRuntimeResourceID m_idResource;
	SScatterPackedMaterialBase m_Base;
};
