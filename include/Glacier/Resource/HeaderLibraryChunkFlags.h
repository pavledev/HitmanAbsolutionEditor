#pragma once

enum class HeaderLibraryChunkFlags
{
	GLOBAL = 1,
	NON_STREAMED = 2,
	MEDIA_STREAMED = 4,
	INIT_LIBRARY = 8,
	DYNAMIC = 16
};

inline HeaderLibraryChunkFlags operator&(HeaderLibraryChunkFlags lhs, HeaderLibraryChunkFlags rhs)
{
	return static_cast<HeaderLibraryChunkFlags>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
