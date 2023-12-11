#include "imgui.h"

#include "Rendering/DirectXRenderer.h"
#include "Display.h"
#include "Utility/D3D11Utility.h"

DirectXRenderer::DirectXRenderer()
{
    d3dDevice = nullptr;
    d3dDeviceContext = nullptr;
    swapChain = nullptr;
}

bool DirectXRenderer::Setup(HWND hwnd, WNDCLASSEXW* wc)
{
    this->hwnd = hwnd;
    swapChain = new SwapChain(Display::GetWidth(), Display::GetHeight(), SwapChain::PresentMode::Immediate, 2);

    if (!CreateD3DDevice(hwnd))
    {
        CleanupD3DDevice();
        UnregisterClassW(wc->lpszClassName, wc->hInstance);

        return false;
    }

    if (!swapChain->Create() || !swapChain->CreateRenderTargetView())
    {
        CleanupD3DDevice();
        UnregisterClassW(wc->lpszClassName, wc->hInstance);

        return false;
    }

    Logger::GetInstance().Log(Logger::Level::Info, "DirectX renderer successfully set up.");

    return true;
}

bool DirectXRenderer::CreateD3DDevice(HWND hWnd)
{
    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &d3dDevice, &featureLevel, &d3dDeviceContext);

    if (result == DXGI_ERROR_UNSUPPORTED) // Try high-performance WARP software driver if hardware is not available.
    {
        result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &d3dDevice, &featureLevel, &d3dDeviceContext);
    }

    if (result != S_OK)
    {
        return false;
    }

    return true;
}

void DirectXRenderer::CleanupD3DDevice()
{
    delete swapChain;

    D3D11Utility::Release(d3dDeviceContext);
    D3D11Utility::Release(d3dDevice);
}

const HWND DirectXRenderer::GetHWND() const
{
    return hwnd;
}

const ID3D11Device* DirectXRenderer::GetD3D11Device()const
{
    return d3dDevice;
}

ID3D11Device* DirectXRenderer::GetD3D11Device()
{
    return d3dDevice;
}

const ID3D11DeviceContext* DirectXRenderer::GetD3D11DeviceContext() const
{
    return d3dDeviceContext;
}

ID3D11DeviceContext* DirectXRenderer::GetD3D11DeviceContext()
{
    return d3dDeviceContext;
}

SwapChain* DirectXRenderer::GetSwapChain()
{
    return swapChain;
}

Viewport& DirectXRenderer::GetViewport()
{
    return viewport;
}

void DirectXRenderer::SetBackBufferAsRenderTarget()
{
    ID3D11RenderTargetView* renderTargetView = swapChain->GetRenderTargetView();

    d3dDeviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
}

void DirectXRenderer::ClearBackBuffer()
{
    static ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static const float clearColorWithAlpha[4] = { backgroundColor.x * backgroundColor.w, backgroundColor.y * backgroundColor.w, backgroundColor.z * backgroundColor.w, backgroundColor.w };

    d3dDeviceContext->ClearRenderTargetView(swapChain->GetRenderTargetView(), clearColorWithAlpha);
}

CommandList& DirectXRenderer::GetCommandList()
{
    return commandList;
}

HRESULT DirectXRenderer::CreateTexture(unsigned short width, unsigned short height, ID3D11Texture2D** texture, ID3D11ShaderResourceView** textureView)
{
    D3D11_TEXTURE2D_DESC desc = {};

    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    HRESULT result = d3dDevice->CreateTexture2D(&desc, nullptr, texture);

    if (FAILED(result))
    {
        return result;
    }

    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};

    viewDesc.Format = desc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MostDetailedMip = 0;
    viewDesc.Texture2D.MipLevels = 1;

    result = d3dDevice->CreateShaderResourceView(*texture, &viewDesc, textureView);

    if (FAILED(result))
    {
        (*texture)->Release();
    }

    return result;
}
