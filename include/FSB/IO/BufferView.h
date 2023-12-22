#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>

namespace FSB
{
    namespace IO
    {
        // View on sequence of bytes that allows to read them as integers of different 
        // sizes. Holds and offset within buffer that is updated by a number of bytes
        // read, after each read operation.
        //
        // Does not retain ownership of given byte buffer.
        class BufferView
        {
        public:
            BufferView(const char* buffer, std::size_t length);
            BufferView(const char* begin, const char* end);

            // Returns pointer to the first character of buffer.
            const char* Begin() const;

            // Returns a pointer to the past-the-end character of buffer.
            const char* End() const;

            // Returns a pointer to the current character of buffer.
            const char* Current() const;

            // Returns true if the buffer is empty.
            bool Empty() const;

            // Returns the size of the buffer.
            std::size_t Size() const;

            // Returns number of bytes remaining in the buffer from current offset.
            std::size_t Remaining() const;

            // Returns current position in the buffer.
            std::size_t Offset() const;

            // Next read will start from given offset.
            void SetOffset(std::size_t offset);

            // Skips next length bytes.
            void Skip(std::size_t length);

            // Reads length bytes and returns pointer to the beginning of read data.
            const char* Read(std::size_t length);

            // Reads character.
            char ReadChar();

            // Reads 8-bit unsigned integer.
            unsigned char ReadUInt8();

            // Reads 16-bit unsigned integer.
            unsigned short ReadUInt16();

            // Reads 24-bit unsigned integer.
            unsigned int ReadUInt24();

            // Reads 32-bit unsigned integer.
            unsigned int ReadUInt32();

            // Reads 64-bit unsigned integer.
            unsigned long long ReadUInt64();

        private:
            const char* begin;
            const char* end;
            const char* current;
        };
    }
}
