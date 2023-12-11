#pragma once

#include <d3d11.h>

#include "Resources/RenderMaterialInstance.h"
#include "Math/Color.h"

class RenderTexture
{
public:
	enum Flags
	{
		SRV = 1,
		UAV = 2,
		RenderTarget = 4
	};

	RenderTexture() = default;
	RenderTexture(const float width, const float height, const DXGI_FORMAT format = DXGI_FORMAT_B8G8R8A8_UNORM, const unsigned int flags = 0, const Color* color = nullptr);
	RenderTexture(const RenderMaterialInstance::Texture& texture);
	~RenderTexture();
	const unsigned int GetWidth() const;
	const unsigned int GetHeight() const;
	const DXGI_FORMAT GetFormat() const;
	const bool HasAlpha() const;
	ID3D11Texture2D* GetTexture() const;
	ID3D11RenderTargetView* GetRenderTargetView() const;
	ID3D11ShaderResourceView* GetShaderResourceView() const;
	ID3D11UnorderedAccessView* GetUnorderedAccessView() const;
	ID3D11DepthStencilView* GetDepthStencilView() const;
	UINT GetBindFlags();
	static DXGI_FORMAT FindShaderResourceDXGIFormat(DXGI_FORMAT format, bool sRGB);
	bool IsSRV() const;
	bool IsUAV() const;
	bool IsRenderTarget() const;
	bool IsRenderTargetDepthStencil() const;
	bool IsRenderTargetColor() const;
	bool IsDepthFormat() const;
	bool IsStencilFormat() const;
	bool IsDepthStencilFormat() const;
	bool IsColorFormat() const;
	void Resize(unsigned int width, unsigned int height, const Color* color = nullptr);
	bool CreateTexture(const Color* color);
	bool CreateTexture(DirectX::ScratchImage* scratchImage = nullptr);
	bool CreateShaderResourceView();
	bool CreateUnorderedAccessView();
	bool CreateDepthStencilView();
	bool CreateRenderTargetView();
	void Release();
	void SetAsRendererTarget();
	void ClearRenderTarget(const float* bgColor);
	static HRESULT GetSurfaceInfo(size_t width, size_t height, DXGI_FORMAT fmt, size_t* outNumBytes, size_t* outRowBytes, size_t* outNumRows);

private:
	std::string name;
	unsigned int width;
	unsigned int height;
	Flags flags;
	DXGI_FORMAT format;
	bool hasAlpha;
	ID3D11Texture2D* texture = nullptr;
	ID3D11RenderTargetView* renderTargetView = nullptr;
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	ID3D11UnorderedAccessView* unorderedAccessView = nullptr;
};
