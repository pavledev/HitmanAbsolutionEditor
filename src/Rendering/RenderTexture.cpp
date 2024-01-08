#include <utility>

#include "directxtk/DDSTextureLoader.h"

#include "Rendering/RenderTexture.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/EnumUtility.h"
#include "Logger.h"
#include "Utility/D3D11Utility.h"
#include <directxtk/ScreenGrab.h>
#include <wincodec.h>
#include "Utility/Math.h"
#include "Editor.h"

RenderTexture::RenderTexture(const float width, const float height, const DXGI_FORMAT format, const unsigned int flags, const Color* color) : format(format), flags(static_cast<Flags>(flags)), hasAlpha(false)
{
	Resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height), color);
}

RenderTexture::RenderTexture(const RenderMaterialInstance::Texture& texture)
{
	DirectX::ScratchImage scratchImage;
	HRESULT result = DirectX::LoadFromDDSMemory(texture.blob->GetBufferPointer(), texture.blob->GetBufferSize(), DirectX::DDS_FLAGS_NONE, nullptr, scratchImage);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to load {} texture!", texture.name.c_str());

		return;
	}

	name = texture.name;
	width = static_cast<unsigned int>(scratchImage.GetMetadata().width);
	height = static_cast<unsigned int>(scratchImage.GetMetadata().height);
	flags = Flags::SRV;
	format = scratchImage.GetMetadata().format;
	hasAlpha = !scratchImage.IsAlphaAllOpaque();

	if (!CreateTexture(&scratchImage))
	{
		return;
	}

	CreateShaderResourceView();
}

RenderTexture::~RenderTexture()
{
	Release();
}

const unsigned int RenderTexture::GetWidth() const
{
	return width;
}

const unsigned int RenderTexture::GetHeight() const
{
	return height;
}

const DXGI_FORMAT RenderTexture::GetFormat() const
{
	return format;
}

const bool RenderTexture::HasAlpha() const
{
	return hasAlpha;
}

ID3D11Texture2D* RenderTexture::GetTexture() const
{
	return texture;
}

ID3D11RenderTargetView* RenderTexture::GetRenderTargetView() const
{
	return renderTargetView;
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView() const
{
	return shaderResourceView;
}

ID3D11UnorderedAccessView* RenderTexture::GetUnorderedAccessView() const
{
	return unorderedAccessView;
}

ID3D11DepthStencilView* RenderTexture::GetDepthStencilView() const
{
	return depthStencilView;
}

UINT RenderTexture::GetBindFlags()
{
	UINT bindFlags = 0;

	bindFlags |= IsSRV() ? D3D11_BIND_SHADER_RESOURCE : 0;
	bindFlags |= IsUAV() ? D3D11_BIND_UNORDERED_ACCESS : 0;
	bindFlags |= IsRenderTargetDepthStencil() ? D3D11_BIND_DEPTH_STENCIL : 0;
	bindFlags |= IsRenderTargetColor() ? D3D11_BIND_RENDER_TARGET : 0;

	return bindFlags;
}

DXGI_FORMAT RenderTexture::FindShaderResourceDXGIFormat(DXGI_FORMAT format, bool sRGB)
{
	if (sRGB)
	{
		switch (format)
		{
			case DXGI_FORMAT_B8G8R8A8_TYPELESS:    return DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
			case DXGI_FORMAT_R8G8B8A8_TYPELESS:    return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
			case DXGI_FORMAT_BC1_TYPELESS:         return DXGI_FORMAT_BC1_UNORM_SRGB;
			case DXGI_FORMAT_BC2_TYPELESS:         return DXGI_FORMAT_BC2_UNORM_SRGB;
			case DXGI_FORMAT_BC3_TYPELESS:         return DXGI_FORMAT_BC3_UNORM_SRGB;
			case DXGI_FORMAT_BC7_TYPELESS:         return DXGI_FORMAT_BC7_UNORM_SRGB;
		};
	}
	else
	{
		switch (format)
		{
			case DXGI_FORMAT_B8G8R8A8_TYPELESS: return DXGI_FORMAT_B8G8R8A8_UNORM;
			case DXGI_FORMAT_R8G8B8A8_TYPELESS: return DXGI_FORMAT_R8G8B8A8_UNORM;
			case DXGI_FORMAT_BC1_TYPELESS:      return DXGI_FORMAT_BC1_UNORM;
			case DXGI_FORMAT_BC2_TYPELESS:      return DXGI_FORMAT_BC2_UNORM;
			case DXGI_FORMAT_BC3_TYPELESS:      return DXGI_FORMAT_BC3_UNORM;
			case DXGI_FORMAT_BC7_TYPELESS:      return DXGI_FORMAT_BC7_UNORM;
		};
	}

	switch (format)
	{
		case DXGI_FORMAT_R24G8_TYPELESS: return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		case DXGI_FORMAT_R32_TYPELESS: return DXGI_FORMAT_R32_FLOAT;
		case DXGI_FORMAT_R16_TYPELESS: return DXGI_FORMAT_R16_UNORM;
			// Changing Depth Buffers to 32 bit on Dingo as D24S8 is actually implemented as a 32 bit buffer in the hardware
		case DXGI_FORMAT_R32G8X24_TYPELESS: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
	}

	return format;
}

bool RenderTexture::IsSRV() const
{
	return EnumUtility::HasFlag(flags, Flags::SRV);
}

bool RenderTexture::IsUAV() const
{
	return EnumUtility::HasFlag(flags, Flags::UAV);
}

bool RenderTexture::IsRenderTarget() const
{
	return EnumUtility::HasFlag(flags, Flags::RenderTarget);
}

bool RenderTexture::IsRenderTargetDepthStencil() const
{
	return IsRenderTarget() && IsDepthStencilFormat();
}

bool RenderTexture::IsRenderTargetColor() const
{
	return IsRenderTarget() && IsColorFormat();
}

bool RenderTexture::IsDepthFormat() const
{
	return format == DXGI_FORMAT_R32_TYPELESS || format == DXGI_FORMAT_R32G8X24_TYPELESS;
}

bool RenderTexture::IsStencilFormat() const
{
	return format == DXGI_FORMAT_R32G8X24_TYPELESS;
}

bool RenderTexture::IsDepthStencilFormat() const
{
	return format == DXGI_FORMAT_R32_TYPELESS || format == DXGI_FORMAT_R32G8X24_TYPELESS;
}

bool RenderTexture::IsColorFormat() const
{
	return !IsDepthStencilFormat();
}

void RenderTexture::Resize(unsigned int width, unsigned int height, const Color* color)
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();

	this->width = width;
	this->height = height;

	Release();

	if (!CreateTexture(color))
	{
		return;
	}

	if (IsSRV() && !CreateShaderResourceView())
	{
		return;
	}

	if (IsUAV() && !CreateUnorderedAccessView())
	{
		return;
	}

	if (IsRenderTargetDepthStencil() && !CreateDepthStencilView())
	{
		return;
	}

	if (IsRenderTargetColor())
	{
		CreateRenderTargetView();
	}
}

