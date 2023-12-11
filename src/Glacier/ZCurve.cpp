#include "Glacier/ZCurve.h"

void ZCurve::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("data");
	data.SerializeToJson(writer);

	writer.EndObject();
}

void ZCurve::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int dataOffset = offset + offsetof(ZCurve, data);

	data.SerializeToMemory(binarySerializer, dataOffset);
}

ZCurve* ZCurve::DeserializeFromJson(const rapidjson::Value& object)
{
	ZCurve* curve = new ZCurve();

	curve->data = *TArray<TFixedArray<float, 8>>::DeserializeFromJson(object["data"].GetArray());

	return curve;
}

bool ZCurve::operator==(const ZCurve& other) const
{
	return data == other.data;
}

const TArray<TFixedArray<float, 8>>& ZCurve::GetData() const
{
	return data;
}

TArray<TFixedArray<float, 8>>& ZCurve::GetData()
{
	return data;
}
