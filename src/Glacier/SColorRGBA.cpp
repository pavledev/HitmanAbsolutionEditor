#include "Glacier/SColorRGBA.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SColorRGBA::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("r");
	writer.Double(r);

	writer.String("g");
	writer.Double(g);

	writer.String("b");
	writer.Double(b);

	writer.String("a");
	writer.Double(a);

	writer.EndObject();
}

void SColorRGBA::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int rOffset = offset + offsetof(SColorRGBA, r);
	unsigned int gOffset = offset + offsetof(SColorRGBA, g);
	unsigned int bOffset = offset + offsetof(SColorRGBA, b);
	unsigned int aOffset = offset + offsetof(SColorRGBA, a);

	binarySerializer.WriteToMemory(&r, sizeof(float), rOffset);
	binarySerializer.WriteToMemory(&g, sizeof(float), gOffset);
	binarySerializer.WriteToMemory(&b, sizeof(float), bOffset);
	binarySerializer.WriteToMemory(&a, sizeof(float), aOffset);
}

SColorRGBA* SColorRGBA::DeserializeFromJson(const rapidjson::Value& object)
{
	SColorRGBA* colorRGBA = new SColorRGBA();

	colorRGBA->r = object["r"].GetFloat();
	colorRGBA->g = object["g"].GetFloat();
	colorRGBA->b = object["b"].GetFloat();
	colorRGBA->a = object["a"].GetFloat();

	return colorRGBA;
}

bool SColorRGBA::operator==(const SColorRGBA& other) const
{
	return r == other.r && g == other.g && b == other.b && a == other.a;
}
