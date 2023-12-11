#pragma once

#include "rapidjson/prettywriter.h"

struct SPoseBoneHeader
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned int m_nPoseBoneArrayOffset;
	unsigned int m_nPoseBoneIndeArrayOffset;
	int m_nPoseBoneCountTotal;
	unsigned int m_nPoseEntryIndexArrayOffset;
	unsigned int m_nPoseBoneCountArrayOffset;
	int m_nPoseCount;
	unsigned int m_nNamesListOffset;
	unsigned int m_nNamesEntryIndexArrayOffset;
	unsigned int m_nFaceBoneIndexArrayOffset;
	int m_nFaceBoneCount;
};
