#include "Glacier/PackageList/SPackageListData.h"

std::string SPackageListData::SerializeToJson()
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("folders");
    folders.SerializeToJson(writer);

    writer.EndObject();

    return stringBuffer.GetString();
}
