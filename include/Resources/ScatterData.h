#pragma once

#include "Glacier/ScatterData/SScatterDataHeader.h"
#include "Glacier/ScatterData/SScatterPackedMaterial.h"
#include "Glacier/ScatterData/SScatterPackedInstance.h"

#include "Resource.h"

class ScatterData : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(const std::string& outputFilePath);
	SScatterDataHeader& GetScatterDataHeader();
	std::vector<SScatterPackedMaterial>& GetMaterialHeaders();
	std::vector<std::vector<SScatterPackedInstance>>& GetInstances();

private:
	SScatterDataHeader scatterDataHeader;
	std::vector<SScatterPackedMaterial> materialHeaders;
	std::vector<std::vector<SScatterPackedInstance>> instances;
};
