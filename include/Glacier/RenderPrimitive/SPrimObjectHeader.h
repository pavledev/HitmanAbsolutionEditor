#pragma once

#include "SPrimHeader.h"
#include "Math/Vector3.h"

struct SPrimObjectHeader : SPrimHeader
{
	enum class PROPERTY_FLAGS
	{
		HAS_BONES = 1,
		HAS_FRAMES = 2,
		IS_LINKED_OBJECT = 4,
		IS_WEIGHTED_OBJECT = 8,
		USE_BOUNDS = 256,
		HAS_HIRES_POSITIONS = 512
	};

	unsigned int lPropertyFlags;
	unsigned int lBoneRigResourceIndex;
	unsigned int lNumObjects;
	unsigned int lObjectTable;
	Vector3 vBoundingBoxMin;
	Vector3 vBoundingBoxMax;
};

inline SPrimObjectHeader::PROPERTY_FLAGS operator&(SPrimObjectHeader::PROPERTY_FLAGS lhs, SPrimObjectHeader::PROPERTY_FLAGS rhs)
{
	return static_cast<SPrimObjectHeader::PROPERTY_FLAGS>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
