#include "Glacier/Sound/SMusicCompositionData.h"

void SMusicCompositionData::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("m_aGameCues");
    m_aGameCues.SerializeToJson(writer);

    writer.String("m_aPlaylists");
    m_aPlaylists.SerializeToJson(writer);

    writer.String("m_aStingers");
    m_aStingers.SerializeToJson(writer);

    writer.String("m_aSegments");
    m_aSegments.SerializeToJson(writer);

    writer.String("m_aEndBehaviors");
    m_aEndBehaviors.SerializeToJson(writer);

    writer.String("m_transitionMap");
    m_transitionMap.SerializeToJson(writer);

    writer.String("m_nJumpCue");
    writer.Int(m_nJumpCue);

    writer.String("m_aTrackExposingGroups");
    m_aTrackExposingGroups.SerializeToJson(writer);

    writer.EndObject();

    std::ofstream outputFileStream = std::ofstream(outputFilePath);

    outputFileStream << stringBuffer.GetString();

    outputFileStream.close();
}
