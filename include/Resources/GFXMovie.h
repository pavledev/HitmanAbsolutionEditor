#pragma once

#include <Glacier/Scaleform/SGfxResource.h>

#include "Resource.h"

class GFXMovie : public Resource
{
public:
	~GFXMovie() override;
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void Parse(void* gfxResource);
	std::shared_ptr<SGfxResource> GetGfxResource();

private:
	std::shared_ptr<SGfxResource> gfxResource;
};
