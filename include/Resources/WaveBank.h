#pragma once

#include "Glacier/Sound/SWaveBankResourceData.h"

#include "Resource.h"

class WaveBank : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	SWaveBankResourceData& GetWaveBankResourceData();

private:
	SWaveBankResourceData waveBankResourceData;
};
