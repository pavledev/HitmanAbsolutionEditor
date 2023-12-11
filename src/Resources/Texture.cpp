#include <format>
#include <wincodec.h>

#include "Resources/Texture.h"
#include "Logger.h"
#include "IO/BinaryWriter.h"

// HRESULT_FROM_WIN32(ERROR_ARITHMETIC_OVERFLOW)
#define HRESULT_E_ARITHMETIC_OVERFLOW static_cast<HRESULT>(0x80070216L)

// HRESULT_FROM_WIN32(ERROR_NOT_SUPPORTED)
#define HRESULT_E_NOT_SUPPORTED static_cast<HRESULT>(0x80070032L)

// HRESULT_FROM_WIN32(ERROR_HANDLE_EOF)
#define HRESULT_E_HANDLE_EOF static_cast<HRESULT>(0x80070026L)

// HRESULT_FROM_WIN32(ERROR_INVALID_DATA)
#define HRESULT_E_INVALID_DATA static_cast<HRESULT>(0x8007000DL)

// HRESULT_FROM_WIN32(ERROR_FILE_TOO_LARGE)
#define HRESULT_E_FILE_TOO_LARGE static_cast<HRESULT>(0x800700DFL)

// HRESULT_FROM_WIN32(ERROR_CANNOT_MAKE)
#define HRESULT_E_CANNOT_MAKE static_cast<HRESULT>(0x80070052L)

// HRESULT_FROM_WIN32(ERROR_INSUFFICIENT_BUFFER)
#ifndef E_NOT_SUFFICIENT_BUFFER
#define E_NOT_SUFFICIENT_BUFFER static_cast<HRESULT>(0x8007007AL)
#endif

Texture::~Texture()
{
    if (data)
    {
        delete[] data;
    }
}

void Texture::Deserialize(const std::string& texPath)
{
    BinaryReader binaryReader = BinaryReader(texPath);

    Deserialize(binaryReader);
}

void Texture::Deserialize(void* data, const unsigned int dataSize)
{
    BinaryReader binaryReader = BinaryReader(data, dataSize);

    Deserialize(binaryReader);
}

void Texture::Deserialize(BinaryReader& binaryReader)
{
    textureMapHeader = binaryReader.Read<ZTextureMap::STextureMapHeader>();
    data = binaryReader.Read<unsigned char>(binaryReader.GetSize() - binaryReader.GetPosition());
    texMetadata = GenerateTexMetadata();

    if (static_cast<ERenderFormat>(textureMapHeader.nFormat) == ERenderFormat::RENDER_FORMAT_R8G8_UNORM)
    {
        ConvertR8G8ToB8G8R8A8();
    }

    scratchImage.Initialize(texMetadata);

    memcpy(scratchImage.GetPixels(), data, scratchImage.GetPixelsSize());
}

ZTextureMap::SMipLevel Texture::GetMipLevel(unsigned int level)
{
    ZTextureMap::SMipLevel mipLevel;
    SRenderFormatDesc pDesc;
    ERenderFormat eFormat = static_cast<ERenderFormat>(textureMapHeader.nFormat);

    mipLevel.nWidth = textureMapHeader.nWidth;
    mipLevel.nHeight = textureMapHeader.nHeight;

    GetRenderFormatDescription(&pDesc, eFormat);

    mipLevel.nSizeInBytes = pDesc.nBytesPerBlock *
        ((pDesc.nBlockWidth + mipLevel.nWidth - 1) /
            pDesc.nBlockWidth *
            ((pDesc.nBlockHeight + mipLevel.nHeight - 1) /
                pDesc.nBlockHeight));

    unsigned int offset = 0;

    if (level)
    {
        int i = 0;

        do
        {
            unsigned int width = mipLevel.nWidth + 1;
            unsigned int height = mipLevel.nHeight + 1;

            offset += mipLevel.nSizeInBytes;
            width >>= 1;
            height >>= 1;
            mipLevel.nWidth = width;
            mipLevel.nHeight = height;

            GetRenderFormatDescription(&pDesc, eFormat);

            i = level-- == 1;

            mipLevel.nSizeInBytes = pDesc.nBytesPerBlock *
                ((pDesc.nBlockWidth + width - 1) /
                    pDesc.nBlockWidth *
                    ((pDesc.nBlockHeight + height - 1) /
                        pDesc.nBlockHeight));
        }
        while (!i);
    }

    mipLevel.pData = &data[offset];

    return mipLevel;
}

