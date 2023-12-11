#include <Windows.h>
#include <format>

#include "Glacier/Entity/SEntityTemplateProperty.h"
#include "Glacier/Serializer/ZBinarySerializer.h"
#include "Registry/PropertyRegistry.h"

void SEntityTemplateProperty::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("nPropertyID");
    writer.Uint(nPropertyID);

    writer.String("propertyName");
    writer.String(PropertyRegistry::GetInstance().GetPropertyName(nPropertyID).c_str());

    writer.String("value");
    value.SerializeToJson(writer);

    writer.EndObject();
}

void SEntityTemplateProperty::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int nPropertyIDOffset = offset + offsetof(SEntityTemplateProperty, nPropertyID);
    unsigned int valueOffset = offset + offsetof(SEntityTemplateProperty, value);

    binarySerializer.WriteToMemory(&nPropertyID, sizeof(unsigned int), nPropertyIDOffset);

    value.SerializeToMemory(binarySerializer, valueOffset);
}

SEntityTemplateProperty* SEntityTemplateProperty::DeserializeFromJson(const rapidjson::Value& object)
{
    SEntityTemplateProperty* entityTemplateProperty = new SEntityTemplateProperty();

    entityTemplateProperty->nPropertyID = object["nPropertyID"].GetUint();
    entityTemplateProperty->value = *ZVariant::DeserializeFromJson(object["value"].GetObj());

    return entityTemplateProperty;
}
