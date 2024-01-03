#include "Glacier/ScatterData/SScatterPackedMaterialBase.h"

void SScatterPackedMaterialBase::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_nInstances");
	writer.Uint(m_nInstances);

	writer.String("m_bBend");
	writer.Uint(m_bBend);

	writer.String("m_fBendConstraint");
	writer.Double(m_fBendConstraint);

	writer.String("m_fCutoffDistance");
	writer.Double(m_fCutoffDistance);

	writer.String("m_fScaleBeginDistance");
	writer.Double(m_fScaleBeginDistance);

	writer.EndObject();
}
