#pragma once

#include "../ZString.hpp"
#include "../TArray.hpp"

namespace IOI
{
	struct SGfxResource
	{
		ZString movieName;
		unsigned char* gfxData;
		int gfxDataLength;
		TArray<ZString> filenames;
		TArray<TArray<unsigned char>> fileContents;
	};
}
