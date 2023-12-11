#include "Rendering/DepthStencilState.h"
#include "Utility/D3D11Utility.h"
#include "Rendering/DirectXRenderer.h"
#include "Editor.h"

DepthStencilState::DepthStencilState(
    bool enableDepthWrite,
    DepthStencilState::CompareFunction depthTest,
    bool enableFrontFaceStencil,
    DepthStencilState::CompareFunction frontFaceStencilTest,
    DepthStencilState::StencilOp frontFaceStencilFailStencilOp,
    DepthStencilState::StencilOp frontFaceDepthFailStencilOp,
    DepthStencilState::StencilOp frontFacePassStencilOp,
    bool enableBackFaceStencil,
    DepthStencilState::CompareFunction backFaceStencilTest,
    DepthStencilState::StencilOp backFaceStencilFailStencilOp,
    DepthStencilState::StencilOp backFaceDepthFailStencilOp,
    DepthStencilState::StencilOp backFacePassStencilOp,
    unsigned char stencilReadMask,
    unsigned char stencilWriteMask
)
{
    this->enableDepthWrite = enableDepthWrite;
    this->depthTest = ConvertCompareFunction(depthTest);
    this->enableFrontFaceStencil = enableFrontFaceStencil;
    this->frontFaceStencilTest = ConvertCompareFunction(frontFaceStencilTest);
    this->frontFaceStencilFailStencilOp = ConvertStencilOp(frontFaceStencilFailStencilOp);
    this->frontFaceDepthFailStencilOp = ConvertStencilOp(frontFaceDepthFailStencilOp);
    this->frontFacePassStencilOp = ConvertStencilOp(frontFacePassStencilOp);
    this->enableBackFaceStencil = enableBackFaceStencil;
    this->backFaceStencilTest = ConvertCompareFunction(backFaceStencilTest);
    this->backFaceStencilFailStencilOp = ConvertStencilOp(backFaceStencilFailStencilOp);
    this->backFaceDepthFailStencilOp = ConvertStencilOp(backFaceDepthFailStencilOp);
    this->backFacePassStencilOp = ConvertStencilOp(backFacePassStencilOp);
    this->stencilReadMask = stencilReadMask;
    this->stencilWriteMask = stencilWriteMask;

    CreateResource();
}

DepthStencilState::~DepthStencilState()
{
    D3D11Utility::Release(depthStencilState);
}

ID3D11DepthStencilState* DepthStencilState::GetDepthStencilState() const
{
    return depthStencilState;
}

D3D11_COMPARISON_FUNC DepthStencilState::ConvertCompareFunction(const CompareFunction compareFunction)
{
    switch (compareFunction)
    {
        case CompareFunction::Less:
            return D3D11_COMPARISON_LESS;
        case CompareFunction::LessEqual:
            return D3D11_COMPARISON_LESS_EQUAL;
        case CompareFunction::Greater:
            return D3D11_COMPARISON_GREATER;
        case CompareFunction::GreaterEqual:
            return D3D11_COMPARISON_GREATER_EQUAL;
        case CompareFunction::Equal:
            return D3D11_COMPARISON_EQUAL;
        case CompareFunction::NotEqual:
            return D3D11_COMPARISON_NOT_EQUAL;
        case CompareFunction::Never:
            return D3D11_COMPARISON_NEVER;
        default:
            return D3D11_COMPARISON_ALWAYS;
    };
}

D3D11_STENCIL_OP DepthStencilState::ConvertStencilOp(const StencilOp stencilOp)
{
    switch (stencilOp)
    {
        case StencilOp::Zero:
            return D3D11_STENCIL_OP_ZERO;
        case StencilOp::Replace:
            return D3D11_STENCIL_OP_REPLACE;
        case StencilOp::SaturatedIncrement:
            return D3D11_STENCIL_OP_INCR_SAT;
        case StencilOp::SaturatedDecrement:
            return D3D11_STENCIL_OP_DECR_SAT;
        case StencilOp::Invert:
            return D3D11_STENCIL_OP_INVERT;
        case StencilOp::Increment:
            return D3D11_STENCIL_OP_INCR;
        case StencilOp::Decrement:
            return D3D11_STENCIL_OP_DECR;
        default:
            return D3D11_STENCIL_OP_KEEP;
    };
}

void DepthStencilState::CreateResource()
{
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

    //Depth part
    depthStencilDesc.DepthEnable = depthTest != D3D11_COMPARISON_ALWAYS || enableDepthWrite;
    depthStencilDesc.DepthWriteMask = enableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.DepthFunc = depthTest;

    //Stencil part
    depthStencilDesc.StencilEnable = enableFrontFaceStencil || enableBackFaceStencil;
    depthStencilDesc.StencilReadMask = stencilReadMask;
    depthStencilDesc.StencilWriteMask = stencilWriteMask;
    depthStencilDesc.FrontFace.StencilFunc = frontFaceStencilTest;
    depthStencilDesc.FrontFace.StencilFailOp = frontFaceStencilFailStencilOp;
    depthStencilDesc.FrontFace.StencilDepthFailOp = frontFaceDepthFailStencilOp;
    depthStencilDesc.FrontFace.StencilPassOp = frontFacePassStencilOp;

    if (enableBackFaceStencil)
    {
        depthStencilDesc.BackFace.StencilFunc = backFaceStencilTest;
        depthStencilDesc.BackFace.StencilFailOp = backFaceStencilFailStencilOp;
        depthStencilDesc.BackFace.StencilDepthFailOp = backFaceDepthFailStencilOp;
        depthStencilDesc.BackFace.StencilPassOp = backFacePassStencilOp;
    }
    else
    {
        depthStencilDesc.BackFace = depthStencilDesc.FrontFace;
    }

    ID3D11Device* d3dDevice = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    HRESULT result = d3dDevice->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create depth stencil state!");
    }
}
