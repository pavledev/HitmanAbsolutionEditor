#pragma once

#include <vector>

#include "Resource.h"

class AspectEntityBlueprint : public Resource
{
public:
	void Deserialize();

private:
	std::vector<unsigned int> referenceIndices;
};
