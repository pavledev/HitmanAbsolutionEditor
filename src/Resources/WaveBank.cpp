#include "Resources/WaveBank.h"

void WaveBank::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	waveBankResourceData = binaryReader.Read<SWaveBankResourceData>();

	isResourceDeserialized = true;
}

SWaveBankResourceData& WaveBank::GetWaveBankResourceData()
{
	return waveBankResourceData;
}
