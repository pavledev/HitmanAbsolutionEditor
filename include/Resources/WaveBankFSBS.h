#pragma once

#include "WaveBankFSB.h"

class WaveBankFSBS : public WaveBankFSB
{
public:
	FSB::Format GetFormat() override;
	void GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size) override;
	void GetFilePaths(std::vector<std::string>& filePaths);

private:
	unsigned int FindFSB5DataOffsetInFSBS();
};
