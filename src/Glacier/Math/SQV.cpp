#include "Glacier/Math/SQV.h"

void SQV::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_Rotation");
	m_Rotation.SerializeToJson(writer);

	writer.String("m_Translation");
	m_Translation.SerializeToJson(writer);

	writer.EndObject();
}
