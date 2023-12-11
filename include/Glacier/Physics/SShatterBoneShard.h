#pragma once

#include <vector>

#include "ZCollisionShape.h"

struct SShatterBoneShard
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZCollisionShape m_pShape;
	bool m_bNoCollision;
	bool m_bRemain;
	unsigned int m_nGlobalBoneID;
	unsigned int m_nShardIndex;
	unsigned int m_nParentIndex;
	std::vector<unsigned int> m_aChildIndices;
	std::vector<unsigned int> m_aConnections;
};
