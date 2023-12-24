#pragma once

#include <Glacier/Scaleform/SGfxResource.h>

#include "Resource.h"

class GFXMovie : public Resource
{
public:
	~GFXMovie() override;
	void Deserialize();
	void Parse(void* gfxResource);
	std::shared_ptr<SGfxResource> GetGfxResource();

private:
	std::shared_ptr<SGfxResource> gfxResource;
};
