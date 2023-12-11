#pragma once

#include <string>

struct SPrimHeader
{
	enum class EPrimType
	{
		PTOBJECTHEADER = 1,
		PTMESH = 2,
		PTDECAL = 3,
		PTSPRITES = 4,
		PTSHAPE = 5,
		PTUNUSED = 6
	};

	static std::string ConvertPrimTypeToString(const EPrimType primType);

	unsigned char lDrawDestination;
	unsigned char lPackType;
	unsigned short lType;
};
