#include "Glacier/Sound/SMusicTimeSignature.h"

void SMusicTimeSignature::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_nBeatsPerMinute");
    writer.Int(m_nBeatsPerMinute);

    writer.String("m_nBeatsPerBar");
    writer.Int(m_nBeatsPerBar);

    writer.EndObject();
}
