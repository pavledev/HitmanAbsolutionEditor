#include "Resources/WaveBank.h"

void WaveBank::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	waveBankResourceData = binaryReader.Read<SWaveBankResourceData>();

	isResourceDeserialized = true;
}

void WaveBank::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		waveBankResourceData.SerializeToJson(outputPath);
	}
}

SWaveBankResourceData& WaveBank::GetWaveBankResourceData()
{
	return waveBankResourceData;
}
