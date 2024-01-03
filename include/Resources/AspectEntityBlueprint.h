#pragma once

#include <vector>

#include "Resource.h"

class AspectEntityBlueprint : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;

private:
	std::vector<unsigned int> referenceIndices;
};
