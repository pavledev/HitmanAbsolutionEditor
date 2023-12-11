#include "Glacier/PackageList/SPackageListDataSection.h"

void SPackageListDataSection::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("sceneResourceIDs");
    sceneResourceIDs.SerializeToJson(writer);

    writer.String("resources");
    resources.SerializeToJson(writer);

    writer.EndObject();
}
