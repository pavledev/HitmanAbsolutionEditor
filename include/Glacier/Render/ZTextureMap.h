#pragma once

class ZTextureMap
{
public:
	enum class ETextureFlags
	{
		TEXF_SWIZZLED = 1,
		TEXF_DEFERRED = 2,
		TEXF_MEMORY_READY_XBOX360 = 4,
		TEXF_GAMMA = 8,
		TEXF_EMISSIVE = 16,
		TEXF_DDSC_ENCODED = 32
	};

	enum class EInterpretAs
	{
		INTERPRET_AS_COLOR = 0,
		INTERPRET_AS_NORMAL = 1,
		INTERPRET_AS_HEIGHT = 2
	};

	enum class EDimensions
	{
		DIMENSIONS_2D = 0,
		DIMENSIONS_CUBE = 1,
		DIMENSIONS_VOLUME = 2
	};

	struct SMipLevel
	{
		unsigned int nWidth;
		unsigned int nHeight;
		unsigned int nSizeInBytes;
		unsigned char* pData;
	};

	struct STextureMapHeader
	{
		unsigned int nNumSlices;
		unsigned int nTotalSize;
		unsigned int nFlags;
		unsigned short nWidth;
		unsigned short nHeight;
		unsigned short nFormat;
		unsigned char nNumMipLevels;
		unsigned char nDefaultMipLevel;
		unsigned char nInterpretAs;
		unsigned char nDimensions;
		unsigned char nMipInterpolation;
		unsigned int nIADataSize;
		unsigned int nIADataOffset;
	};
};

inline ZTextureMap::ETextureFlags operator&(ZTextureMap::ETextureFlags lhs, ZTextureMap::ETextureFlags rhs)
{
	return static_cast<ZTextureMap::ETextureFlags>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
