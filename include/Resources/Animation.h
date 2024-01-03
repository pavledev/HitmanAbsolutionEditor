#pragma once

#include "Resource.h"

class Animation : public Resource
{
public:
	void Export(const std::string& outputPath, const std::string& exportOption) override;
};
