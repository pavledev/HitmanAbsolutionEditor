#include "FSB/IO/BufferView.h"

namespace FSB
{
	namespace IO
	{
		BufferView::BufferView(const char* buffer, std::size_t length) : begin(buffer), end(buffer + length), current(buffer)
		{
		}

		BufferView::BufferView(const char* begin, const char* end) : begin(begin), end(end), current(begin)
		{
		}

		const char* BufferView::Begin() const
		{
			return begin;
		}

		const char* BufferView::End() const
		{
			return end;
		}

		const char* BufferView::Current() const
		{
			return current;
		}

		bool BufferView::Empty() const
		{
			return end == begin;
		}

		std::size_t BufferView::Size() const
		{
			return end - begin;
		}

		std::size_t BufferView::Remaining() const
		{
			return end - current;
		}

		std::size_t BufferView::Offset() const
		{
			return current - begin;
		}

		void BufferView::SetOffset(std::size_t offset)
		{
			if (offset > Size())
			{
				throw std::invalid_argument("Offset is greater then size.");
			}

			current = begin + offset;
		}

		void BufferView::Skip(std::size_t length)
		{
			if (Remaining() < length)
			{
				throw std::invalid_argument("Remaining size is smaller then length.");
			}

			current += length;
		}

		const char* BufferView::Read(std::size_t length)
		{
			if (Remaining() < length)
			{
				throw std::invalid_argument("Remaining size is smaller then length.");
			}

			const char* const result = current;
			current += length;

			return result;
		}

		char BufferView::ReadChar()
		{
			if (Remaining() < 1u)
			{
				throw std::invalid_argument("Remaining size is smaller then 1.");
			}

			return *current++;
		}

		uint8_t BufferView::ReadUInt8()
		{
			if (Remaining() < 1u)
			{
				throw std::invalid_argument("Remaining size is smaller then 1.");
			}

			return *current++;
		}

		uint16_t BufferView::ReadUInt16()
		{
			if (Remaining() < 1u)
			{
				throw std::invalid_argument("Remaining size is smaller then 2.");
			}

			const uint16_t x = ReadUInt8();
			const uint16_t y = ReadUInt8();

			return x + (y << 8u);
		}

		uint32_t BufferView::ReadUInt24()
		{
			if (Remaining() < 3u)
			{
				throw std::invalid_argument("Remaining size is smaller then 3.");
			}

			const uint32_t x = ReadUInt8();
			const uint32_t y = ReadUInt16();

			return x + (y << 8u);
		}

		uint32_t BufferView::ReadUInt32()
		{
			if (Remaining() < 4u)
			{
				throw std::invalid_argument("Remaining size is smaller then 4.");
			}

			const uint32_t x = ReadUInt16();
			const uint32_t y = ReadUInt16();

			return x + (y << 16u);
		}

		uint64_t BufferView::ReadUInt64()
		{
			if (Remaining() < 8u)
			{
				throw std::invalid_argument("Remaining size is smaller then 8.");
			}

			const uint64_t x = ReadUInt32();
			const uint64_t y = ReadUInt32();

			return x + (y << 32u);
		}

	}
}
