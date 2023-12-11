#include "Glacier/BoneRig/SPoseBone.h"
#include "Glacier/Math/SVector4.h"

void SPoseBone::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	SVector4 quaternion = SVector4(m_quat[0], m_quat[1], m_quat[2], m_quat[3]);
	SVector4 position = SVector4(m_pos[0], m_pos[1], m_pos[2], m_pos[3]);

	writer.String("m_quat");
	quaternion.SerializeToJson(writer);

	writer.String("m_pos");
	position.SerializeToJson(writer);

	writer.EndObject();
}
