#pragma once

#include "../ZString.h"
#include "../Containers/TArray.h"

struct SGfxResource
{
	ZString movieName;
	unsigned char* gfxData;
	int gfxDataLength;
	TArray<ZString> filenames;
	TArray<TArray<unsigned char>> fileContents;
};
