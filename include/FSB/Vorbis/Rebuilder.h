#pragma once

#include <cstdint>
#include <iosfwd>

#include "../FSB.h"
#include "../IO/BufferView.h"
#include "Vorbis.h"
#include "IO/BinaryWriter.h"

namespace FSB
{
    namespace Vorbis
    {
        // Rebuilds Vorbis headers and audio data.
        class Rebuilder
        {
        public:
            Rebuilder();

            // Rebuilds sample and write it to a stream.
            static void Rebuild(const Sample& sample, IO::BufferView& sampleView, std::ostream& stream);
            static void Rebuild(const Sample& sample, IO::BufferView& sampleView, BinaryWriter& binaryWriter);

            // Rebuilds Vorbis headers and returns them as Ogg packets.
            static void RebuildHeaders(
                int channels, int rate, std::uint32_t crc32,
                std::uint32_t loopStart, std::uint32_t loopEnd,
                OGGPacketHolder& id,
                OGGPacketHolder& comment,
                OGGPacketHolder& setup);

            // Rebuilds identification header.
            static void RebuildIDHeader(int channels, int rate, int blocksizeShort, int blocksizeLong, OGGPacketHolder& packet);

            // Rebuilds comment header.
            static void RebuildCommentHeader(OGGPacketHolder& packet, std::uint32_t loopStart, std::uint32_t loopEnd);

            // Rebuilds setup header.
            static void RebuildSetupHeader(const char* payload, std::size_t payloadSize, OGGPacketHolder& packet);

        private:
            Rebuilder(const Rebuilder& rhs) = delete;
            Rebuilder& operator=(const Rebuilder& rhs) = delete;
        };
    }
}