bool RenderTexture::CreateTexture(const Color* color)
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = GetBindFlags();
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HRESULT result = 0;
	D3D11_SUBRESOURCE_DATA initData = {};

	if (color)
	{
		size_t NumBytes = 0;
		size_t RowBytes = 0;
		result = GetSurfaceInfo(width, height, format, &NumBytes, &RowBytes, nullptr);

		if (FAILED(result))
		{
			return false;
		}

		initData.pSysMem = color;
		initData.SysMemPitch = static_cast<UINT>(RowBytes);
		initData.SysMemSlicePitch = static_cast<UINT>(NumBytes);

		result = d3d11Device->CreateTexture2D(&textureDesc, &initData, &texture);
	}
	else
	{
		result = d3d11Device->CreateTexture2D(&textureDesc, nullptr, &texture);
	}

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create texture!");

		return false;
	}

	return true;
}

bool RenderTexture::CreateTexture(DirectX::ScratchImage* scratchImage)
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = GetBindFlags();
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	size_t NumBytes = 0;
	size_t RowBytes = 0;
	HRESULT result = GetSurfaceInfo(width, height, format, &NumBytes, &RowBytes, nullptr);

	if (FAILED(result))
	{
		return false;
	}

	initData.pSysMem = scratchImage->GetPixels();
	initData.SysMemPitch = static_cast<UINT>(RowBytes);
	initData.SysMemSlicePitch = static_cast<UINT>(NumBytes);

	result = d3d11Device->CreateTexture2D(&textureDesc, &initData, &texture);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create texture!");

		return false;
	}

	return true;
}

bool RenderTexture::CreateShaderResourceView()
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResViewDesc = {};

	if (IsDepthStencilFormat())
	{
		shaderResViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
	}
	else
	{
		shaderResViewDesc.Format = FindShaderResourceDXGIFormat(format, false);
	}

	shaderResViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResViewDesc.Texture2D.MipLevels = 1;

	HRESULT result = d3d11Device->CreateShaderResourceView(texture, &shaderResViewDesc, &shaderResourceView);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create shader resource view!");

		return false;
	}

	return true;
}

bool RenderTexture::CreateUnorderedAccessView()
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
	D3D11_UNORDERED_ACCESS_VIEW_DESC unorderedAccessViewDesc = {};

	unorderedAccessViewDesc.Format = format;
	unorderedAccessViewDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
	unorderedAccessViewDesc.Texture2D.MipSlice = 0;

	HRESULT result = d3d11Device->CreateUnorderedAccessView(texture, &unorderedAccessViewDesc, &unorderedAccessView);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create unordered access view!");

		return false;
	}

	return true;
}

bool RenderTexture::CreateDepthStencilView()
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

	depthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2DArray.MipSlice = 0;
	depthStencilViewDesc.Texture2DArray.ArraySize = 1;
	depthStencilViewDesc.Flags = 0;

	HRESULT result = d3d11Device->CreateDepthStencilView(texture, &depthStencilViewDesc, &depthStencilView);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create depth stencil view!");

		return false;
	}

	return true;
}

