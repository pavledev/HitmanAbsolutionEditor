#include "Glacier/PackageList/SPackageListDataFolder.h"

void SPackageListDataFolder::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("name");
    name.SerializeToJson(writer);

    writer.String("sections");
    sections.SerializeToJson(writer);

    writer.EndObject();
}
