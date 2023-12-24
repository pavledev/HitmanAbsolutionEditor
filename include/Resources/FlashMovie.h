#pragma once

#include <d3d11.h>

#include "Resource.h"

class FlashMovie : public Resource
{
public:
	void GetTextureData(void*& textureData, unsigned int& textureDataSize, bool& isDDSTexture);
	void CreateTextureFromMemory();
	const unsigned int GetTextureWidth() const;
	const unsigned int GetTextureHeight() const;
	ID3D11Resource* GetTexture() const;
	ID3D11ShaderResourceView* GetTextureView() const;

private:
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	unsigned int textureWidth;
	unsigned int textureHeight;
};
