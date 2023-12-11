#include "Glacier/Sound/SMusicTrackExposingGroupData.h"

void SMusicTrackExposingGroupData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_sName");
    m_sName.SerializeToJson(writer);

    writer.String("m_trackNames");
    m_trackNames.SerializeToJson(writer);

    writer.String("m_trackTypes");
    m_trackTypes.SerializeToJson(writer);

    writer.EndObject();
}
