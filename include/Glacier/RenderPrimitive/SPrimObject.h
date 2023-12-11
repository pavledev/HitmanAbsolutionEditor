#pragma once

#include "SPrimHeader.h"
#include "Math/Vector3.h"

struct SPrimObject : SPrimHeader
{
	enum class SUBTYPE
	{
		SUBTYPE_STANDARD = 0,
		SUBTYPE_LINKED = 1,
		SUBTYPE_WEIGHTED = 2,
		SUBTYPE_STANDARDUV2 = 3,
		SUBTYPE_STANDARDUV3 = 4,
		SUBTYPE_STANDARDUV4 = 5
	};

	enum class PROPERTY_FLAGS
	{
		PROPERTY_XAXISLOCKED = 1,
		PROPERTY_YAXISLOCKED = 2,
		PROPERTY_ZAXISLOCKED = 4,
		PROPERTY_HIRES_POSITIONS = 8,
		PROPERTY_PS3_EDGE = 16,
		PROPERTY_COLOR1 = 32,
		PROPERTY_ISNOPHYSICSPROP = 64
	};

	static std::string ConvertSubTypeToString(const SUBTYPE subType);

	unsigned char lSubType;
	unsigned char lProperties;
	unsigned char lLODMask;
	unsigned char lVariantId;
	unsigned char nZBias;
	unsigned char nZOffset;
	unsigned short lMaterialId;
	unsigned int lWireColor;
	unsigned int lColor1;
	Vector3 vBoundingBoxMin;
	Vector3 vBoundingBoxMax;
};