DXGI_FORMAT Texture::GetDXGIFormat(const ERenderFormat renderFormat)
{
    DXGI_FORMAT result;

    switch (renderFormat)
    {
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_TYPELESS:
            result = DXGI_FORMAT_R32G32B32A32_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_FLOAT:
            result = DXGI_FORMAT_R32G32B32A32_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_UINT:
            result = DXGI_FORMAT_R32G32B32A32_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_SINT:
            result = DXGI_FORMAT_R32G32B32A32_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32_TYPELESS:
            result = DXGI_FORMAT_R32G32B32_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32_FLOAT:
            result = DXGI_FORMAT_R32G32B32_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32_UINT:
            result = DXGI_FORMAT_R32G32B32_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32_SINT:
            result = DXGI_FORMAT_R32G32B32_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_TYPELESS:
            result = DXGI_FORMAT_R16G16B16A16_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_FLOAT:
            result = DXGI_FORMAT_R16G16B16A16_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_UNORM:
            result = DXGI_FORMAT_R16G16B16A16_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_UINT:
            result = DXGI_FORMAT_R16G16B16A16_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_SNORM:
            result = DXGI_FORMAT_R16G16B16A16_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_SINT:
            result = DXGI_FORMAT_R16G16B16A16_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32_TYPELESS:
            result = DXGI_FORMAT_R32G32_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32_FLOAT:
            result = DXGI_FORMAT_R32G32_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32_UINT:
            result = DXGI_FORMAT_R32G32_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G32_SINT:
            result = DXGI_FORMAT_R32G32_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32G8X24_TYPELESS:
            result = DXGI_FORMAT_R32G8X24_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_D32_FLOAT_S8X24_UINT:
            result = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            result = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_X32_TYPELESS_G8X24_UINT:
            result = DXGI_FORMAT_X32_TYPELESS_G8X24_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_TYPELESS:
            result = DXGI_FORMAT_R10G10B10A2_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_UNORM:
            result = DXGI_FORMAT_R10G10B10A2_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_UINT:
            result = DXGI_FORMAT_R10G10B10A2_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R11G11B10_FLOAT:
            result = DXGI_FORMAT_R11G11B10_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_TYPELESS:
            result = DXGI_FORMAT_R8G8B8A8_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM:
            result = DXGI_FORMAT_R8G8B8A8_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM_SRGB:
            result = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UINT:
            result = DXGI_FORMAT_R8G8B8A8_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_SNORM:
            result = DXGI_FORMAT_R8G8B8A8_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_SINT:
            result = DXGI_FORMAT_R8G8B8A8_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16_TYPELESS:
            result = DXGI_FORMAT_R16G16_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16_FLOAT:
            result = DXGI_FORMAT_R16G16_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16_UNORM:
            result = DXGI_FORMAT_R16G16_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16_UINT:
            result = DXGI_FORMAT_R16G16_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16_SNORM:
            result = DXGI_FORMAT_R16G16_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16G16_SINT:
            result = DXGI_FORMAT_R16G16_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32_TYPELESS:
            result = DXGI_FORMAT_R32_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_D32_FLOAT:
            result = DXGI_FORMAT_D32_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32_FLOAT:
            result = DXGI_FORMAT_R32_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32_UINT:
        case ERenderFormat::RENDER_FORMAT_INDEX_32:
            result = DXGI_FORMAT_R32_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R32_SINT:
            result = DXGI_FORMAT_R32_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R24G8_TYPELESS:
            result = DXGI_FORMAT_R24G8_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_D24_UNORM_S8_UINT:
            result = DXGI_FORMAT_D24_UNORM_S8_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R24_UNORM_X8_TYPELESS:
            result = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_X24_TYPELESS_G8_UINT:
            result = DXGI_FORMAT_X24_TYPELESS_G8_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R9G9B9E5_SHAREDEXP:
            result = DXGI_FORMAT_R9G9B9E5_SHAREDEXP;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_B8G8_UNORM:
            result = DXGI_FORMAT_R8G8_B8G8_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_G8R8_G8B8_UNORM:
            result = DXGI_FORMAT_G8R8_G8B8_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_TYPELESS:
            result = DXGI_FORMAT_R8G8_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_UNORM:
            result = DXGI_FORMAT_R8G8_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_UINT:
            result = DXGI_FORMAT_R8G8_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_SNORM:
            result = DXGI_FORMAT_R8G8_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_SINT:
            result = DXGI_FORMAT_R8G8_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16_TYPELESS:
            result = DXGI_FORMAT_R16_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R16_FLOAT:
            result = DXGI_FORMAT_R16_FLOAT;

            break;
        case ERenderFormat::RENDER_FORMAT_D16_UNORM:
            result = DXGI_FORMAT_D16_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16_UNORM:
            result = DXGI_FORMAT_R16_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16_UINT:
        case ERenderFormat::RENDER_FORMAT_INDEX_16:
            result = DXGI_FORMAT_R16_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R16_SNORM:
            result = DXGI_FORMAT_R16_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R16_SINT:
            result = DXGI_FORMAT_R16_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_B5G6R5_UNORM:
            result = DXGI_FORMAT_B5G6R5_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_B5G5R5A1_UNORM:
            result = DXGI_FORMAT_B5G5R5A1_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8_TYPELESS:
            result = DXGI_FORMAT_R8_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_R8_UNORM:
            result = DXGI_FORMAT_R8_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8_UINT:
            result = DXGI_FORMAT_R8_UINT;

            break;
        case ERenderFormat::RENDER_FORMAT_R8_SNORM:
            result = DXGI_FORMAT_R8_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R8_SINT:
            result = DXGI_FORMAT_R8_SINT;

            break;
        case ERenderFormat::RENDER_FORMAT_A8_UNORM:
            result = DXGI_FORMAT_A8_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_R1_UNORM:
            result = DXGI_FORMAT_R1_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC1_TYPELESS:
            result = DXGI_FORMAT_BC1_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_BC1_UNORM:
            result = DXGI_FORMAT_BC1_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC1_UNORM_SRGB:
            result = DXGI_FORMAT_BC1_UNORM_SRGB;

            break;
        case ERenderFormat::RENDER_FORMAT_BC2_TYPELESS:
            result = DXGI_FORMAT_BC2_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_BC2_UNORM:
            result = DXGI_FORMAT_BC2_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC2_UNORM_SRGB:
            result = DXGI_FORMAT_BC2_UNORM_SRGB;

            break;
        case ERenderFormat::RENDER_FORMAT_BC3_TYPELESS:
            result = DXGI_FORMAT_BC3_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_BC3_UNORM:
            result = DXGI_FORMAT_BC3_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC3_UNORM_SRGB:
            result = DXGI_FORMAT_BC3_UNORM_SRGB;

            break;
        case ERenderFormat::RENDER_FORMAT_BC4_TYPELESS:
            result = DXGI_FORMAT_BC4_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_BC4_UNORM:
            result = DXGI_FORMAT_BC4_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC4_SNORM:
            result = DXGI_FORMAT_BC4_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC5_TYPELESS:
            result = DXGI_FORMAT_BC5_TYPELESS;

            break;
        case ERenderFormat::RENDER_FORMAT_BC5_UNORM:
            result = DXGI_FORMAT_BC5_UNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_BC5_SNORM:
            result = DXGI_FORMAT_BC5_SNORM;

            break;
        case ERenderFormat::RENDER_FORMAT_LE_X2R10G10B10_UNORM:
            Logger::GetInstance().Log(Logger::Level::Error, "LE_X2R10G10B10 texture type isn't supported!");
        case ERenderFormat::RENDER_FORMAT_LE_X8R8G8B8_UNORM:
            Logger::GetInstance().Log(Logger::Level::Error, "LE_X8R8G8B8 texture type isn't supported!");
        case ERenderFormat::RENDER_FORMAT_X16Y16Z16_SNORM:
            Logger::GetInstance().Log(Logger::Level::Error, "X16Y16Z16 texture type isn't supported!");
    }

    return result;
}

