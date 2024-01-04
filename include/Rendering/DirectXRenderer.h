#pragma once

#include <d3d11.h>

#include "SwapChain.h"
#include "Logger.h"
#include "CommandList.h"
#include "Viewport.h"

class DirectXRenderer
{
public:
	DirectXRenderer();
	bool Setup(HWND hwnd, WNDCLASSEXA* wc);
	bool CreateD3DDevice(HWND hWnd);
	void CleanupD3DDevice();
	const HWND GetHWND() const;
	const ID3D11Device* GetD3D11Device() const;
	ID3D11Device* GetD3D11Device();
	const ID3D11DeviceContext* GetD3D11DeviceContext() const;
	ID3D11DeviceContext* GetD3D11DeviceContext();
	SwapChain* GetSwapChain();
	Viewport& GetViewport();
	void SetBackBufferAsRenderTarget();
	void ClearBackBuffer();
	CommandList& GetCommandList();
	HRESULT CreateTexture(unsigned short width, unsigned short height, ID3D11Texture2D** texture, ID3D11ShaderResourceView** textureView);
	HRESULT UpdateTexture(ID3D11Texture2D* texture, ID3D11ShaderResourceView* textureView, const unsigned char* pData, unsigned short width, unsigned short height);

private:
	HWND hwnd;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dDeviceContext;
	SwapChain* swapChain;
	Viewport viewport;
	CommandList commandList;
};
