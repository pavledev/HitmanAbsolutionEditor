#pragma once

#include <string>

#include "DirectXTex.h"

#include "Glacier/Render/ZTextureMap.h"
#include "Glacier/Render/ERenderFormat.h"
#include "Glacier/Render/SRenderFormatDesc.h"
#include "Glacier/Render/ERenderResourceMipInterpolation.h"

#include "Resources/Resource.h"

class Texture : public Resource
{
public:
    ~Texture() override;
    void Deserialize() override;
    void Export(const std::string& outputPath, const std::string& exportOption) override;
    ZTextureMap::SMipLevel GetMipLevel(unsigned int level);
    DXGI_FORMAT GetDXGIFormat();
    static DXGI_FORMAT GetDXGIFormat(const ERenderFormat renderFormat);
    static ERenderFormat GetRenderFormat(const DXGI_FORMAT dxgiFormat);
    static DirectX::TEX_DIMENSION GetTexDimension(const ZTextureMap::EDimensions dimension);
    static void GetRenderFormatDescription(SRenderFormatDesc* pDesc, const ERenderFormat& eFormat);
    DirectX::TexMetadata GenerateTexMetadata();
    void ConvertR8G8ToB8G8R8A8();
    void ConvertB8G8R8A8ToR8G8();
    void ConvertTextureToDDS(const std::string& outputFilePath);
    void ConvertTextureToDDS(DirectX::Blob& blob, unsigned short& width, unsigned short& height);
    void ConvertTextureToDDS(DirectX::Blob* blob);
    void ConvertTextureToDDS(const std::wstring& outputFilePath);
    void ConvertTextureToTGA(const std::string& outputFilePath);
    void ConvertTextureToTGA(const std::wstring& outputFilePath);
    void ConvertTextureToBMP(const std::string& outputFilePath);
    void ConvertTextureToBMP(const std::wstring& outputFilePath);
    void ConvertTextureToPNG(const std::string& outputFilePath);
    void ConvertTextureToPNG(DirectX::Blob* blob);
    void ConvertTextureToPNG(const std::wstring& outputFilePath);
    void ConvertTextureToJPG(DirectX::Blob* blob);
    static ZTextureMap::STextureMapHeader GetTextureMapHeader(void* data, const unsigned int dataSize);
    static std::string ConvertTextureFlagsToString(const ZTextureMap::ETextureFlags flags);
    static std::string ConvertRenderFormatToString(const ERenderFormat renderFormat);
    static std::string ConvertInterpretAsToString(const ZTextureMap::EInterpretAs interpretAs);
    static std::string ConvertDimensionsToString(const ZTextureMap::EDimensions dimensions);
    static std::string ConvertRenderResourceMipInterpolationToString(const ERenderResourceMipInterpolation renderResourceMipInterpolation);
    static std::string GetErrorMessage(HRESULT hresult);
    static void LoadTextureFromMemory(void* data, unsigned int width, unsigned int height, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** shaderResourceView);

private:
    ZTextureMap::STextureMapHeader textureMapHeader;
    unsigned char* data;
    DirectX::TexMetadata texMetadata;
    DirectX::ScratchImage scratchImage;
};
