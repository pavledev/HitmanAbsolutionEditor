#pragma once

#include "WaveBankFSB.h"

class WaveBankFSBF : public WaveBankFSB
{
public:
	FSB::Format GetFormat() override;
	void GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size) override;
};
