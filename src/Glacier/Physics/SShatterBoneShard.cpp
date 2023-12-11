#include "Glacier/Physics/SShatterBoneShard.h"

void SShatterBoneShard::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_pShape");
	m_pShape.SerializeToJson(writer);

	writer.String("m_bNoCollision");
	writer.Bool(m_bNoCollision);

	writer.String("m_bRemain");
	writer.Bool(m_bRemain);

	writer.String("m_nGlobalBoneID");
	writer.Uint(m_nGlobalBoneID);

	writer.String("m_nShardIndex");
	writer.Uint(m_nShardIndex);

	writer.String("m_nParentIndex");
	writer.Uint(m_nParentIndex);

	writer.String("m_aChildIndices");
	writer.StartArray();

	for (size_t i = 0; i < m_aChildIndices.size(); ++i)
	{
		writer.Uint(m_aChildIndices[i]);
	}

	writer.EndArray();

	writer.String("m_pNxShapeList");
	writer.StartArray();

	for (size_t i = 0; i < m_aConnections.size(); ++i)
	{
		writer.Uint(m_aConnections[i]);
	}

	writer.EndArray();

	writer.EndObject();
}
