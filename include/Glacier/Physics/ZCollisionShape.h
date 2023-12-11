#pragma once

#include <vector>

#include "rapidjson/prettywriter.h"

#include "G2NxShapeDesc.h"

class ZCollisionShape
{
public:
	~ZCollisionShape();
	const std::vector<G2NxShapeDesc*>& GetShapeDescriptors() const;
	std::vector<G2NxShapeDesc*>& GetShapeDescriptors();
	void AddShapeDescriptor(G2NxShapeDesc* shapeDescriptor);
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

private:
	std::vector<G2NxShapeDesc*> m_pNxShapeList;
};
