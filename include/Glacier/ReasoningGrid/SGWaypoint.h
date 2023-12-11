#pragma once

#include "../Math/float4.h"

struct SGWaypoint
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	void SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset);
	static void DeserializeFromJson(SGWaypoint& gWaypoint, const rapidjson::Value& object);

	union
	{
		struct
		{
			short nNeighbor0;
			short nNeighbor1;
			short nNeighbor2;
			short nNeighbor3;
			short nNeighbor4;
			short nNeighbor5;
			short nNeighbor6;
			short nNeighbor7;
		};

		short Neighbors[8];
	};

	float4 vPos;
};
