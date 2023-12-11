#pragma once

#include <array>

#include "Shader.h"
#include "RasterizerState.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "Utility/D3D11Utility.h"
#include "RenderTexture.h"
#include "Math/LinearColor.h"
#include "SwapChain.h"

class PipelineState
{
public:
    PipelineState();
    ~PipelineState() = default;
    const unsigned int GetWidth() const;
    const unsigned int GetHeight() const;
    const bool IsValid() const;
    const bool IsGraphics() const;
    const bool IsCompute() const;

    //= STATIC - Will cause PSO generation ===============================================
    Shader* vertexShader = nullptr;
    Shader* pixelShader = nullptr;
    Shader* computeShader = nullptr;
    RasterizerState* rasterizerState = nullptr;
    BlendState* blendState = nullptr;
    DepthStencilState* depthStencilState = nullptr;
    SwapChain* renderTargetSwapchain = nullptr;
    PrimitiveType primitiveType;
    RenderTexture* renderTargetDepthTexture = nullptr;
    std::array<RenderTexture*, maxRenderTargetCount> renderTargetColorTextures;
    //====================================================================================

    //= DYNAMIC - Will not cause PSO generation ===============
    float clearDepth = depthLoad;
    unsigned int clearStencil = stencilLoad;
    std::array<LinearColor, maxRenderTargetCount> clearColor;
    //=========================================================
};
