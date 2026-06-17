#pragma once

#include "WaveBankFSB.h"
#include "WaveBank.h"

class WaveBankFSBS : public WaveBankFSB
{
public:
	FSB::Format GetFormat() override;
	void GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size) override;
	void GetFilePaths(std::vector<std::string>& filePaths);
	bool PatchResourceLibrary(const std::string& oggFilePath, int indexToReplace = 0);

private:
	unsigned int FindFSB5DataOffsetInFSBS();
	void PatchWAVBDuration(const std::vector<unsigned char>& newFsbData);
};
