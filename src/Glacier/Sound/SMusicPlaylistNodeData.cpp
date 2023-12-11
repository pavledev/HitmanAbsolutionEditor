#include "Glacier/Sound/SMusicPlaylistNodeData.h"

void SMusicPlaylistNodeData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
    writer.StartObject();

    writer.String("m_nSegmentIndex");
    writer.Int(m_nSegmentIndex);

    writer.String("m_sName");
    m_sName.SerializeToJson(writer);

    writer.String("m_nParentIndex");
    writer.Int(m_nParentIndex);

    writer.String("m_aChildIndices");
    m_aChildIndices.SerializeToJson(writer);

    writer.String("m_ePlayType");
    writer.Int(m_ePlayType);

    writer.String("m_nWeight");
    writer.Int(m_nWeight);

    writer.String("m_nLoopCount");
    writer.Int(m_nLoopCount);

    writer.String("m_NoRepeatCount");
    writer.Int(m_NoRepeatCount);

    writer.String("m_nSelectProbability");
    writer.Int(m_nSelectProbability);

    writer.String("m_nMaxLoopCount");
    writer.Int(m_nMaxLoopCount);

    writer.String("m_nRealLoopCount");
    writer.Int(m_nRealLoopCount);

    writer.EndObject();
}
