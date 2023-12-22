#include <format>

#include "FSB/Vorbis/Rebuilder.h"

namespace FSB
{
    namespace Vorbis
    {
        Rebuilder::Rebuilder()
        {

        }

        void Rebuilder::Rebuild(const Sample& sample, IO::BufferView& sampleView, BinaryWriter& binaryWriter)
        {
            VorbisInfoHolder info;
            VorbisCommentHolder comment;

            long prevBlocksize = 0;
            ogg_int64_t prevGranulepos = 0;
            ogg_int64_t prevPacketno = 0;

            OGGBinaryWriter oggBinaryWriter(1, binaryWriter);

            {
                // Reconstruct and write Vorbis headers to stream.
                OGGPacketHolder headerID;
                OGGPacketHolder headerComment;
                OGGPacketHolder headerSetup;

                RebuildHeaders(
                    sample.channels, sample.frequency, sample.vorbisCRC32,
                    sample.loopStart, sample.loopEnd,
                    headerID, headerComment, headerSetup);

                if (vorbis_synthesis_headerin(info, comment, headerID) != 0)
                {
                    throw std::invalid_argument("Result of vorbis_synthesis_headerin isn't 0.");
                }

                if (vorbis_synthesis_headerin(info, comment, headerComment) != 0)
                {
                    throw std::invalid_argument("Result of vorbis_synthesis_headerin isn't 0.");
                }

                if (vorbis_synthesis_headerin(info, comment, headerSetup) != 0)
                {
                    throw std::invalid_argument("Result of vorbis_synthesis_headerin isn't 0.");
                }

                oggBinaryWriter.WritePacket(headerID);
                oggBinaryWriter.WritePacket(headerComment);
                oggBinaryWriter.WritePacket(headerSetup);
                oggBinaryWriter.FlushPackets();

                prevPacketno = headerSetup->packetno;
                prevGranulepos = 0;
            }

            {
                // Reconstruct audio packets.
                std::uint16_t packetSize = sampleView.ReadUInt16();
                while (packetSize)
                {
                    ogg_packet packet{};
                    packet.packet =
                        reinterpret_cast<unsigned char*>(
                            const_cast<char*>(sampleView.Read(packetSize)));
                    packet.bytes = packetSize;
                    packet.packetno = prevPacketno + 1;
                    packet.granulepos = -1;

                    // Read size of next packet to determine if we reached end of stream.
                    packetSize = sampleView.Offset() + 2 < sampleView.Size() ?
                        sampleView.ReadUInt16() : 0;
                    packet.e_o_s = packetSize ? 0 : 1;

                    // Update granulepos for packet.
                    const long blocksize = vorbis_packet_blocksize(info, &packet);
                    std::string message = std::format("vorbis_packet_blocksize failed: {}", blocksize);

                    if (blocksize <= 0)
                    {
                        throw std::invalid_argument(message);
                    }

                    packet.granulepos = prevBlocksize ? prevGranulepos + (blocksize + prevBlocksize) / 4 : 0;

                    oggBinaryWriter.WritePacket(packet);

                    prevBlocksize = blocksize;
                    prevGranulepos = packet.granulepos;
                    prevPacketno = packet.packetno;
                }
            }
        }

        namespace
        {
            struct HeadersInfo
            {
                std::uint32_t crc32;
                int blocksizeShort;
                int blocksizeLong;
                std::size_t setupHeaderSize;
                const char* setupHeader;
            };

            struct HeadersInfoCRC32Less
            {
                bool operator()(const HeadersInfo& lhs, std::uint32_t rhs) const
                {
                    return lhs.crc32 < rhs;
                }
            };

            // Vorbis setup headers map (ordered according to crc32).
            const HeadersInfo headers[]{
            #include "FSB/Vorbis/headers.inc"
            };

            const HeadersInfo* const headersEnd = headers + sizeof(headers) / (sizeof(HeadersInfo) == 0 ? 1 : sizeof(HeadersInfo));
        }

