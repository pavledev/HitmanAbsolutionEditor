#pragma once

#include "SPrimMesh.h"

struct SPrimMeshWeighted : SPrimMesh
{
	unsigned int lNumCopyBones;
	unsigned int lCopyBones;
	unsigned int lBoneIndices;
	unsigned int lBoneInfo;
};
