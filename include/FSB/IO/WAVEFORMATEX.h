#pragma once

namespace FSB
{
	namespace IO
	{
		struct WAVEFORMATEX
		{
			unsigned short wFormatTag;
			unsigned short nChannels;
			unsigned int nSamplesPerSec;
			unsigned int nAvgBytesPerSec;
			unsigned short nBlockAlign;
			unsigned short wBitsPerSample;
			unsigned short cbSize;
		};
	}
}
