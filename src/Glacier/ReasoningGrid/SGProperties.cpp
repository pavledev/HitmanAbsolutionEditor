#include "Glacier/ReasoningGrid/SGProperties.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SGProperties::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("vMin");
	vMin.SerializeToJson(writer);

	writer.String("vMax");
	vMax.SerializeToJson(writer);

	writer.String("nGridWidth");
	writer.Int(nGridWidth);

	writer.String("fGridSpacing");
	writer.Double(fGridSpacing);

	writer.EndObject();
}

void SGProperties::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int minOffset = offset + offsetof(SGProperties, vMin);
	unsigned int maxOffset = offset + offsetof(SGProperties, vMax);
	unsigned int gridWidthOffset = offset + offsetof(SGProperties, nGridWidth);
	unsigned int gridSpacingOffset = offset + offsetof(SGProperties, fGridSpacing);

	vMin.SerializeToMemory(binarySerializer, minOffset);
	vMax.SerializeToMemory(binarySerializer, maxOffset);
	binarySerializer.WriteToMemory(&nGridWidth, sizeof(int), gridWidthOffset);
	binarySerializer.WriteToMemory(&fGridSpacing, sizeof(float), gridSpacingOffset);
}

void SGProperties::DeserializeFromJson(SGProperties& gProperties, const rapidjson::Value& object)
{
	float4::DeserializeFromJson(gProperties.vMin, object["vMin"].GetObj());
	float4::DeserializeFromJson(gProperties.vMax, object["vMax"].GetObj());
	gProperties.nGridWidth = object["nGridWidth"].GetInt();
	gProperties.fGridSpacing = object["fGridSpacing"].GetFloat();
}
