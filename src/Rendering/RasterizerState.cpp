#include "Rendering/RasterizerState.h"
#include "Utility/D3D11Utility.h"
#include "Rendering/DirectXRenderer.h"
#include "Editor.h"

RasterizerState::RasterizerState
(
    const FillMode fillMode,
    const CullMode cullMode,
    const bool enableLineAA,
    const bool enableMSAA,
    const RasterizerState::DepthClipMode depthClipMode,
    const float depthBias,
    const float depthBiasClamp,
    const float depthBiasSlopeScaled)
{
    this->fillMode = ConvertFillMode(fillMode);
    this->cullMode = ConvertCullMode(cullMode);
    this->depthClipEnabled = depthClipMode == DepthClipMode::DepthClip;
    this->scissorEnabled = false;
    this->enableMSAA = enableMSAA;
    this->enableLineAA = enableLineAA;
    this->depthBias = depthBias;
    this->depthBiasClamp = depthBiasClamp;
    this->depthBiasSlopeScaled = depthBiasSlopeScaled;

    CreateResource();
}

RasterizerState::~RasterizerState()
{
    D3D11Utility::Release(rasterizerState);
}

ID3D11RasterizerState* RasterizerState::GetRasterizerState() const
{
    return rasterizerState;
}

D3D11_FILL_MODE RasterizerState::ConvertFillMode(const FillMode fillMode)
{
    switch (fillMode)
    {
        case FillMode::Wireframe:
            return D3D11_FILL_WIREFRAME;
        default:
            return D3D11_FILL_SOLID;
    };
}

D3D11_CULL_MODE RasterizerState::ConvertCullMode(const CullMode cullMode)
{
    switch (cullMode)
    {
        case CullMode::Clockwise:
            return D3D11_CULL_BACK;
        case CullMode::CounterClockwise:
            return D3D11_CULL_FRONT;
        default:
            return D3D11_CULL_NONE;
    };
}

void RasterizerState::CreateResource()
{
    D3D11_RASTERIZER_DESC desc = {};
    desc.FillMode = fillMode;
    desc.CullMode = cullMode;
    desc.FrontCounterClockwise = false;
    desc.DepthBias = static_cast<UINT>(std::floor(depthBias * static_cast<float>(1 << 24)));
    desc.DepthBiasClamp = depthBiasClamp;
    desc.SlopeScaledDepthBias = depthBiasSlopeScaled;
    desc.DepthClipEnable = depthClipEnabled;
    desc.MultisampleEnable = enableMSAA;
    desc.AntialiasedLineEnable = enableLineAA;
    desc.ScissorEnable = scissorEnabled;

    ID3D11Device* d3dDevice = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    HRESULT result = d3dDevice->CreateRasterizerState(&desc, &rasterizerState);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create rasterizer state!");
    }
}
