#include "Glacier/Sound/SMusicGameCueData.h"

void SMusicGameCueData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_sName");
    m_sName.SerializeToJson(writer);

    writer.String("m_bIsStinger");
    writer.Bool(m_bIsStinger);

    writer.String("m_nTargetIndex");
    writer.Int(m_nTargetIndex);

    writer.EndObject();
}
