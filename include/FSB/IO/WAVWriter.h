#pragma once

#include <cstring>

#include "WAVHeader.h"
#include "IMAADPCMWAVEFORMAT.h"
#include "../FSB.h"
#include "IO/BinaryWriter.h"

namespace FSB
{
	namespace IO
	{
		class WAVWriter
		{
		public:
			static WAVHeader GenerateWAVHeader(const FSB::Sample& sample);
			static void WriteToFile(const FSB::Sample& sample, const char* const data, std::ostream& stream);
			static void WriteToFile(const FSB::Sample& sample, char* data, BinaryWriter& binaryWriter);
		};
	}
}