ERenderFormat Texture::GetRenderFormat(const DXGI_FORMAT dxgiFormat)
{
    ERenderFormat result;

    switch (dxgiFormat)
    {
        case DXGI_FORMAT_R32G32B32A32_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32A32_TYPELESS;

            break;
        case DXGI_FORMAT_R32G32B32A32_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32A32_FLOAT;

            break;
        case DXGI_FORMAT_R32G32B32A32_UINT:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32A32_UINT;

            break;
        case DXGI_FORMAT_R32G32B32A32_SINT:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32A32_SINT;

            break;
        case DXGI_FORMAT_R32G32B32_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32_TYPELESS;

            break;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32_FLOAT;

            break;
        case DXGI_FORMAT_R32G32B32_UINT:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32_UINT;

            break;
        case DXGI_FORMAT_R32G32B32_SINT:
            result = ERenderFormat::RENDER_FORMAT_R32G32B32_SINT;

            break;
        case DXGI_FORMAT_R16G16B16A16_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R16G16B16A16_TYPELESS;

            break;
        case DXGI_FORMAT_R16G16B16A16_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R16G16B16A16_FLOAT;

            break;
        case DXGI_FORMAT_R16G16B16A16_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R16G16B16A16_UNORM;

            break;
        case DXGI_FORMAT_R16G16B16A16_UINT:
            result = ERenderFormat::RENDER_FORMAT_R16G16B16A16_UINT;

            break;
        case DXGI_FORMAT_R16G16B16A16_SNORM:
            result = ERenderFormat::RENDER_FORMAT_R16G16B16A16_SNORM;

            break;
        case DXGI_FORMAT_R16G16B16A16_SINT:
            result = ERenderFormat::RENDER_FORMAT_R16G16B16A16_SINT;

            break;
        case DXGI_FORMAT_R32G32_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R32G32_TYPELESS;

            break;
        case DXGI_FORMAT_R32G32_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R32G32_FLOAT;

            break;
        case DXGI_FORMAT_R32G32_UINT:
            result = ERenderFormat::RENDER_FORMAT_R32G32_UINT;

            break;
        case DXGI_FORMAT_R32G32_SINT:
            result = ERenderFormat::RENDER_FORMAT_R32G32_SINT;

            break;
        case DXGI_FORMAT_R32G8X24_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R32G8X24_TYPELESS;

            break;
        case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
            result = ERenderFormat::RENDER_FORMAT_D32_FLOAT_S8X24_UINT;

            break;
        case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS;

            break;
        case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
            result = ERenderFormat::RENDER_FORMAT_X32_TYPELESS_G8X24_UINT;

            break;
        case DXGI_FORMAT_R10G10B10A2_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R10G10B10A2_TYPELESS;

            break;
        case DXGI_FORMAT_R10G10B10A2_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R10G10B10A2_UNORM;

            break;
        case DXGI_FORMAT_R10G10B10A2_UINT:
            result = ERenderFormat::RENDER_FORMAT_R10G10B10A2_UINT;

            break;
        case DXGI_FORMAT_R11G11B10_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R11G11B10_FLOAT;

            break;
        case DXGI_FORMAT_R8G8B8A8_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R8G8B8A8_TYPELESS;

            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM;

            break;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            result = ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM_SRGB;

            break;
        case DXGI_FORMAT_R8G8B8A8_UINT:
            result = ERenderFormat::RENDER_FORMAT_R8G8B8A8_UINT;

            break;
        case DXGI_FORMAT_R8G8B8A8_SNORM:
            result = ERenderFormat::RENDER_FORMAT_R8G8B8A8_SNORM;

            break;
        case DXGI_FORMAT_R8G8B8A8_SINT:
            result = ERenderFormat::RENDER_FORMAT_R8G8B8A8_SINT;

            break;
        case DXGI_FORMAT_R16G16_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R16G16_TYPELESS;

            break;
        case DXGI_FORMAT_R16G16_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R16G16_FLOAT;

            break;
        case DXGI_FORMAT_R16G16_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R16G16_UNORM;

            break;
        case DXGI_FORMAT_R16G16_UINT:
            result = ERenderFormat::RENDER_FORMAT_R16G16_UINT;

            break;
        case DXGI_FORMAT_R16G16_SNORM:
            result = ERenderFormat::RENDER_FORMAT_R16G16_SNORM;

            break;
        case DXGI_FORMAT_R16G16_SINT:
            result = ERenderFormat::RENDER_FORMAT_R16G16_SINT;

            break;
        case DXGI_FORMAT_R32_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R32_TYPELESS;

            break;
        case DXGI_FORMAT_D32_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_D32_FLOAT;

            break;
        case DXGI_FORMAT_R32_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R32_FLOAT;

            break;
        case DXGI_FORMAT_R32_UINT:
            result = ERenderFormat::RENDER_FORMAT_R32_UINT;

            break;
        case DXGI_FORMAT_R32_SINT:
            result = ERenderFormat::RENDER_FORMAT_R32_SINT;

            break;
        case DXGI_FORMAT_R24G8_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R24G8_TYPELESS;

            break;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            result = ERenderFormat::RENDER_FORMAT_D24_UNORM_S8_UINT;

            break;
        case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R24_UNORM_X8_TYPELESS;

            break;
        case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
            result = ERenderFormat::RENDER_FORMAT_X24_TYPELESS_G8_UINT;

            break;
        case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
            result = ERenderFormat::RENDER_FORMAT_R9G9B9E5_SHAREDEXP;

            break;
        case DXGI_FORMAT_R8G8_B8G8_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R8G8_B8G8_UNORM;

            break;
        case DXGI_FORMAT_G8R8_G8B8_UNORM:
            result = ERenderFormat::RENDER_FORMAT_G8R8_G8B8_UNORM;

            break;
        case DXGI_FORMAT_R8G8_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R8G8_TYPELESS;

            break;
        case DXGI_FORMAT_R8G8_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R8G8_UNORM;

            break;
        case DXGI_FORMAT_R8G8_UINT:
            result = ERenderFormat::RENDER_FORMAT_R8G8_UINT;

            break;
        case DXGI_FORMAT_R8G8_SNORM:
            result = ERenderFormat::RENDER_FORMAT_R8G8_SNORM;

            break;
        case DXGI_FORMAT_R8G8_SINT:
            result = ERenderFormat::RENDER_FORMAT_R8G8_SINT;

            break;
        case DXGI_FORMAT_R16_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R16_TYPELESS;

            break;
        case DXGI_FORMAT_R16_FLOAT:
            result = ERenderFormat::RENDER_FORMAT_R16_FLOAT;

            break;
        case DXGI_FORMAT_D16_UNORM:
            result = ERenderFormat::RENDER_FORMAT_D16_UNORM;

            break;
        case DXGI_FORMAT_R16_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R16_UNORM;

            break;
        case DXGI_FORMAT_R16_UINT:
            result = ERenderFormat::RENDER_FORMAT_R16_UINT;

            break;
        case DXGI_FORMAT_R16_SNORM:
            result = ERenderFormat::RENDER_FORMAT_R16_SNORM;

            break;
        case DXGI_FORMAT_R16_SINT:
            result = ERenderFormat::RENDER_FORMAT_R16_SINT;

            break;
        case DXGI_FORMAT_B5G6R5_UNORM:
            result = ERenderFormat::RENDER_FORMAT_B5G6R5_UNORM;

            break;
        case DXGI_FORMAT_B5G5R5A1_UNORM:
            result = ERenderFormat::RENDER_FORMAT_B5G5R5A1_UNORM;

            break;
        case DXGI_FORMAT_R8_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_R8_TYPELESS;

            break;
        case DXGI_FORMAT_R8_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R8_UNORM;

            break;
        case DXGI_FORMAT_R8_UINT:
            result = ERenderFormat::RENDER_FORMAT_R8_UINT;

            break;
        case DXGI_FORMAT_R8_SNORM:
            result = ERenderFormat::RENDER_FORMAT_R8_SNORM;

            break;
        case DXGI_FORMAT_R8_SINT:
            result = ERenderFormat::RENDER_FORMAT_R8_SINT;

            break;
        case DXGI_FORMAT_A8_UNORM:
            result = ERenderFormat::RENDER_FORMAT_A8_UNORM;

            break;
        case DXGI_FORMAT_R1_UNORM:
            result = ERenderFormat::RENDER_FORMAT_R1_UNORM;

            break;
        case DXGI_FORMAT_BC1_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_BC1_TYPELESS;

            break;
        case DXGI_FORMAT_BC1_UNORM:
            result = ERenderFormat::RENDER_FORMAT_BC1_UNORM;

            break;
        case DXGI_FORMAT_BC1_UNORM_SRGB:
            result = ERenderFormat::RENDER_FORMAT_BC1_UNORM_SRGB;

            break;
        case DXGI_FORMAT_BC2_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_BC2_TYPELESS;

            break;
        case DXGI_FORMAT_BC2_UNORM:
            result = ERenderFormat::RENDER_FORMAT_BC2_UNORM;

            break;
        case DXGI_FORMAT_BC2_UNORM_SRGB:
            result = ERenderFormat::RENDER_FORMAT_BC2_UNORM_SRGB;

            break;
        case DXGI_FORMAT_BC3_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_BC3_TYPELESS;

            break;
        case DXGI_FORMAT_BC3_UNORM:
            result = ERenderFormat::RENDER_FORMAT_BC3_UNORM;

            break;
        case DXGI_FORMAT_BC3_UNORM_SRGB:
            result = ERenderFormat::RENDER_FORMAT_BC3_UNORM_SRGB;

            break;
        case DXGI_FORMAT_BC4_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_BC4_TYPELESS;

            break;
        case DXGI_FORMAT_BC4_UNORM:
            result = ERenderFormat::RENDER_FORMAT_BC4_UNORM;

            break;
        case DXGI_FORMAT_BC4_SNORM:
            result = ERenderFormat::RENDER_FORMAT_BC4_SNORM;

            break;
        case DXGI_FORMAT_BC5_TYPELESS:
            result = ERenderFormat::RENDER_FORMAT_BC5_TYPELESS;

            break;
        case DXGI_FORMAT_BC5_UNORM:
            result = ERenderFormat::RENDER_FORMAT_BC5_UNORM;

            break;
        case DXGI_FORMAT_BC5_SNORM:
            result = ERenderFormat::RENDER_FORMAT_BC5_SNORM;

            break;
    }

    return result;
}

