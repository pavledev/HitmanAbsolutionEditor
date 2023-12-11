#include "Glacier/Sound/SMusicTrackVariationData.h"

void SMusicTrackVariationData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_nWeight");
    writer.Int(m_nWeight);

    writer.String("m_nWaveIndex");
    writer.Int(m_nWaveIndex);

    writer.EndObject();
}