bool RenderTexture::CreateRenderTargetView()
{
	ID3D11Device* d3d11Device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;

	renderTargetViewDesc.Format = format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	HRESULT result = d3d11Device->CreateRenderTargetView(texture, &renderTargetViewDesc, &renderTargetView);

	if (FAILED(result))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to create render target view!");

		return false;
	}

	return true;
}

void RenderTexture::Release()
{
	D3D11Utility::Release(shaderResourceView);
	D3D11Utility::Release(unorderedAccessView);
	D3D11Utility::Release(depthStencilView);
	D3D11Utility::Release(renderTargetView);
	D3D11Utility::Release(texture);
}

void RenderTexture::SetAsRendererTarget()
{
	Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void RenderTexture::ClearRenderTarget(const float* colour)
{
	Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->ClearRenderTargetView(renderTargetView, colour);
	//Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

HRESULT RenderTexture::GetSurfaceInfo(size_t width, size_t height, DXGI_FORMAT fmt, size_t* outNumBytes, size_t* outRowBytes, size_t* outNumRows)
{
	unsigned long long numBytes = 0;
	unsigned long long rowBytes = 0;
	unsigned long long numRows = 0;

	bool bc = false;
	bool packed = false;
	bool planar = false;
	size_t bpe = 0;

	switch (fmt)
	{
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			bc = true;
			bpe = 8;
			break;

		case DXGI_FORMAT_BC2_TYPELESS:
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_UNORM_SRGB:
		case DXGI_FORMAT_BC3_TYPELESS:
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_UNORM_SRGB:
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
		case DXGI_FORMAT_BC6H_TYPELESS:
		case DXGI_FORMAT_BC6H_UF16:
		case DXGI_FORMAT_BC6H_SF16:
		case DXGI_FORMAT_BC7_TYPELESS:
		case DXGI_FORMAT_BC7_UNORM:
		case DXGI_FORMAT_BC7_UNORM_SRGB:
			bc = true;
			bpe = 16;
			break;

		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_YUY2:
			packed = true;
			bpe = 4;
			break;

		case DXGI_FORMAT_Y210:
		case DXGI_FORMAT_Y216:
			packed = true;
			bpe = 8;
			break;

		case DXGI_FORMAT_NV12:
		case DXGI_FORMAT_420_OPAQUE:
			if ((height % 2) != 0)
			{
				// Requires a height alignment of 2.
				return E_INVALIDARG;
			}
			planar = true;
			bpe = 2;
			break;

		case DXGI_FORMAT_P010:
		case DXGI_FORMAT_P016:
			if ((height % 2) != 0)
			{
				// Requires a height alignment of 2.
				return E_INVALIDARG;
			}
			planar = true;
			bpe = 4;
			break;

		default:
			break;
	}

	if (bc)
	{
		unsigned long long numBlocksWide = 0;
		if (width > 0)
		{
			numBlocksWide = Math::Max<unsigned long long>(1u, (unsigned long long(width) + 3u) / 4u);
		}
		unsigned long long numBlocksHigh = 0;
		if (height > 0)
		{
			numBlocksHigh = Math::Max<unsigned long long>(1u, (unsigned long long(height) + 3u) / 4u);
		}
		rowBytes = numBlocksWide * bpe;
		numRows = numBlocksHigh;
		numBytes = rowBytes * numBlocksHigh;
	}
	else if (packed)
	{
		rowBytes = ((unsigned long long(width) + 1u) >> 1) * bpe;
		numRows = unsigned long long(height);
		numBytes = rowBytes * height;
	}
	else if (fmt == DXGI_FORMAT_NV11)
	{
		rowBytes = ((unsigned long long(width) + 3u) >> 2) * 4u;
		numRows = unsigned long long(height) * 2u; // Direct3D makes this simplifying assumption, although it is larger than the 4:1:1 data
		numBytes = rowBytes * numRows;
	}
	else if (planar)
	{
		rowBytes = ((unsigned long long(width) + 1u) >> 1) * bpe;
		numBytes = (rowBytes * unsigned long long(height)) + ((rowBytes * unsigned long long(height) + 1u) >> 1);
		numRows = height + ((unsigned long long(height) + 1u) >> 1);
	}
	else
	{
		const size_t bpp = DirectX::BitsPerPixel(fmt);
		if (!bpp)
			return E_INVALIDARG;

		rowBytes = (unsigned long long(width) * bpp + 7u) / 8u; // round up to nearest byte
		numRows = unsigned long long(height);
		numBytes = rowBytes * height;
	}

	if (outNumBytes)
	{
		*outNumBytes = static_cast<size_t>(numBytes);
	}
	if (outRowBytes)
	{
		*outRowBytes = static_cast<size_t>(rowBytes);
	}
	if (outNumRows)
	{
		*outNumRows = static_cast<size_t>(numRows);
	}

	return S_OK;
}