DirectX::TEX_DIMENSION Texture::GetTexDimension(const ZTextureMap::EDimensions dimension)
{
    DirectX::TEX_DIMENSION result;

    switch (dimension)
    {
        case ZTextureMap::EDimensions::DIMENSIONS_2D:
        case ZTextureMap::EDimensions::DIMENSIONS_CUBE:
            result = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;

            break;
        case ZTextureMap::EDimensions::DIMENSIONS_VOLUME:
            result = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE3D;

            break;
    }

    return result;
}

void Texture::GetRenderFormatDescription(SRenderFormatDesc* pDesc, const ERenderFormat& eFormat)
{
    switch (eFormat)
    {
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_UINT:
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_SINT:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 16;
            break;
        case ERenderFormat::RENDER_FORMAT_R32G32B32_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R32G32B32_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R32G32B32_UINT:
        case ERenderFormat::RENDER_FORMAT_R32G32B32_SINT:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 12;
            break;
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_UNORM:
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_UINT:
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_SNORM:
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_SINT:
        case ERenderFormat::RENDER_FORMAT_R32G32_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R32G32_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R32G32_UINT:
        case ERenderFormat::RENDER_FORMAT_R32G32_SINT:
        case ERenderFormat::RENDER_FORMAT_R32G8X24_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_D32_FLOAT_S8X24_UINT:
        case ERenderFormat::RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_X32_TYPELESS_G8X24_UINT:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 8;
            break;
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_UNORM:
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_UINT:
        case ERenderFormat::RENDER_FORMAT_R11G11B10_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM:
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM_SRGB:
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UINT:
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_SNORM:
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_SINT:
        case ERenderFormat::RENDER_FORMAT_R16G16_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R16G16_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R16G16_UNORM:
        case ERenderFormat::RENDER_FORMAT_R16G16_UINT:
        case ERenderFormat::RENDER_FORMAT_R16G16_SNORM:
        case ERenderFormat::RENDER_FORMAT_R16G16_SINT:
        case ERenderFormat::RENDER_FORMAT_R32_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_D32_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R32_FLOAT:
        case ERenderFormat::RENDER_FORMAT_R32_UINT:
        case ERenderFormat::RENDER_FORMAT_R32_SINT:
        case ERenderFormat::RENDER_FORMAT_R24G8_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_D24_UNORM_S8_UINT:
        case ERenderFormat::RENDER_FORMAT_R24_UNORM_X8_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_X24_TYPELESS_G8_UINT:
        case ERenderFormat::RENDER_FORMAT_R9G9B9E5_SHAREDEXP:
        case ERenderFormat::RENDER_FORMAT_R8G8_B8G8_UNORM:
        case ERenderFormat::RENDER_FORMAT_G8R8_G8B8_UNORM:
        case ERenderFormat::RENDER_FORMAT_INDEX_32:
        case ERenderFormat::RENDER_FORMAT_LE_X8R8G8B8_UNORM:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 4;
            break;
        case ERenderFormat::RENDER_FORMAT_R8G8_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R8G8_UNORM:
        case ERenderFormat::RENDER_FORMAT_R8G8_UINT:
        case ERenderFormat::RENDER_FORMAT_R8G8_SNORM:
        case ERenderFormat::RENDER_FORMAT_R8G8_SINT:
        case ERenderFormat::RENDER_FORMAT_R16_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R16_FLOAT:
        case ERenderFormat::RENDER_FORMAT_D16_UNORM:
        case ERenderFormat::RENDER_FORMAT_R16_UNORM:
        case ERenderFormat::RENDER_FORMAT_R16_UINT:
        case ERenderFormat::RENDER_FORMAT_R16_SNORM:
        case ERenderFormat::RENDER_FORMAT_R16_SINT:
        case ERenderFormat::RENDER_FORMAT_B5G6R5_UNORM:
        case ERenderFormat::RENDER_FORMAT_B5G5R5A1_UNORM:
        case ERenderFormat::RENDER_FORMAT_INDEX_16:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 2;
            break;
        case ERenderFormat::RENDER_FORMAT_R8_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_R8_UNORM:
        case ERenderFormat::RENDER_FORMAT_R8_UINT:
        case ERenderFormat::RENDER_FORMAT_R8_SNORM:
        case ERenderFormat::RENDER_FORMAT_R8_SINT:
        case ERenderFormat::RENDER_FORMAT_A8_UNORM:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 1;
            break;
        case ERenderFormat::RENDER_FORMAT_R1_UNORM:
            pDesc->nBlockWidth = 1;
            pDesc->nBlockHeight = 1;
            pDesc->nBytesPerBlock = 0;
            break;
        case ERenderFormat::RENDER_FORMAT_BC1_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_BC1_UNORM:
        case ERenderFormat::RENDER_FORMAT_BC1_UNORM_SRGB:
        case ERenderFormat::RENDER_FORMAT_BC4_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_BC4_UNORM:
        case ERenderFormat::RENDER_FORMAT_BC4_SNORM:
            pDesc->nBlockWidth = 4;
            pDesc->nBlockHeight = 4;
            pDesc->nBytesPerBlock = 8;
            break;
        case ERenderFormat::RENDER_FORMAT_BC2_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_BC2_UNORM:
        case ERenderFormat::RENDER_FORMAT_BC2_UNORM_SRGB:
        case ERenderFormat::RENDER_FORMAT_BC3_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_BC3_UNORM:
        case ERenderFormat::RENDER_FORMAT_BC3_UNORM_SRGB:
        case ERenderFormat::RENDER_FORMAT_BC5_TYPELESS:
        case ERenderFormat::RENDER_FORMAT_BC5_UNORM:
        case ERenderFormat::RENDER_FORMAT_BC5_SNORM:
            pDesc->nBlockWidth = 4;
            pDesc->nBlockHeight = 4;
            pDesc->nBytesPerBlock = 16;
            break;
    }
}

