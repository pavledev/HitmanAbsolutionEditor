#pragma once

#include <cstddef>
#include <iosfwd>
#include <vector>

#include "IO/BinaryReader.h"

namespace FSB
{
	namespace IO
	{
		class Utility
		{
		public:
			static void Read(std::istream& stream, char* buffer, std::size_t size);
			static std::vector<char> Read(std::istream& stream, std::size_t size);

			static void Read(BinaryReader& binaryReader, char* buffer, std::size_t size);
			static std::vector<char> Read(BinaryReader& binaryReader, std::size_t size);
		};
	}
}
