#pragma once

enum class EResourceReferenceFlags : unsigned int
{
	RESOURCE_REFERENCE_NO_FLAGS = 0x0,
	RESOURCE_REFERENCE_OFFSET_BITS = 0xFFFFFF,
	RESOURCE_REFERENCE_OFFSET_BITCOUNT = 0x18,
	RESOURCE_REFERENCE_FLAG_BITS = 0xFF000000,
	RESOURCE_REFERENCE_TYPE_OF_STREAMING_ENTITY = 0x10000000,
	RESOURCE_REFERENCE_STATE_STREAMED = 0x20000000,
	RESOURCE_REFERENCE_MEDIA_STREAMED = 0x40000000,
	RESOURCE_REFERENCE_INSTALL_DEPENDENCY = 0x80000000,
	RESOURCE_REFERENCE_CYCLIC = 0x4000000
};

inline EResourceReferenceFlags operator&(EResourceReferenceFlags lhs, EResourceReferenceFlags rhs)
{
	return static_cast<EResourceReferenceFlags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}
