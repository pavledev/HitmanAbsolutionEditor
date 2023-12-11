#include "Glacier/Entity/SEntityTemplatePinConnection.h"
#include "Glacier/Serializer/ZBinarySerializer.h"

void SEntityTemplatePinConnection::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("fromID");
    writer.Int(fromID);

    writer.String("toID");
    writer.Int(toID);

    writer.String("fromPinName");
    fromPinName.SerializeToJson(writer);

    writer.String("toPinName");
    toPinName.SerializeToJson(writer);

    writer.EndObject();
}

void SEntityTemplatePinConnection::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
    unsigned int fromIDOffset = offset + offsetof(SEntityTemplatePinConnection, fromID);
    unsigned int toIDOffset = offset + offsetof(SEntityTemplatePinConnection, toID);
    unsigned int fromPinNameOffset = offset + offsetof(SEntityTemplatePinConnection, fromPinName);
    unsigned int toPinNameOffset = offset + offsetof(SEntityTemplatePinConnection, toPinName);

    binarySerializer.WriteToMemory(&fromID, sizeof(int), fromIDOffset);
    binarySerializer.WriteToMemory(&toID, sizeof(int), toIDOffset);

    fromPinName.SerializeToMemory(binarySerializer, fromPinNameOffset);
    toPinName.SerializeToMemory(binarySerializer, toPinNameOffset);
}

SEntityTemplatePinConnection* SEntityTemplatePinConnection::DeserializeFromJson(const rapidjson::Value& object)
{
    SEntityTemplatePinConnection* entityTemplatePinConnection = new SEntityTemplatePinConnection();

    entityTemplatePinConnection->fromID = object["fromID"].GetInt();
    entityTemplatePinConnection->toID = object["toID"].GetInt();
    entityTemplatePinConnection->fromPinName = *ZString::DeserializeFromJson(object["fromPinName"].GetString());

    return entityTemplatePinConnection;
}
