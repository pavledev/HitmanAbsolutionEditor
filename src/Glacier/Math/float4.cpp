#include "Glacier/Math/float4.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void float4::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("x");
	writer.Double(m.m128_f32[0]);

	writer.String("y");
	writer.Double(m.m128_f32[1]);

	writer.String("z");
	writer.Double(m.m128_f32[2]);

	writer.String("w");
	writer.Double(m.m128_f32[3]);

	writer.EndObject();
}

void float4::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int mOffset = offset + offsetof(float4, m);

	binarySerializer.WriteToMemory(&m, sizeof(__m128), mOffset);
}

void float4::DeserializeFromJson(float4& float4, const rapidjson::Value& object)
{
	float4.m.m128_f32[0] = object["x"].GetFloat();
	float4.m.m128_f32[1] = object["y"].GetFloat();
	float4.m.m128_f32[2] = object["z"].GetFloat();
	float4.m.m128_f32[3] = object["w"].GetFloat();
}
