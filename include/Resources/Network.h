#pragma once

#include <string>

#include "Glacier/Morpheme/Morpheme.h"

#include "Resources/Resource.h"

class Network : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(std::string& jsonOutput);

private:
	MR::NetworkDef* networkDef;
};
