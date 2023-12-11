#include "Glacier/Physics/ZCollisionShape.h"

ZCollisionShape::~ZCollisionShape()
{
	for (size_t i = 0; i < m_pNxShapeList.size(); ++i)
	{
		delete m_pNxShapeList[i];
	}

	m_pNxShapeList.clear();
}

const std::vector<G2NxShapeDesc*>& ZCollisionShape::GetShapeDescriptors() const
{
	return m_pNxShapeList;
}

std::vector<G2NxShapeDesc*>& ZCollisionShape::GetShapeDescriptors()
{
	return m_pNxShapeList;
}

void ZCollisionShape::AddShapeDescriptor(G2NxShapeDesc* shapeDescriptor)
{
	m_pNxShapeList.push_back(shapeDescriptor);
}

void ZCollisionShape::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_pNxShapeList");
	writer.StartArray();

	for (size_t i = 0; i < m_pNxShapeList.size(); ++i)
	{
		m_pNxShapeList[i]->SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();
}
