#include "Glacier/Math/SVector3.h"
#include "Glacier/Math/SVector4.h"
#include "Glacier/Math/float4.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

#include "Utility/Math.h"
#include "Math/Vector3.h"

SVector3::SVector3()
{
    x = 0;
    y = 0;
    z = 0;
}

SVector3::SVector3(const float x, const float y, const float z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

SVector3::SVector3(const float4& vector)
{
    this->x = vector.m.m128_f32[0];
    this->y = vector.m.m128_f32[1];
    this->z = vector.m.m128_f32[2];
}

SVector3::SVector3(const SVector4& vector)
{
    x = vector.x;
    y = vector.y;
    z = vector.z;
}

SVector3::SVector3(const Vector3& other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}

void SVector3::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("x");
    writer.Double(x);

    writer.String("y");
    writer.Double(y);

    writer.String("z");
    writer.Double(z);

    writer.EndObject();
}

void SVector3::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int xOffset = offset + offsetof(SVector3, x);
    unsigned int yOffset = offset + offsetof(SVector3, y);
    unsigned int zOffset = offset + offsetof(SVector3, z);

    binarySerializer.WriteToMemory(&x, sizeof(float), xOffset);
    binarySerializer.WriteToMemory(&y, sizeof(float), yOffset);
    binarySerializer.WriteToMemory(&z, sizeof(float), zOffset);
}

SVector3* SVector3::DeserializeFromJson(const rapidjson::Value& object)
{
    SVector3* vector = new SVector3();

    vector->x = object["x"].GetFloat();
    vector->y = object["y"].GetFloat();
    vector->z = object["z"].GetFloat();

    return vector;
}

bool SVector3::operator==(const SVector3& other) const
{
    return x == other.x && y == other.y && z == other.z;
}

bool SVector3::operator>(const SVector3& other) const
{
    return x > other.x && y > other.y && z > other.z;
}

bool SVector3::operator<(const SVector3& other) const
{
    return x < other.x&& y < other.y&& z < other.z;
}

SVector3& SVector3::operator*=(const float value)
{
    x *= value;
    y *= value;
    z *= value;

    return *this;
}

SVector3 SVector3::operator*(const float value) const
{
    return SVector3(x * value, y * value, z * value);
}

SVector3& SVector3::operator+=(const SVector3& other)
{
    x += other.x;
    y += other.y;
    z += other.z;

    return *this;
}

SVector3 SVector3::operator+(const SVector3& other) const
{
    return SVector3(x + other.x, y + other.y, z + other.z);
}

SVector3 SVector3::operator-(const SVector3& other) const
{
    return SVector3(x - other.x, y - other.y, z - other.z);
}

SVector3 SVector3::operator-(const float value) const
{
    return SVector3(x - value, y - value, z - value);
}

SVector3& SVector3::operator-=(const SVector3& other)
{
    x -= other.x;
    y -= other.y;
    z -= other.z;

    return *this;
}

const float& SVector3::operator[](const unsigned int index) const
{
    return v[index];
}

float& SVector3::operator[](const unsigned int index)
{
    return v[index];
}

SVector3 SVector3::Normalize()
{
    float length = 0.f;

    return Normalize(length);
}

SVector3 SVector3::Normalize(float& length)
{
    SVector3& vector = *this;
    float squaredLength = Dot(vector, vector);

    /* A larger value causes normalize errors in a scaled down models with camera extreme close. */
    if (squaredLength > 1.0e-35f)
    {
        length = sqrt(squaredLength);
        vector *= 1.0f / length;
    }
    else
    {
        /* Either the vector is small or one of it's values contained `nan`. */
        length = 0.f;
    }

    return vector;
}

SVector3 SVector3::Normalize(const SVector3& v)
{
    float length;

    return Normalize(v, length);
}

void SVector3::Normalize(SVector3& vector)
{
    float length;

    Normalize(vector, length);
}

SVector3 SVector3::Normalize(const SVector3& v, float& length)
{
    SVector3 result = v;

    Normalize(result, length);

    return result;
}

void SVector3::Normalize(SVector3& vector, float& length)
{
    float squaredLength = Dot(vector, vector);

    /* A larger value causes normalize errors in a scaled down models with camera extreme close. */
    if (squaredLength > 1.0e-35f)
    {
        length = sqrt(squaredLength);

        vector *= 1.0f / length;
    }
}

bool SVector3::IsNormalized() const
{
    const float testUnit = SquaredLength();

    return fabs(testUnit - 1.0f) < UNIT_EPSILON || fabs(testUnit) < UNIT_EPSILON;
}

float SVector3::Dot(const SVector3& v1, const SVector3& v2)
{
    return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
}

float SVector3::Dot(const SVector3& rhs) const
{
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

SVector3 SVector3::Cross(const SVector3& vector, const SVector3& vector2)
{
    SVector3 result;

    result.x = vector.y * vector2.z - vector.z * vector2.y;
    result.y = vector.z * vector2.x - vector.x * vector2.z;
    result.z = vector.x * vector2.y - vector.y * vector2.x;

    return result;
}

SVector3 SVector3::Cross(const SVector3& vector) const
{
    return Cross(*this, vector);
}

float SVector3::Length() const
{
    return Math::Sqrt(x * x + y * y + z * z);
}

float SVector3::Length(const SVector3& vector, const SVector3& vector2)
{
    return (vector2 - vector).Length();
}

float SVector3::SquaredLength() const
{
    return x * x + y * y + z * z;
}

SVector3 SVector3::Negate(const SVector3& vector)
{
    return SVector3(-vector.x, -vector.y, -vector.z);
}

void SVector3::Negate()
{
    x = -x;
    y = -y;
    z = -z;
}

float SVector3::NormalizedAngle(const SVector3& vector, const SVector3& vector2)
{
    /* double check they are normalized */
    assert(vector.IsNormalized());
    assert(vector2.IsNormalized());

    /* this is the same as acos(Dot(v1, v2)), but more accurate */
    if (Dot(vector, vector2) >= 0.0f)
    {
        return 2.0f * Math::ASin(Length(vector, vector2) / 2.0f);
    }

    return static_cast<float>(M_PI) - 2.0f * Math::ASin(Length(vector, Negate(vector2)) / 2.0f);
}

SVector3 SVector3::Ortho(const SVector3& vector)
{
    SVector3 result = {};
    const int axis = DominantAxis(vector);

    assert(result != vector);

    switch (axis)
    {
        case 0:
            result.x = -vector.y - vector.z;
            result.y = vector.x;
            result.z = vector.x;
            break;
        case 1:
            result.x = vector.y;
            result.y = -vector.x - vector.z;
            result.z = vector.y;
            break;
        case 2:
            result.x = vector.z;
            result.y = vector.z;
            result.z = -vector.x - vector.y;
            break;
    }

    return result;
}

int SVector3::DominantAxis(const SVector3& vector)
{
    const float x = fabsf(vector.x);
    const float y = fabsf(vector.y);
    const float z = fabsf(vector.z);

    return ((x > y) ? ((x > z) ? 0 : 2) : ((y > z) ? 1 : 2));
}

SVector3 SVector3::ProjectNormalized(const SVector3& p, const SVector3& proj)
{
    return proj * Dot(p, proj);
}

float SVector3::Angle(const SVector3& vector, const SVector3& vector2)
{
    const Vector3 normalizedVector = Vector3::Normalize(vector);
    const Vector3 normalizedVector2 = Vector3::Normalize(vector2);

    return NormalizedAngle(normalizedVector, normalizedVector2);
}
