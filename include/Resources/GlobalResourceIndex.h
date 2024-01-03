#pragma once

#include "Glacier/GlobalResourceIndex/SResourceIndex.h"

#include "Resource.h"

class GlobalResourceIndex : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void Parse(void* resourceIndex);
	std::shared_ptr<SResourceIndex> GetResourceIndex();

private:
	std::shared_ptr<SResourceIndex> resourceIndex;
};
