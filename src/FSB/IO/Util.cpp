#include <limits>
#include <istream>

#include "FSB/IO/Util.h"

#undef max

namespace FSB
{
    namespace IO
    {
        void Utility::Read(BinaryReader& binaryReader, char* buffer, std::size_t size)
        {
            constexpr std::streamsize streamSizeMax = std::numeric_limits<std::streamsize>::max();

            while (size > 0)
            {
                const std::streamsize toRead = streamSizeMax < size ? streamSizeMax : size;

                binaryReader.Read(buffer, static_cast<size_t>(toRead));

                size -= static_cast<size_t>(toRead);
            }
        }

        std::vector<char> Utility::Read(BinaryReader& binaryReader, std::size_t size)
        {
            std::vector<char> buffer = std::vector<char>(size);

            Read(binaryReader, buffer.data(), buffer.size());

            return buffer;
        }
    }
}
