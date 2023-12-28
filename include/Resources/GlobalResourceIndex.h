#pragma once

#include "Glacier/GlobalResourceIndex/SResourceIndex.h"

#include "Resource.h"

class GlobalResourceIndex : public Resource
{
public:
	void Deserialize() override;
	void Parse(void* resourceIndex);
	std::shared_ptr<SResourceIndex> GetResourceIndex();
	void SerializeToJson(const std::string& outputFilePath);

private:
	std::shared_ptr<SResourceIndex> resourceIndex;
};
