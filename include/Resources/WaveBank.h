#pragma once

#include "Glacier/Sound/SWaveBankResourceData.h"

#include "Resource.h"

class WaveBank : public Resource
{
public:
	void Deserialize();
	SWaveBankResourceData& GetWaveBankResourceData();

private:
	SWaveBankResourceData waveBankResourceData;
};
