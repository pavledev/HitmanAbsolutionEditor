#include "Glacier/Sound/SMusicPlaylistData.h"

void SMusicPlaylistData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_sName");
    m_sName.SerializeToJson(writer);

    writer.String("m_aNodes");
    m_aNodes.SerializeToJson(writer);

    writer.String("m_aBursts");
    m_aBursts.SerializeToJson(writer);

    writer.String("m_bPlayBursts");
    writer.Bool(m_bPlayBursts);

    writer.String("m_fMinBurstDelay");
    writer.Double(m_fMinBurstDelay);

    writer.String("m_fMaxBurstDelay");
    writer.Double(m_fMaxBurstDelay);

    writer.EndObject();
}
