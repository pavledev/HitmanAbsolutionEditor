#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

struct SScatterPackedMaterialBase
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	unsigned int m_nInstances;
	unsigned int m_bBend;
	float m_fBendConstraint;
	float m_fCutoffDistance;
	float m_fScaleBeginDistance;
};
