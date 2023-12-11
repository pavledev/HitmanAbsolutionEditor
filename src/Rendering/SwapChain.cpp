#include <cassert>
#include <dxgi1_4.h>
#include <dxgi1_5.h>
#include <wrl/client.h>

#include "Rendering/SwapChain.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/D3D11Utility.h"
#include "Display.h"
#include "Editor.h"

SwapChain::SwapChain(const unsigned int width, const unsigned int height, const PresentMode presentMode, const unsigned int bufferCount)
{
    this->width = width;
    this->height = height;
    this->presentMode = presentMode;
    this->bufferCount = bufferCount;
    format = DXGI_FORMAT_R8G8B8A8_UNORM;
    windowed = true;
}

SwapChain::~SwapChain()
{
    // Before shutting down set to windowed mode to avoid swap chain exception
    if (swapChain && !windowed)
    {
        swapChain->SetFullscreenState(false, nullptr);
    }

    D3D11Utility::Release(swapChain);
    D3D11Utility::Release(renderTargetView);
}

unsigned int SwapChain::GetWidth() const
{
    return width;
}

unsigned int SwapChain::GetHeight() const
{
    return height;
}

unsigned int SwapChain::GetBufferCount() const
{
    return bufferCount;
}

DXGI_FORMAT SwapChain::GetFormat() const
{
    return format;
}

ID3D11RenderTargetView* SwapChain::GetRenderTargetView() const
{
    return renderTargetView;
}

bool SwapChain::Create()
{
    ID3D11Device* d3dDevice = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    IDXGIDevice* dxgiDevice = nullptr;
    HRESULT result = d3dDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to query IDXGIDevice interface!");

        return false;
    }

    IDXGIAdapter* dxgiAdapter = nullptr;
    result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to get adapter!");

        return false;
    }

    IDXGIFactory* dxgiFactory = nullptr;
    result = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to get factory!");

        return false;
    }

    DXGI_SWAP_CHAIN_DESC desc = {};
    desc.BufferCount = bufferCount;
    desc.BufferDesc.Width = width;
    desc.BufferDesc.Height = height;
    desc.BufferDesc.RefreshRate = DXGI_RATIONAL{ Display::GetRefreshRate(), 1 };
    desc.BufferDesc.Format = format;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.OutputWindow = Editor::GetInstance().GetDirectXRenderer()->GetHWND();
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Windowed = windowed ? TRUE : FALSE;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.Flags = GetPresentFlags(presentMode);

    result = dxgiFactory->CreateSwapChain(d3dDevice, &desc, &swapChain);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create swapchain!");

        return false;
    }

    D3D11Utility::Release(dxgiDevice);
    D3D11Utility::Release(dxgiAdapter);
    D3D11Utility::Release(dxgiFactory);

    return true;
}

bool SwapChain::CreateRenderTargetView()
{
    ID3D11Device* d3dDevice = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    ID3D11Texture2D* backbuffer = nullptr;
    HRESULT result = swapChain->GetBuffer(0, IID_PPV_ARGS(&backbuffer));

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to get buffer!");

        return false;
    }

    result = d3dDevice->CreateRenderTargetView(backbuffer, nullptr, &renderTargetView);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create render target view!");

        return false;
    }

    D3D11Utility::Release(backbuffer);

    return true;
}

bool SwapChain::Resize(unsigned int width, unsigned int height, const bool force)
{
    assert(renderTargetView != nullptr);

    // Only resize if needed
    if (!force)
    {
        if (this->width == width && this->height == height)
        {
            return false;
        }
    }

    this->width = width;
    this->height = height;

    // Set this flag to enable an application to switch modes by calling IDXGISwapChain::ResizeTarget.
    // When switching from windowed to full-screen mode, the display mode (or monitor resolution)
    // will be changed to match the dimensions of the application window.
    if (true) // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    {
        DXGI_MODE_DESC dxgi_mode_desc = {};
        dxgi_mode_desc.Width = width;
        dxgi_mode_desc.Height = height;
        dxgi_mode_desc.Format = format;
        dxgi_mode_desc.RefreshRate = DXGI_RATIONAL{ Display::GetRefreshRate(), 1 };
        dxgi_mode_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        dxgi_mode_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

        const HRESULT result = swapChain->ResizeTarget(&dxgi_mode_desc);

        if (FAILED(result))
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Failed to resize swapchain target!");

            return false;
        }
    }

    // Release the previous render target view (to avoid IDXGISwapChain::ResizeBuffers: Swapchain cannot be resized unless all outstanding buffer references have been released)
    D3D11Utility::Release(renderTargetView);

    const UINT d3d11Flags = GetPresentFlags(GetSupportedPresentMode(presentMode));
    HRESULT result = swapChain->ResizeBuffers(bufferCount, width, height, format, d3d11Flags);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to resize swapchain buffers!");

        return false;
    }

    CreateRenderTargetView();

    return true;
}

void SwapChain::Present()
{
    if (!swapChain)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Can't present, the swapchain has not been initialised!");

        return;
    }

    const bool tearingAllowed = presentMode == PresentMode::Immediate;
    const UINT syncInterval = tearingAllowed ? 0 : 1; // sync interval can go up to 4, so this could be improved
    const UINT flags = (tearingAllowed && windowed) ? DXGI_PRESENT_ALLOW_TEARING : 0;
    const HRESULT result = swapChain->Present(syncInterval, flags);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to present!");
    }
}

UINT SwapChain::GetPresentFlags(const PresentMode presentMode)
{
    UINT d3d11Flags = 0;

    d3d11Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    d3d11Flags |= presentMode == PresentMode::Immediate ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0;

    return d3d11Flags;
}

SwapChain::PresentMode SwapChain::GetSupportedPresentMode(const PresentMode presentMode)
{
    // If SwapChain_Allow_Tearing was requested
    if (presentMode == PresentMode::Immediate)
    {
        // Check if the adapter supports it, if not, disable it (tends to fail with Intel adapters)
        if (!IsTearingSupported())
        {
            Logger::GetInstance().Log(Logger::Level::Warning, "PresentMode::Immediate is not supported, falling back to PresentMode::Fifo!");

            return PresentMode::Fifo;
        }
    }

    return presentMode;
}

// Determines whether tearing support is available for fullscreen borderless windows.
bool SwapChain::IsTearingSupported()
{
    // Rather than create the 1.5 factory interface directly, we create the 1.4
    // interface and query for the 1.5 interface. This will enable the graphics
    // debugging tools which might not support the 1.5 factory interface
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory4;
    HRESULT result = CreateDXGIFactory1(IID_PPV_ARGS(&factory4));
    BOOL allowTearing = FALSE;

    if (SUCCEEDED(result))
    {
        Microsoft::WRL::ComPtr<IDXGIFactory5> factory5;
        result = factory4.As(&factory5);

        if (SUCCEEDED(result))
        {
            result = factory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(allowTearing));
        }
    }

    return SUCCEEDED(result) && allowTearing;
}
