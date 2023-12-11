#pragma once

#include "../Math/SVector3.h"

struct SBoneDefinition
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	SVector3 Center;
	unsigned int lPrevBoneNr;
	SVector3 Size;
	char Name[34];
	unsigned char nBodyPart;
};
