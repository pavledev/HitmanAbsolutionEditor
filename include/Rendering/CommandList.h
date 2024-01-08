#pragma once

#include <atomic>

#include "PipelineState.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Sampler.h"
#include "ConstantBuffer.h"
#include "RendererBindings.h"

class Renderer3D;

class CommandList
{
public:
    CommandList() = default;
    ~CommandList() = default;

    void SetPipelineState(PipelineState& pipelineState, Renderer3D* renderer3D, bool setRenderTargets = true);
    void ClearPipelineStateRenderTargets(PipelineState& pipelineState);
    void ClearRenderTarget(
        RenderTexture* texture,
        const unsigned int colorIndex = 0,
        const unsigned int depthStencilIndex = 0,
        const bool storage = false,
        const LinearColor& clearColor = colorLoad,
        const float clearDepth = depthLoad,
        const unsigned int clearStencil = stencilLoad
    );
    void DrawPrimitive(const unsigned int baseVertexIndex, const unsigned int primitiveCount, const unsigned int instanceCount);
    void DrawIndexedPrimitive(const IndexBuffer* indexBuffer, const unsigned int baseVertexIndex, const unsigned int firstInstance, const unsigned int startIndex, const unsigned int instanceCount);
    void Dispatch(unsigned int x, unsigned int y, unsigned int z = 1);
    void SetViewport(const D3D11_VIEWPORT& viewport) const;
    void SetVertexBuffer(const VertexBuffer* vertexBuffer);
    void SetIndexBuffer(const IndexBuffer* indexBuffer);
    void SetConstantBuffer(const ConstantBufferRendererBindings slot, const unsigned char scope, ConstantBuffer* constantBuffer) const;
    void SetSampler(const SamplerRendererBindings slot, Sampler* sampler) const;
    void SetTexture(const ShaderResourceViewRendererBindings slot, RenderTexture* texture);
    void SetTexture(const UnorderedAccessViewRendererBindings slot, RenderTexture* texture);
    void SetTexture(const unsigned int slot, RenderTexture* texture, const bool uav = false);
    void UnbindTexture(const ShaderResourceViewRendererBindings slot);
    void SetInputLayout();
    void SetVertexShader();
    void SetPixelShader();
    void SetComputeShader();
    void SetBlendState();
    void SetDepthStencilState();
    void SetRasterizerState();
    void SetPrimitiveTopology();
    void SetRenderTargets();

private:
    void UnbindOutputTextures();

    PipelineState pipelineState;

    // Keep track of output textures so that we can unbind them and prevent
    // D3D11 warnings when trying to bind them as SRVs in following passes
    struct OutputTexture
    {
        RenderTexture* texture = nullptr;
        unsigned int slot;
    };

    static const unsigned char resourceArrayLengthMax = 16;
    std::array<OutputTexture, resourceArrayLengthMax> outputTextures;
    unsigned int outputTexturesIndex = 0;
};