DirectX::TexMetadata Texture::GenerateTexMetadata()
{
    unsigned int width = textureMapHeader.nWidth;
    unsigned int height = textureMapHeader.nHeight;
    bool isCubeMap = static_cast<ZTextureMap::EDimensions>(textureMapHeader.nDimensions) == ZTextureMap::EDimensions::DIMENSIONS_CUBE;
    unsigned int arraySize = isCubeMap ? 6 : 1;
    int mipMapLevels = textureMapHeader.nNumMipLevels;
    unsigned int miscFlags = isCubeMap ? DirectX::TEX_MISC_FLAG::TEX_MISC_TEXTURECUBE : 0;
    const DXGI_FORMAT dxgiFormat = GetDXGIFormat(static_cast<ERenderFormat>(textureMapHeader.nFormat));
    DirectX::TEX_DIMENSION dimension = GetTexDimension(static_cast<ZTextureMap::EDimensions>(textureMapHeader.nDimensions));

    return DirectX::TexMetadata(width, height, 1, arraySize, mipMapLevels, miscFlags, 0, dxgiFormat, dimension);
}

void Texture::ConvertR8G8ToB8G8R8A8()
{
    void* data = nullptr;
    unsigned int currentSize = 0;

    for (int level = 0; level < textureMapHeader.nNumMipLevels; ++level)
    {
        ZTextureMap::SMipLevel mipLevel = GetMipLevel(level);
        BinaryReader binaryReader = BinaryReader(mipLevel.pData, mipLevel.nSizeInBytes);
        BinaryWriter binaryWriter = BinaryWriter(mipLevel.nSizeInBytes * 2);

        for (unsigned int i = 0; i < mipLevel.nSizeInBytes / 2; ++i)
        {
            unsigned char red = binaryReader.Read<unsigned char>();
            unsigned char green = binaryReader.Read<unsigned char>();

            binaryWriter.Write<unsigned char>(255);
            binaryWriter.Write<unsigned char>(green);
            binaryWriter.Write<unsigned char>(red);
            binaryWriter.Write<unsigned char>(255);
        }

        if (data)
        {
            data = realloc(data, currentSize + binaryWriter.GetPosition());
        }
        else
        {
            data = malloc(binaryWriter.GetPosition());
        }

        uintptr_t pointer = reinterpret_cast<uintptr_t>(data) + currentSize;

        memcpy(reinterpret_cast<void*>(pointer), binaryWriter.GetBuffer(), binaryWriter.GetPosition());
        currentSize += binaryWriter.GetPosition();
    }

    texMetadata.format = DXGI_FORMAT::DXGI_FORMAT_B8G8R8A8_UNORM;
    this->data = reinterpret_cast<unsigned char*>(data);
}

void Texture::ConvertB8G8R8A8ToR8G8()
{
    void* data = nullptr;
    unsigned int currentSize = 0;

    for (int level = 0; level < textureMapHeader.nNumMipLevels; ++level)
    {
        ZTextureMap::SMipLevel mipLevel = GetMipLevel(level);
        BinaryReader binaryReader = BinaryReader(mipLevel.pData, mipLevel.nSizeInBytes);
        BinaryWriter binaryWriter = BinaryWriter(mipLevel.nSizeInBytes * 2);

        for (unsigned int i = 0; i < mipLevel.nSizeInBytes / 4; ++i)
        {
            unsigned char blue = binaryReader.Read<unsigned char>();
            unsigned char green = binaryReader.Read<unsigned char>();
            unsigned char red = binaryReader.Read<unsigned char>();
            unsigned char alpha = binaryReader.Read<unsigned char>();

            binaryWriter.Write<unsigned char>(red);
            binaryWriter.Write<unsigned char>(green);
        }

        if (data)
        {
            data = realloc(data, currentSize + binaryWriter.GetPosition());
        }
        else
        {
            data = malloc(binaryWriter.GetPosition());
        }

        uintptr_t pointer = reinterpret_cast<uintptr_t>(data) + currentSize;

        memcpy(reinterpret_cast<void*>(pointer), binaryWriter.GetBuffer(), binaryWriter.GetPosition());
        currentSize += binaryWriter.GetPosition();
    }

    texMetadata.format = DXGI_FORMAT::DXGI_FORMAT_R8G8_UNORM;
    this->data = reinterpret_cast<unsigned char*>(data);
}

void Texture::ConvertTextureToDDS(const std::string& outputFilePath)
{
	std::wstring outputFilePath2 = std::wstring(outputFilePath.begin(), outputFilePath.end());

	ConvertTextureToDDS(outputFilePath2);
}

