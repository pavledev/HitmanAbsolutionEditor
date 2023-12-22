#include "FSB/Vorbis/Vorbis.h"

namespace FSB
{
    namespace Vorbis
    {
        OGGPacketHolder::OGGPacketHolder() : value{}
        {
        }

        OGGPacketHolder::~OGGPacketHolder()
        {
            ogg_packet_clear(&value);
        }

        void OGGPacketHolder::Assign(const unsigned char* buffer, std::size_t buffer_size)
        {
            unsigned char* const packet = static_cast<unsigned char*>(std::malloc(buffer_size));

            if (!packet || !buffer_size)
            {
                throw std::invalid_argument("Failed to allocate memory.");
            }

            std::copy_n(buffer, buffer_size, packet);
            std::free(value.packet);

            value.packet = packet;
            value.bytes = buffer_size;
        }

        void OGGPacketHolder::Assign(const char* buffer, std::size_t buffer_size)
        {
            Assign(reinterpret_cast<const unsigned char*>(buffer), buffer_size);
        }

        void OGGPacketHolder::Clear()
        {
            ogg_packet_clear(&value);
        }

        VorbisInfoHolder::VorbisInfoHolder()
        {
            vorbis_info_init(&value);
        }

        VorbisInfoHolder::~VorbisInfoHolder()
        {
            vorbis_info_clear(&value);
        }

        VorbisCommentHolder::VorbisCommentHolder()
        {
            vorbis_comment_init(&value);
        }

        void VorbisCommentHolder::AddTag(const char* tag, const char* contents)
        {
            vorbis_comment_add_tag(&value, tag, contents);
        }

        VorbisCommentHolder::~VorbisCommentHolder()
        {
            vorbis_comment_clear(&value);
        }

        OGGOuputStream::OGGOuputStream(int serialNumber, std::ostream& output) : output(output)
        {
            if (ogg_stream_init(&stream_state, serialNumber) != 0)
            {
                throw std::invalid_argument("Couldn't initialize ogg stream.");
            }
        }

        OGGOuputStream::~OGGOuputStream()
        {
            ogg_stream_clear(&stream_state);
        }

        void OGGOuputStream::WritePacket(ogg_packet& packet)
        {
            if (ogg_stream_packetin(&stream_state, &packet) != 0)
            {
                throw std::invalid_argument("Couldn't write packet.");
            }

            ogg_page page;

            while (ogg_stream_pageout(&stream_state, &page))
            {
                WritePage(page);
            }
        }

        void OGGOuputStream::FlushPackets()
        {
            ogg_page page;

            while (ogg_stream_flush(&stream_state, &page))
            {
                WritePage(page);
            }
        }

        void OGGOuputStream::WritePage(const ogg_page& page)
        {
            output.write(reinterpret_cast<char*>(page.header), page.header_len);
            output.write(reinterpret_cast<char*>(page.body), page.body_len);
        }

        OGGBinaryWriter::OGGBinaryWriter(int serialNumber, BinaryWriter& binaryWriter) : binaryWriter(binaryWriter)
        {
            if (ogg_stream_init(&streamState, serialNumber) != 0)
            {
                throw std::invalid_argument("Couldn't initialize ogg stream.");
            }
        }

        OGGBinaryWriter::~OGGBinaryWriter()
        {
            ogg_stream_clear(&streamState);
        }

        void OGGBinaryWriter::WritePacket(ogg_packet& packet)
        {
            if (ogg_stream_packetin(&streamState, &packet) != 0)
            {
                throw std::invalid_argument("Couldn't write packet.");
            }

            ogg_page page;

            while (ogg_stream_pageout(&streamState, &page))
            {
                WritePage(page);
            }
        }

        void OGGBinaryWriter::FlushPackets()
        {
            ogg_page page;

            while (ogg_stream_flush(&streamState, &page))
            {
                WritePage(page);
            }
        }

        void OGGBinaryWriter::WritePage(const ogg_page& page)
        {
            binaryWriter.Write(page.header, page.header_len);
            binaryWriter.Write(page.body, page.body_len);
        }
    }
}
