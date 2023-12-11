#include "Rendering/PipelineState.h"

PipelineState::PipelineState()
{
    renderTargetColorTextures.fill(nullptr);
}

const unsigned int PipelineState::GetWidth() const
{
    if (renderTargetSwapchain)
    {
        return renderTargetSwapchain->GetWidth();
    }

    if (renderTargetColorTextures[0])
    {
        return renderTargetColorTextures[0]->GetWidth();
    }

    if (renderTargetDepthTexture)
    {
        return renderTargetDepthTexture->GetWidth();
    }

    return 0;
}

const unsigned int PipelineState::GetHeight() const
{
    if (renderTargetSwapchain)
    {
        return renderTargetSwapchain->GetHeight();
    }

    if (renderTargetColorTextures[0])
    {
        return renderTargetColorTextures[0]->GetHeight();
    }

    if (renderTargetDepthTexture)
    {
        return renderTargetDepthTexture->GetHeight();
    }

    return 0;
}

const bool PipelineState::IsValid() const
{
    // Deduce states
    bool hasComputeShader = computeShader ? computeShader->IsCompiled() : false;
    bool hasVertexShader = vertexShader ? vertexShader->IsCompiled() : false;
    bool hasPixelShader = pixelShader ? pixelShader->IsCompiled() : false;
    bool hasRenderTarget = renderTargetColorTextures[0] || renderTargetDepthTexture; // Check that there is at least one render target
    bool hasBackbuffer = renderTargetSwapchain;                                        // Check that no both the swapchain and the color render target are active
    bool hasGraphicsStates = rasterizerState && blendState && depthStencilState;
    bool isGraphicsPSO = (hasVertexShader || hasPixelShader) && !hasComputeShader;
    bool isComputePSO = hasComputeShader && (!hasVertexShader && !hasPixelShader);

    // There must be at least one shader
    if (!hasComputeShader && !hasVertexShader && !hasPixelShader)
    {
        return false;
    }

    // If this is a graphics pso then there must he graphics states
    if (isGraphicsPSO && !hasGraphicsStates)
    {
        return false;
    }

    // If this is a graphics pso then there must be a render target
    if (isGraphicsPSO && !hasRenderTarget && !hasBackbuffer)
    {
        if (!hasRenderTarget && !hasBackbuffer)
        {
            return false;
        }

        if (hasRenderTarget && hasBackbuffer)
        {
            return false;
        }
    }

    return true;
}

const bool PipelineState::IsGraphics() const
{
    return (vertexShader != nullptr || pixelShader != nullptr) && !computeShader;
}

const bool PipelineState::IsCompute() const
{
    return computeShader != nullptr && !IsGraphics();
}
