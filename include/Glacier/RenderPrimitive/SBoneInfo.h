#pragma once

struct SBoneInfo
{
	unsigned short nTotalSize;
	unsigned short nNumAccelEntries;
	unsigned char aBoneRemap[255];
	unsigned char nPad;
};
