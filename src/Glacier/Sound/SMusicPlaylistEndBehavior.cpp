#include "Glacier/Sound/SMusicPlaylistEndBehavior.h"

void SMusicPlaylistEndBehavior::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_bGotoPrevious");
    writer.Bool(m_bGotoPrevious);

    writer.String("m_nAttenuationOffset");
    writer.Int(m_nAttenuationOffset);

    writer.EndObject();
}
