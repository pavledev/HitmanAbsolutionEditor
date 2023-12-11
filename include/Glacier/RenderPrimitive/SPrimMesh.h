#pragma once

#include "SPrimObject.h"
#include "Math/Vector2.h"
#include "Math/Vector4.h"

struct SPrimMesh : SPrimObject
{
	unsigned int lSubMeshTable;
	unsigned int lNumFrames;
	unsigned short lFrameStart;
	unsigned short lFrameStep;
	Vector4 vPosScale;
	Vector4 vPosBias;
	Vector2 vTexScale;
	Vector2 vTexBias;
};
