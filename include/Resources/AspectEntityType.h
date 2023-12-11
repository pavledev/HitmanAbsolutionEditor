#pragma once

#include <vector>

#include "Resource.h"

class AspectEntityType : public Resource
{
public:
	void Deserialize();

private:
	std::vector<unsigned int> referenceIndices;
};