        void Rebuilder::RebuildHeaders(
            int channels, int rate, std::uint32_t crc32,
            std::uint32_t loop_start, std::uint32_t loop_end,
            OGGPacketHolder& id,
            OGGPacketHolder& comment,
            OGGPacketHolder& setup)
        {

            const auto i = std::lower_bound(headers, headersEnd, crc32, HeadersInfoCRC32Less());
            std::string message = std::format("Headers with CRC-32 equal {} not found.", crc32);

            if (headersEnd == i && i->crc32 != crc32)
            {
                throw std::invalid_argument(message);
            }

            RebuildIDHeader(channels, rate, i->blocksizeShort, i->blocksizeLong, id);
            RebuildCommentHeader(comment, loop_start, loop_end);
            RebuildSetupHeader(i->setupHeader, i->setupHeaderSize, setup);
        }

        namespace
        {
            // Returns ceil(log2(x)) of non-negative integer.
            int ilog2(int v)
            {
                if (v <= 0)
                {
                    throw std::invalid_argument("Value must be greather then 0.");
                }

                int result = 0;

                if (v)
                {
                    --v;
                }
                while (v)
                {
                    result++;
                    v >>= 1;
                }

                return result;
            }

            // Writes string to Ogg buffer as sequence of bytes.
            void oggpack_write_string(oggpack_buffer* opb, std::string str)
            {
                for (auto c : str)
                {
                    oggpack_write(opb, c, 8);
                }
            }
        }

        void Rebuilder::RebuildIDHeader(int channels, int rate, int blocksizeShort, int blocksizeLong, OGGPacketHolder& packet)
        {
            // Identification header
            oggpack_buffer opb;
            oggpack_writeinit(&opb);

            // Preamble
            oggpack_write(&opb, 0x01, 8);
            oggpack_write_string(&opb, "vorbis");

            // Basic information about the stream.
            oggpack_write(&opb, 0x00, 32);
            oggpack_write(&opb, channels, 8);
            oggpack_write(&opb, rate, 32);

            // Bitrate upper, nominal and lower.
            // All are optional and we do not provide them.
            oggpack_write(&opb, 0, 32);
            oggpack_write(&opb, 0, 32);
            oggpack_write(&opb, 0, 32);

            oggpack_write(&opb, ilog2(blocksizeShort), 4);
            oggpack_write(&opb, ilog2(blocksizeLong), 4);
            oggpack_write(&opb, 1, 1);

            //CHECK(oggpack_writecheck(&opb) == 0);

            packet.Assign(opb.buffer, oggpack_bytes(&opb));
            packet->b_o_s = 1;
            packet->e_o_s = 0;
            packet->granulepos = 0;
            packet->packetno = 0;

            oggpack_writeclear(&opb);
        }

        void Rebuilder::RebuildCommentHeader(OGGPacketHolder& packet, std::uint32_t loopStart, std::uint32_t loopEnd)
        {
            // Comments header are generated using libvorbis for simplicity.
            VorbisCommentHolder comment;
            if (loopStart != 0 && loopEnd != 0)
            {
                const std::string s_loop_start = std::to_string(loopStart);
                const std::string s_loop_end = std::to_string(loopEnd);
                comment.AddTag("LOOP_START", s_loop_start.c_str());
                comment.AddTag("LOOP_END", s_loop_end.c_str());
            }

            packet.Clear();
            vorbis_commentheader_out(comment, packet);
        }

        void Rebuilder::RebuildSetupHeader(const char* payload, std::size_t payloadSize, OGGPacketHolder& packet)
        {
            if (payloadSize == 0)
            {
                throw std::invalid_argument("Empty setup header.");
            }

            // Setup header 
            packet.Assign(payload, payloadSize);
            packet->b_o_s = 0;
            packet->e_o_s = 0;
            packet->granulepos = 0;
            packet->packetno = 2;
        }
    }
}
