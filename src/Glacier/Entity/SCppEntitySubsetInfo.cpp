#include "Glacier/Entity/SCppEntitySubsetInfo.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SCppEntitySubsetInfo::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("name");
    name.SerializeToJson(writer);

    writer.String("type");
    type->SerializeToJson(writer);

    writer.String("flags");
    writer.Uint(flags);

    writer.EndObject();
}

void SCppEntitySubsetInfo::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int nameOffset = offset + offsetof(SCppEntitySubsetInfo, name);
    unsigned int typeOffset = offset + offsetof(SCppEntitySubsetInfo, type);
    unsigned int flagsOffset = offset + offsetof(SCppEntitySubsetInfo, flags);

    name.SerializeToMemory(binarySerializer, nameOffset);
    type->SerializeToMemory(binarySerializer, typeOffset);

    binarySerializer.WriteToMemory(&flags, sizeof(unsigned int), flagsOffset);
}

SCppEntitySubsetInfo* SCppEntitySubsetInfo::DeserializeFromJson(const rapidjson::Value& object)
{
    SCppEntitySubsetInfo* cppEntitySubsetInfo = new SCppEntitySubsetInfo();

    cppEntitySubsetInfo->name = *ZString::DeserializeFromJson(object["name"].GetString());
    cppEntitySubsetInfo->type = STypeID::DeserializeFromJson(object["type"].GetString());
    cppEntitySubsetInfo->flags = object["flags"].GetUint();

    return cppEntitySubsetInfo;
}
