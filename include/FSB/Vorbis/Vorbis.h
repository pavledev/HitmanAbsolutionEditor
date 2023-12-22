#pragma once

#include <cstdint>
#include <iosfwd>

#include "ogg/ogg.h"
#include "vorbis/codec.h"

#include "../FSB.h"
#include "IO/BinaryWriter.h"

namespace FSB
{
    namespace Vorbis
    {
        // Returns CRC-32 of Vorbis packet payload.
        //std::uint32_t CRC32(ogg_packet const& packet);

        // RAII holder for ogg_packet.
        class OGGPacketHolder
        {
        public:
            // Value initializes ogg_packet.
            OGGPacketHolder();
            // Clears packet with ogg_packet_clear.
            ~OGGPacketHolder();

            operator ogg_packet* ()
            {
                return &value;
            }
            operator ogg_packet& ()
            {
                return value;
            }
            ogg_packet* operator->()
            {
                return &value;
            }

            // Copies a content of the range [buffer, buffer + bytes) to a given Ogg 
            // packet. Updates packet and bytes fields.
            // 
            // Previous content of a packet is freed.
            void Assign(const unsigned char* buffer, std::size_t buffer_size);
            void Assign(const char* buffer, std::size_t buffer_size);

            // Clears packet content.
            void Clear();

        private:
            OGGPacketHolder(const OGGPacketHolder& rhs) = delete;
            OGGPacketHolder& operator=(const OGGPacketHolder& rhs) = delete;

            ogg_packet value;
        };

        // RAII wrapper for ogg_stream_state that forwards written data to std::ostream.
        class OGGOuputStream
        {
        public:
            // Construct Ogg stream with given serial number, that will write it's output
            // to given output stream.
            OGGOuputStream(int serialNumber, std::ostream& output);

            // Destroys Ogg stream.
            ~OGGOuputStream();

            // Submits packet to Ogg stream. Writes complete pages to output stream.
            void WritePacket(ogg_packet& packet);

            // Flushes remaining packets inside the Ogg stream and forces them into pages.
            //
            // Note: It does not flush output stream.
            void FlushPackets();

        private:
            // Writes page header and page body to output stream.
            void WritePage(const ogg_page& page);

            OGGOuputStream(const OGGOuputStream& rhs) = delete;
            OGGOuputStream& operator=(const OGGOuputStream& rhs) = delete;

            std::ostream& output;
            ogg_stream_state stream_state;
        };

        class OGGBinaryWriter
        {
        public:
            // Construct Ogg stream with given serial number, that will write it's output
            // to given output stream.
            OGGBinaryWriter(int serialNumber, BinaryWriter& binaryWriter);

            // Destroys Ogg stream.
            ~OGGBinaryWriter();

            // Submits packet to Ogg stream. Writes complete pages to output stream.
            void WritePacket(ogg_packet& packet);

            // Flushes remaining packets inside the Ogg stream and forces them into pages.
            //
            // Note: It does not flush output stream.
            void FlushPackets();

        private:
            // Writes page header and page body to output stream.
            void WritePage(const ogg_page& page);

            OGGBinaryWriter(const OGGBinaryWriter& rhs) = delete;
            OGGBinaryWriter& operator=(const OGGBinaryWriter& rhs) = delete;

            BinaryWriter& binaryWriter;
            ogg_stream_state streamState;
        };

        // RAII holder for vorbis_info.
        class VorbisInfoHolder
        {
        public:
            // Initializes vorbis_info with vorbis_info_init.
            VorbisInfoHolder();
            // Clears vorbis_info with vorbis_info_clear.
            ~VorbisInfoHolder();

            operator vorbis_info* ()
            {
                return &value;
            }
            operator vorbis_info& ()
            {
                return value;
            }
            vorbis_info* operator->()
            {
                return &value;
            }

        private:
            VorbisInfoHolder(const VorbisInfoHolder& rhs) = delete;
            VorbisInfoHolder& operator=(const VorbisInfoHolder& rhs) = delete;

            vorbis_info value;
        };

        // RAII holder for vorbis_comment.
        class VorbisCommentHolder
        {
        public:
            // Intializes vorbis_comment with vorbis_comment_init,
            VorbisCommentHolder();

            // Add a tag-comment pair
            void AddTag(const char* tag, const char* contents);

            // Clears vorbis_comment with vorbis_comment_clear.
            ~VorbisCommentHolder();

            operator vorbis_comment* ()
            {
                return &value;
            }
            operator vorbis_comment& ()
            {
                return value;
            }
            vorbis_comment* operator->()
            {
                return &value;
            }

        private:
            VorbisCommentHolder(const VorbisCommentHolder& rhs) = delete;
            VorbisCommentHolder& operator=(const VorbisCommentHolder& rhs) = delete;

            vorbis_comment value;
        };
    }
}