void Texture::ConvertTextureToDDS(DirectX::Blob& blob, unsigned short& width, unsigned short& height)
{
    HRESULT result = DirectX::SaveToDDSMemory(*scratchImage.GetImages(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, blob);

    width = textureMapHeader.nWidth;
    height = textureMapHeader.nHeight;

    if (!SUCCEEDED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to DDS. {}", GetErrorMessage(result));
    }
}

void Texture::ConvertTextureToDDS(DirectX::Blob* blob)
{
    HRESULT result = DirectX::SaveToDDSMemory(*scratchImage.GetImages(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, *blob);

    if (!SUCCEEDED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to DDS. {}", GetErrorMessage(result));
    }
}

void Texture::ConvertTextureToDDS(const std::wstring& outputFilePath)
{
    HRESULT result = DirectX::SaveToDDSFile(*scratchImage.GetImages(), DirectX::DDS_FLAGS::DDS_FLAGS_NONE, outputFilePath.c_str());

    if (SUCCEEDED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Info, "Texture Map file successfully converted to DDS.");
    }
    else
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to DDS. {}", GetErrorMessage(result));
    }
}

void Texture::ConvertTextureToTGA(const std::string& outputFilePath)
{
    std::wstring outputFilePath2 = std::wstring(outputFilePath.begin(), outputFilePath.end());

    ConvertTextureToTGA(outputFilePath2);
}

void Texture::ConvertTextureToTGA(const std::wstring& outputFilePath)
{
	const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);
	DirectX::ScratchImage scratchImage2;
	const DirectX::Image* image2;
	HRESULT result;
	DXGI_FORMAT dxgiFormat = texMetadata.format;

	if (DirectX::IsCompressed(dxgiFormat))
	{
		result = DirectX::Decompress(*image, DXGI_FORMAT_UNKNOWN, scratchImage2);

		if (FAILED(result))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to decompress texture.");

			return;
		}

		image2 = scratchImage2.GetImage(0, 0, 0);
	}
	else
	{
		image2 = scratchImage.GetImage(0, 0, 0);
	}

    result = DirectX::SaveToTGAFile(*image2, outputFilePath.c_str(), &texMetadata);

	if (SUCCEEDED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Info, "Texture Map file successfully converted to TGA.");
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to TGA. {}", GetErrorMessage(result));
	}
}

void Texture::ConvertTextureToBMP(const std::string& outputFilePath)
{
    std::wstring outputFilePath2 = std::wstring(outputFilePath.begin(), outputFilePath.end());

    ConvertTextureToBMP(outputFilePath2);
}

void Texture::ConvertTextureToBMP(const std::wstring& outputFilePath)
{
    const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);
    DirectX::ScratchImage scratchImage2;
    const DirectX::Image* image2;
    HRESULT result;

    DXGI_FORMAT dxgiFormat = texMetadata.format;

    if (DirectX::IsCompressed(dxgiFormat))
    {
        result = DirectX::Decompress(*image, DXGI_FORMAT_UNKNOWN, scratchImage2);

        if (FAILED(result))
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Failed to decompress texture.");

            return;
        }

        image2 = scratchImage2.GetImage(0, 0, 0);
    }
    else
    {
        image2 = scratchImage.GetImage(0, 0, 0);
    }

    result = DirectX::SaveToWICFile(*image2, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GUID_ContainerFormatBmp, outputFilePath.c_str());

    if (SUCCEEDED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Info, "Texture Map file successfully converted to BMP.");
    }
    else
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to BMP. {}", GetErrorMessage(result));
    }
}

void Texture::ConvertTextureToPNG(const std::string& outputFilePath)
{
    std::wstring outputFilePath2 = std::wstring(outputFilePath.begin(), outputFilePath.end());

    ConvertTextureToPNG(outputFilePath2);
}

void Texture::ConvertTextureToPNG(DirectX::Blob* blob)
{
    const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);
    DirectX::ScratchImage scratchImage2;
    const DirectX::Image* image2;
    HRESULT result = 0;

    DXGI_FORMAT dxgiFormat = texMetadata.format;

    if (DirectX::IsCompressed(dxgiFormat))
    {
        result = DirectX::Decompress(*image, DXGI_FORMAT_UNKNOWN, scratchImage2);

        if (FAILED(result))
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Failed to decompress texture.");

            return;
        }

        image2 = scratchImage2.GetImage(0, 0, 0);
    }
    else
    {
        image2 = scratchImage.GetImage(0, 0, 0);
    }

    result = DirectX::SaveToWICMemory(*image2, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GUID_ContainerFormatPng, *blob);

    if (!SUCCEEDED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to PNG. {}", GetErrorMessage(result));
    }
}

void Texture::ConvertTextureToPNG(const std::wstring& outputFilePath)
{
	const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);
	DirectX::ScratchImage scratchImage2;
	const DirectX::Image* image2;
	HRESULT result = 0;

	DXGI_FORMAT dxgiFormat = texMetadata.format;

	if (DirectX::IsCompressed(dxgiFormat))
	{
		result = DirectX::Decompress(*image, DXGI_FORMAT_UNKNOWN, scratchImage2);

		if (FAILED(result))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to decompress texture.");

			return;
		}

		image2 = scratchImage2.GetImage(0, 0, 0);
	}
	else
	{
		image2 = scratchImage.GetImage(0, 0, 0);
	}

    result = DirectX::SaveToWICFile(*image2, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GUID_ContainerFormatPng, outputFilePath.c_str());

	if (!SUCCEEDED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to PNG. {}", GetErrorMessage(result));
	}
}

void Texture::ConvertTextureToJPG(DirectX::Blob* blob)
{
    const DirectX::Image* image = scratchImage.GetImage(0, 0, 0);
    DirectX::ScratchImage scratchImage2;
    const DirectX::Image* image2;
    HRESULT result = 0;

    DXGI_FORMAT dxgiFormat = texMetadata.format;

    if (DirectX::IsCompressed(dxgiFormat))
    {
        result = DirectX::Decompress(*image, DXGI_FORMAT_UNKNOWN, scratchImage2);

        if (FAILED(result))
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Failed to decompress texture.");

            return;
        }

        image2 = scratchImage2.GetImage(0, 0, 0);
    }
    else
    {
        image2 = scratchImage.GetImage(0, 0, 0);
    }

    result = DirectX::SaveToWICMemory(*image2, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, GUID_ContainerFormatJpeg, *blob);

    if (!SUCCEEDED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to convert texture to PNG. {}", GetErrorMessage(result));
    }
}

std::string Texture::GetErrorMessage(HRESULT result)
{
	std::string message;

	switch (result)
	{
	case HRESULT_E_ARITHMETIC_OVERFLOW:
		message = "Arithmetic result exceeded 32 bits.";

		break;
	case HRESULT_E_NOT_SUPPORTED:
		message = "The request is not supported.";

		break;
	case HRESULT_E_HANDLE_EOF:
		message = "Reached the end of the file.";

		break;
	case HRESULT_E_INVALID_DATA:
		message = "The data is invalid.";

		break;
	case HRESULT_E_FILE_TOO_LARGE:
		message = "The file size exceeds the limit allowed and cannot be saved.";

		break;
	case HRESULT_E_CANNOT_MAKE:
		message = "The directory or file cannot be created.";

		break;
	case E_NOT_SUFFICIENT_BUFFER:
		message = "The data area passed to a system call is too small.";

		break;
	default:
		break;
	}

	return message;
}

