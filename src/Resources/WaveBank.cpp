#include "Resources/WaveBank.h"

void WaveBank::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	waveBankResourceData = binaryReader.Read<SWaveBankResourceData>();
}

SWaveBankResourceData& WaveBank::GetWaveBankResourceData()
{
	return waveBankResourceData;
}
