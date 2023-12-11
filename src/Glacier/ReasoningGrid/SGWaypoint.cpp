#include "Glacier/ReasoningGrid/SGWaypoint.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SGWaypoint::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("nNeighbor0");
	writer.Int(nNeighbor0);

	writer.String("nNeighbor1");
	writer.Int(nNeighbor1);

	writer.String("nNeighbor2");
	writer.Int(nNeighbor2);

	writer.String("nNeighbor3");
	writer.Int(nNeighbor3);

	writer.String("nNeighbor4");
	writer.Int(nNeighbor4);

	writer.String("nNeighbor5");
	writer.Int(nNeighbor5);

	writer.String("nNeighbor6");
	writer.Int(nNeighbor6);

	writer.String("nNeighbor7");
	writer.Int(nNeighbor7);

	writer.String("vPos");
	vPos.SerializeToJson(writer);

	writer.EndObject();
}

void SGWaypoint::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int neighbor0Offset = offset + offsetof(SGWaypoint, nNeighbor0);
	unsigned int neighbor1Offset = offset + offsetof(SGWaypoint, nNeighbor1);
	unsigned int neighbor2Offset = offset + offsetof(SGWaypoint, nNeighbor2);
	unsigned int neighbor3Offset = offset + offsetof(SGWaypoint, nNeighbor3);
	unsigned int neighbor4Offset = offset + offsetof(SGWaypoint, nNeighbor4);
	unsigned int neighbor5Offset = offset + offsetof(SGWaypoint, nNeighbor5);
	unsigned int neighbor6Offset = offset + offsetof(SGWaypoint, nNeighbor6);
	unsigned int neighbor7Offset = offset + offsetof(SGWaypoint, nNeighbor7);
	unsigned int posOffset = offset + offsetof(SGWaypoint, vPos);

	binarySerializer.WriteToMemory(&nNeighbor0, sizeof(short), neighbor0Offset);
	binarySerializer.WriteToMemory(&nNeighbor1, sizeof(short), neighbor1Offset);
	binarySerializer.WriteToMemory(&nNeighbor2, sizeof(short), neighbor2Offset);
	binarySerializer.WriteToMemory(&nNeighbor3, sizeof(short), neighbor3Offset);
	binarySerializer.WriteToMemory(&nNeighbor4, sizeof(short), neighbor4Offset);
	binarySerializer.WriteToMemory(&nNeighbor5, sizeof(short), neighbor5Offset);
	binarySerializer.WriteToMemory(&nNeighbor6, sizeof(short), neighbor6Offset);
	binarySerializer.WriteToMemory(&nNeighbor7, sizeof(short), neighbor7Offset);
	vPos.SerializeToMemory(binarySerializer, posOffset);
}

void SGWaypoint::DeserializeFromJson(SGWaypoint& gWaypoint, const rapidjson::Value& object)
{
	gWaypoint.nNeighbor0 = object["nNeighbor0"].GetInt();
	gWaypoint.nNeighbor1 = object["nNeighbor1"].GetInt();
	gWaypoint.nNeighbor2 = object["nNeighbor2"].GetInt();
	gWaypoint.nNeighbor3 = object["nNeighbor3"].GetInt();
	gWaypoint.nNeighbor4 = object["nNeighbor4"].GetInt();
	gWaypoint.nNeighbor5 = object["nNeighbor5"].GetInt();
	gWaypoint.nNeighbor6 = object["nNeighbor6"].GetInt();
	gWaypoint.nNeighbor7 = object["nNeighbor7"].GetInt();
	float4::DeserializeFromJson(gWaypoint.vPos, object["vPos"].GetObj());
}