ZTextureMap::STextureMapHeader Texture::GetTextureMapHeader(void* data, const unsigned int dataSize)
{
    BinaryReader binaryReader = BinaryReader(data, dataSize);

    return binaryReader.Read<ZTextureMap::STextureMapHeader>();
}

std::string Texture::ConvertTextureFlagsToString(const ZTextureMap::ETextureFlags flags)
{
    std::string flags2;

    if ((flags & ZTextureMap::ETextureFlags::TEXF_SWIZZLED) == ZTextureMap::ETextureFlags::TEXF_SWIZZLED)
    {
        flags2 += "Swizzled, ";
    }

    if ((flags & ZTextureMap::ETextureFlags::TEXF_DEFERRED) == ZTextureMap::ETextureFlags::TEXF_DEFERRED)
    {
        flags2 += "Deferred, ";
    }

    if ((flags & ZTextureMap::ETextureFlags::TEXF_MEMORY_READY_XBOX360) == ZTextureMap::ETextureFlags::TEXF_MEMORY_READY_XBOX360)
    {
        flags2 += "Memory Ready XBOX 360, ";
    }

    if ((flags & ZTextureMap::ETextureFlags::TEXF_GAMMA) == ZTextureMap::ETextureFlags::TEXF_GAMMA)
    {
        flags2 += "Gamma, ";
    }

    if ((flags & ZTextureMap::ETextureFlags::TEXF_EMISSIVE) == ZTextureMap::ETextureFlags::TEXF_EMISSIVE)
    {
        flags2 += "Emissive, ";
    }

    if ((flags & ZTextureMap::ETextureFlags::TEXF_DDSC_ENCODED) == ZTextureMap::ETextureFlags::TEXF_DDSC_ENCODED)
    {
        flags2 += "DDSC Encoded";
    }

    if (flags2.length() == 0)
    {
        flags2 = "None";
    }
    else if (flags2.ends_with(", "))
    {
        flags2 = flags2.erase(flags2.length() - 2, 2);
    }

    return flags2;
}

