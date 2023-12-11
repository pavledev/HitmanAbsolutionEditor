#include "Glacier/BoneRig/SPoseBoneHeader.h"

void SPoseBoneHeader::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_nPoseBoneArrayOffset");
	writer.Uint(m_nPoseBoneArrayOffset);

	writer.String("m_nPoseBoneIndeArrayOffset");
	writer.Uint(m_nPoseBoneIndeArrayOffset);

	writer.String("m_nPoseBoneCountTotal");
	writer.Int(m_nPoseBoneCountTotal);

	writer.String("m_nPoseEntryIndexArrayOffset");
	writer.Uint(m_nPoseEntryIndexArrayOffset);

	writer.String("m_nPoseBoneCountArrayOffset");
	writer.Uint(m_nPoseBoneCountArrayOffset);

	writer.String("m_nPoseCount");
	writer.Int(m_nPoseCount);

	writer.String("m_nNamesListOffset");
	writer.Uint(m_nNamesListOffset);

	writer.String("m_nNamesEntryIndexArrayOffset");
	writer.Uint(m_nNamesEntryIndexArrayOffset);

	writer.String("m_nFaceBoneIndexArrayOffset");
	writer.Uint(m_nFaceBoneIndexArrayOffset);

	writer.String("m_nFaceBoneCount");
	writer.Int(m_nFaceBoneCount);

	writer.EndObject();
}
