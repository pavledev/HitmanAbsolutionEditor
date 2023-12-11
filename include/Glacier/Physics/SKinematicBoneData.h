#pragma once

#include "ZCollisionShape.h"

struct SKinematicBoneData
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned int m_nGlobalBoneId;
	unsigned int m_nParentBoneId;
	ZCollisionShape m_pShape;
};
