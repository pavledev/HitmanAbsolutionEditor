#include "Glacier/Math/SVector4.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

SVector4::SVector4()
{
}

SVector4::SVector4(const float x, const float y, const float z, const float w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

const float& SVector4::operator[](const unsigned int index) const
{
	return v[index];
}

float& SVector4::operator[](const unsigned int index)
{
	return v[index];
}

SVector4 SVector4::operator*(const float value) const
{
	return SVector4(x * value, y * value, z * value, w * value);
}

void SVector4::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("x");
	writer.Double(x);

	writer.String("y");
	writer.Double(y);

	writer.String("z");
	writer.Double(z);

	writer.String("w");
	writer.Double(w);

	writer.EndObject();
}

void SVector4::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int xOffset = offset + offsetof(SVector4, x);
	unsigned int yOffset = offset + offsetof(SVector4, y);
	unsigned int zOffset = offset + offsetof(SVector4, z);
	unsigned int wOffset = offset + offsetof(SVector4, w);

	binarySerializer.WriteToMemory(&x, sizeof(float), xOffset);
	binarySerializer.WriteToMemory(&y, sizeof(float), yOffset);
	binarySerializer.WriteToMemory(&z, sizeof(float), zOffset);
	binarySerializer.WriteToMemory(&w, sizeof(float), wOffset);
}

SVector4* SVector4::DeserializeFromJson(const rapidjson::Value& object)
{
	SVector4* vector = new SVector4();

	vector->x = object["x"].GetFloat();
	vector->y = object["y"].GetFloat();
	vector->z = object["z"].GetFloat();
	vector->w = object["w"].GetFloat();

	return vector;
}
