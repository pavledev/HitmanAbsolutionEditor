#pragma once

#include <d3d11.h>
#include <array>

class BlendState
{
public:
    enum ColorWriteMask
    {
        RED = 1,
        GREEN = 2,
        BLUE = 4,
        ALPHA = 8,

        NONE = 0,
        RGB = RED | GREEN | BLUE,
        RGBA = RED | GREEN | BLUE | ALPHA,
        RG = RED | GREEN,
        BA = BLUE | ALPHA
    };

    enum class BlendOperation
    {
        Add,
        Subtract,
        Min,
        Max,
        ReverseSubtract
    };

    enum class BlendFactor
    {
        Zero,
        One,
        SourceColor,
        InverseSourceColor,
        SourceAlpha,
        InverseSourceAlpha,
        DestAlpha,
        InverseDestAlpha,
        DestColor,
        InverseDestColor,
        ConstantBlendFactor,
        InverseConstantBlendFactor,
        Source1Color,
        InverseSource1Color,
        Source1Alpha,
        InverseSource1Alpha
    };

    struct RenderTarget
    {
        RenderTarget(
            ColorWriteMask colorWriteMask = ColorWriteMask::RGBA,
            BlendOperation colorBlendOp = BlendOperation::Add,
            BlendFactor colorSrcBlend = BlendFactor::One,
            BlendFactor colorDestBlend = BlendFactor::Zero,
            BlendOperation alphaBlendOp = BlendOperation::Add,
            BlendFactor alphaSrcBlend = BlendFactor::One,
            BlendFactor alphaDestBlend = BlendFactor::Zero
        )
            : colorWriteMask(colorWriteMask),
            colorBlendOp(colorBlendOp),
            colorSrcBlend(colorSrcBlend),
            colorDestBlend(colorDestBlend),
            alphaBlendOp(alphaBlendOp),
            alphaSrcBlend(alphaSrcBlend),
            alphaDestBlend(alphaDestBlend)
        {
        }

        ColorWriteMask colorWriteMask;
        BlendOperation colorBlendOp;
        BlendFactor colorSrcBlend;
        BlendFactor colorDestBlend;
        BlendOperation alphaBlendOp;
        BlendFactor alphaSrcBlend;
        BlendFactor alphaDestBlend;
    };

    BlendState(const std::array<RenderTarget, 8>& renderTargets, const bool useAlphaToCoverage = false);
    ~BlendState();
    ID3D11BlendState* GetBlendState() const;
    static D3D11_BLEND_OP ConvertBlendOp(const BlendOperation blendOp);
    static D3D11_BLEND ConvertBlendFactor(const BlendFactor blendFactor);

private:
    void CreateResource();

    std::array<RenderTarget, 8> renderTargets;
    bool useAlphaToCoverage;
    ID3D11BlendState* blendState = nullptr;
};