std::string Texture::ConvertRenderFormatToString(const ERenderFormat renderFormat)
{
    switch (renderFormat)
    {
        case ERenderFormat::RENDER_FORMAT_NONE:
            return "NONE";
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_TYPELESS:
            return "R32G32B32A32_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_FLOAT:
            return "R32G32B32A32_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_UINT:
            return "R32G32B32A32_UINT";
        case ERenderFormat::RENDER_FORMAT_R32G32B32A32_SINT:
            return "R32G32B32A32_SINT";
        case ERenderFormat::RENDER_FORMAT_R32G32B32_TYPELESS:
            return "R32G32B32_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R32G32B32_FLOAT:
            return "R32G32B32_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R32G32B32_UINT:
            return "R32G32B32_UINT";
        case ERenderFormat::RENDER_FORMAT_R32G32B32_SINT:
            return "R32G32B32_SINT";
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_TYPELESS:
            return "R16G16B16A16_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_FLOAT:
            return "R16G16B16A16_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_UNORM:
            return "R16G16B16A16_UNORM";
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_UINT:
            return "R16G16B16A16_UINT";
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_SNORM:
            return "R16G16B16A16_SNORM";
        case ERenderFormat::RENDER_FORMAT_R16G16B16A16_SINT:
            return "R16G16B16A16_SINT";
        case ERenderFormat::RENDER_FORMAT_R32G32_TYPELESS:
            return "R32G32_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R32G32_FLOAT:
            return "R32G32_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R32G32_UINT:
            return "R32G32_UINT";
        case ERenderFormat::RENDER_FORMAT_R32G32_SINT:
            return "R32G32_SINT";
        case ERenderFormat::RENDER_FORMAT_R32G8X24_TYPELESS:
            return "R32G8X24_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_D32_FLOAT_S8X24_UINT:
            return "D32_FLOAT_S8X24_UINT";
        case ERenderFormat::RENDER_FORMAT_R32_FLOAT_X8X24_TYPELESS:
            return "R32_FLOAT_X8X24_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_X32_TYPELESS_G8X24_UINT:
            return "X32_TYPELESS_G8X24_UINT";
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_TYPELESS:
            return "R10G10B10A2_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_UNORM:
            return "R10G10B10A2_UNORM";
        case ERenderFormat::RENDER_FORMAT_R10G10B10A2_UINT:
            return "R10G10B10A2_UINT";
        case ERenderFormat::RENDER_FORMAT_R11G11B10_FLOAT:
            return "R11G11B10_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_TYPELESS:
            return "R8G8B8A8_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM:
            return "R8G8B8A8_UNORM";
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UNORM_SRGB:
            return "R8G8B8A8_UNORM_SRGB";
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_UINT:
            return "R8G8B8A8_UINT";
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_SNORM:
            return "R8G8B8A8_SNORM";
        case ERenderFormat::RENDER_FORMAT_R8G8B8A8_SINT:
            return "R8G8B8A8_SINT";
        case ERenderFormat::RENDER_FORMAT_R16G16_TYPELESS:
            return "R16G16_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R16G16_FLOAT:
            return "R16G16_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R16G16_UNORM:
            return "R16G16_UNORM";
        case ERenderFormat::RENDER_FORMAT_R16G16_UINT:
            return "R16G16_UINT";
        case ERenderFormat::RENDER_FORMAT_R16G16_SNORM:
            return "R16G16_SNORM";
        case ERenderFormat::RENDER_FORMAT_R16G16_SINT:
            return "R16G16_SINT";
        case ERenderFormat::RENDER_FORMAT_R32_TYPELESS:
            return "R32_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_D32_FLOAT:
            return "D32_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R32_FLOAT:
            return "R32_FLOAT";
        case ERenderFormat::RENDER_FORMAT_R32_UINT:
            return "R32_UINT";
        case ERenderFormat::RENDER_FORMAT_R32_SINT:
            return "R32_SINT";
        case ERenderFormat::RENDER_FORMAT_R24G8_TYPELESS:
            return "R24G8_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_D24_UNORM_S8_UINT:
            return "D24_UNORM_S8_UINT";
        case ERenderFormat::RENDER_FORMAT_R24_UNORM_X8_TYPELESS:
            return "R24_UNORM_X8_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_X24_TYPELESS_G8_UINT:
            return "X24_TYPELESS_G8_UINT";
        case ERenderFormat::RENDER_FORMAT_R9G9B9E5_SHAREDEXP:
            return "R9G9B9E5_SHAREDEXP";
        case ERenderFormat::RENDER_FORMAT_R8G8_B8G8_UNORM:
            return "R8G8_B8G8_UNORM";
        case ERenderFormat::RENDER_FORMAT_G8R8_G8B8_UNORM:
            return "G8R8_G8B8_UNORM";
        case ERenderFormat::RENDER_FORMAT_R8G8_TYPELESS:
            return "R8G8_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R8G8_UNORM:
            return "R8G8_UNORM";
        case ERenderFormat::RENDER_FORMAT_R8G8_UINT:
            return "R8G8_UINT";
        case ERenderFormat::RENDER_FORMAT_R8G8_SNORM:
            return "R8G8_SNORM";
        case ERenderFormat::RENDER_FORMAT_R8G8_SINT:
            return "R8G8_SINT";
        case ERenderFormat::RENDER_FORMAT_R16_TYPELESS:
            return "R16_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R16_FLOAT:
            return "R16_FLOAT";
        case ERenderFormat::RENDER_FORMAT_D16_UNORM:
            return "D16_UNORM";
        case ERenderFormat::RENDER_FORMAT_R16_UNORM:
            return "R16_UNORM";
        case ERenderFormat::RENDER_FORMAT_R16_UINT:
            return "R16_UINT";
        case ERenderFormat::RENDER_FORMAT_R16_SNORM:
            return "R16_SNORM";
        case ERenderFormat::RENDER_FORMAT_R16_SINT:
            return "R16_SINT";
        case ERenderFormat::RENDER_FORMAT_B5G6R5_UNORM:
            return "B5G6R5_UNORM";
        case ERenderFormat::RENDER_FORMAT_B5G5R5A1_UNORM:
            return "B5G5R5A1_UNORM";
        case ERenderFormat::RENDER_FORMAT_R8_TYPELESS:
            return "R8_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_R8_UNORM:
            return "R8_UNORM";
        case ERenderFormat::RENDER_FORMAT_R8_UINT:
            return "R8_UINT";
        case ERenderFormat::RENDER_FORMAT_R8_SNORM:
            return "R8_SNORM";
        case ERenderFormat::RENDER_FORMAT_R8_SINT:
            return "R8_SINT";
        case ERenderFormat::RENDER_FORMAT_A8_UNORM:
            return "A8_UNORM";
        case ERenderFormat::RENDER_FORMAT_R1_UNORM:
            return "R1_UNORM";
        case ERenderFormat::RENDER_FORMAT_BC1_TYPELESS:
            return "BC1_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_BC1_UNORM:
            return "BC1_UNORM";
        case ERenderFormat::RENDER_FORMAT_BC1_UNORM_SRGB:
            return "BC1_UNORM_SRGB";
        case ERenderFormat::RENDER_FORMAT_BC2_TYPELESS:
            return "BC2_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_BC2_UNORM:
            return "BC2_UNORM";
        case ERenderFormat::RENDER_FORMAT_BC2_UNORM_SRGB:
            return "BC2_UNORM_SRGB";
        case ERenderFormat::RENDER_FORMAT_BC3_TYPELESS:
            return "BC3_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_BC3_UNORM:
            return "BC3_UNORM";
        case ERenderFormat::RENDER_FORMAT_BC3_UNORM_SRGB:
            return "BC3_UNORM_SRGB";
        case ERenderFormat::RENDER_FORMAT_BC4_TYPELESS:
            return "BC4_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_BC4_UNORM:
            return "BC4_UNORM";
        case ERenderFormat::RENDER_FORMAT_BC4_SNORM:
            return "BC4_SNORM";
        case ERenderFormat::RENDER_FORMAT_BC5_TYPELESS:
            return "BC5_TYPELESS";
        case ERenderFormat::RENDER_FORMAT_BC5_UNORM:
            return "BC5_UNORM";
        case ERenderFormat::RENDER_FORMAT_BC5_SNORM:
            return "BC5_SNORM";
        case ERenderFormat::RENDER_FORMAT_R16G16B16_FLOAT:
            return "R16G16B16_FLOAT";
        case ERenderFormat::RENDER_FORMAT_INDEX_32:
            return "INDEX_32";
        case ERenderFormat::RENDER_FORMAT_INDEX_16:
            return "INDEX_16";
        case ERenderFormat::RENDER_FORMAT_LE_X2R10G10B10_UNORM:
            return "LE_X2R10G10B10_UNORM";
        case ERenderFormat::RENDER_FORMAT_LE_X8R8G8B8_UNORM:
            return "LE_X8R8G8B8_UNORM";
        case ERenderFormat::RENDER_FORMAT_X16Y16Z16_SNORM:
            return "X16Y16Z16_SNORM";
        default:
            return "";
    }
}

std::string Texture::ConvertInterpretAsToString(const ZTextureMap::EInterpretAs interpretAs)
{
    switch (interpretAs)
    {
        case ZTextureMap::EInterpretAs::INTERPRET_AS_COLOR:
            return "Color";
        case ZTextureMap::EInterpretAs::INTERPRET_AS_NORMAL:
            return "Normal";
        case ZTextureMap::EInterpretAs::INTERPRET_AS_HEIGHT:
            return "Height";
        default:
            return "";
    }
}

std::string Texture::ConvertDimensionsToString(const ZTextureMap::EDimensions dimensions)
{
    switch (dimensions)
    {
        case ZTextureMap::EDimensions::DIMENSIONS_2D:
            return "2D";
        case ZTextureMap::EDimensions::DIMENSIONS_CUBE:
            return "Cube";
        case ZTextureMap::EDimensions::DIMENSIONS_VOLUME:
            return "Volume";
        default:
            return "";
    }
}

std::string Texture::ConvertRenderResourceMipInterpolationToString(const ERenderResourceMipInterpolation renderResourceMipInterpolation)
{
    switch (renderResourceMipInterpolation)
    {
        case ERenderResourceMipInterpolation::eRENDER_RESOURCE_MIP_INTERPOLATION_NONE:
            return "None";
        case ERenderResourceMipInterpolation::eRENDER_RESOURCE_MIP_INTERPOLATION_POINT:
            return "Point";
        case ERenderResourceMipInterpolation::eRENDER_RESOURCE_MIP_INTERPOLATION_LINEAR:
            return "Linear";
        default:
            return "";
    }
}

void Texture::LoadTextureFromMemory(void* data, unsigned int width, unsigned int height, ID3D11Device* d3dDevice, ID3D11ShaderResourceView** shaderResourceView)
{
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D* pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	d3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	d3dDevice->CreateShaderResourceView(pTexture, &srvDesc, shaderResourceView);
	pTexture->Release();
}
