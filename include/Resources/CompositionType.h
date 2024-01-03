#pragma once

#include "Resource.h"

class CompositionType : public Resource
{
public:
	void Export(const std::string& outputPath, const std::string& exportOption) override;
};
