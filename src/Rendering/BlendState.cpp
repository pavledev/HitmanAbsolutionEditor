#include "Rendering/BlendState.h"
#include "Utility/D3D11Utility.h"
#include "Rendering/DirectXRenderer.h"
#include "Editor.h"

BlendState::BlendState(const std::array<RenderTarget, 8>& renderTargets, const bool useAlphaToCoverage)
{
    this->renderTargets = renderTargets;
    this->useAlphaToCoverage = useAlphaToCoverage;

    CreateResource();
}

BlendState::~BlendState()
{
    D3D11Utility::Release(blendState);
}

ID3D11BlendState* BlendState::GetBlendState() const
{
    return blendState;
}

D3D11_BLEND_OP BlendState::ConvertBlendOp(const BlendOperation blendOp)
{
    switch (blendOp)
    {
        case BlendOperation::Subtract:
            return D3D11_BLEND_OP_SUBTRACT;
        case BlendOperation::Min:
            return D3D11_BLEND_OP_MIN;
        case BlendOperation::Max:
            return D3D11_BLEND_OP_MAX;
        case BlendOperation::ReverseSubtract:
            return D3D11_BLEND_OP_REV_SUBTRACT;
        default:
            return D3D11_BLEND_OP_ADD;
    };
}

D3D11_BLEND BlendState::ConvertBlendFactor(const BlendFactor blendFactor)
{
    switch (blendFactor)
    {
        case BlendFactor::One:
            return D3D11_BLEND_ONE;
        case BlendFactor::SourceColor:
            return D3D11_BLEND_SRC_COLOR;
        case BlendFactor::InverseSourceColor:
            return D3D11_BLEND_INV_SRC_COLOR;
        case BlendFactor::SourceAlpha:
            return D3D11_BLEND_SRC_ALPHA;
        case BlendFactor::InverseSourceAlpha:
            return D3D11_BLEND_INV_SRC_ALPHA;
        case BlendFactor::DestAlpha:
            return D3D11_BLEND_DEST_ALPHA;
        case BlendFactor::InverseDestAlpha:
            return D3D11_BLEND_INV_DEST_ALPHA;
        case BlendFactor::DestColor:
            return D3D11_BLEND_DEST_COLOR;
        case BlendFactor::InverseDestColor:
            return D3D11_BLEND_INV_DEST_COLOR;
        case BlendFactor::ConstantBlendFactor:
            return D3D11_BLEND_BLEND_FACTOR;
        case BlendFactor::InverseConstantBlendFactor:
            return D3D11_BLEND_INV_BLEND_FACTOR;
        case BlendFactor::Source1Color:
            return D3D11_BLEND_SRC1_COLOR;
        case BlendFactor::InverseSource1Color:
            return D3D11_BLEND_INV_SRC1_COLOR;
        case BlendFactor::Source1Alpha:
            return D3D11_BLEND_SRC1_ALPHA;
        case BlendFactor::InverseSource1Alpha:
            return D3D11_BLEND_INV_SRC1_ALPHA;

        default: return D3D11_BLEND_ZERO;
    };
}

void BlendState::CreateResource()
{
    D3D11_BLEND_DESC blendDesc = {};

    blendDesc.AlphaToCoverageEnable = useAlphaToCoverage;
    blendDesc.IndependentBlendEnable = false;

    for (size_t i = 0; i < renderTargets.size(); ++i)
    {
        D3D11_RENDER_TARGET_BLEND_DESC& renderTarget = blendDesc.RenderTarget[i];

        renderTarget.BlendEnable =
            renderTargets[i].colorBlendOp != BlendOperation::Add || renderTargets[i].colorDestBlend != BlendFactor::Zero || renderTargets[i].colorSrcBlend != BlendFactor::One ||
            renderTargets[i].alphaBlendOp != BlendOperation::Add || renderTargets[i].alphaDestBlend != BlendFactor::Zero || renderTargets[i].alphaSrcBlend != BlendFactor::One;
        renderTarget.BlendOp = ConvertBlendOp(renderTargets[i].colorBlendOp);
        renderTarget.SrcBlend = ConvertBlendFactor(renderTargets[i].colorSrcBlend);
        renderTarget.DestBlend = ConvertBlendFactor(renderTargets[i].colorDestBlend);
        renderTarget.BlendOpAlpha = ConvertBlendOp(renderTargets[i].alphaBlendOp);
        renderTarget.SrcBlendAlpha = ConvertBlendFactor(renderTargets[i].alphaSrcBlend);
        renderTarget.DestBlendAlpha = ConvertBlendFactor(renderTargets[i].alphaDestBlend);
        renderTarget.RenderTargetWriteMask =
            ((renderTargets[i].colorWriteMask & ColorWriteMask::RED) ? D3D11_COLOR_WRITE_ENABLE_RED : 0)
            | ((renderTargets[i].colorWriteMask & ColorWriteMask::GREEN) ? D3D11_COLOR_WRITE_ENABLE_GREEN : 0)
            | ((renderTargets[i].colorWriteMask & ColorWriteMask::BLUE) ? D3D11_COLOR_WRITE_ENABLE_BLUE : 0)
            | ((renderTargets[i].colorWriteMask & ColorWriteMask::ALPHA) ? D3D11_COLOR_WRITE_ENABLE_ALPHA : 0);
    }

    ID3D11Device* d3dDevice = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    HRESULT result = d3dDevice->CreateBlendState(&blendDesc, &blendState);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create blend state!");
    }
}
