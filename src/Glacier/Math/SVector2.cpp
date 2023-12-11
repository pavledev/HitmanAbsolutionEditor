#include "Glacier/Math/SVector2.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

SVector2::SVector2(const float x, const float y)
{
    this->x = x;
    this->y = y;
}

void SVector2::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("x");
    writer.Double(x);

    writer.String("y");
    writer.Double(y);

    writer.EndObject();
}

void SVector2::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int xOffset = offset + offsetof(SVector2, x);
    unsigned int yOffset = offset + offsetof(SVector2, y);

    binarySerializer.WriteToMemory(&x, sizeof(float), xOffset);
    binarySerializer.WriteToMemory(&y, sizeof(float), yOffset);
}

SVector2* SVector2::DeserializeFromJson(const rapidjson::Value& object)
{
    SVector2* vector = new SVector2();

    vector->x = object["x"].GetFloat();
    vector->y = object["y"].GetFloat();

    return vector;
}

bool SVector2::operator==(const SVector2& other) const
{
    return x == other.x && y == other.y;
}
