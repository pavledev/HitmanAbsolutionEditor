#pragma once

#include "SPrimObject.h"

struct SPrimSubMesh : SPrimObject
{
	unsigned int lNumVertices;
	unsigned int lVertices;
	unsigned int lNumIndices;
	unsigned int lIndices;
	unsigned int lCollision;
	unsigned short lColorStreamSize;
	unsigned char lNumUVChannels;
	unsigned char lDummy1;
};
