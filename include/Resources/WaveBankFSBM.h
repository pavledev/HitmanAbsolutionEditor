#pragma once

#include "WaveBankFSB.h"

class WaveBankFSBM : public WaveBankFSB
{
public:
	FSB::Format GetFormat() override;
	void GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size) override;
};
