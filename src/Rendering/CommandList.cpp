#include "Rendering/CommandList.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/Scene/SceneRenderer.h"
#include "Editor.h"

void CommandList::SetPipelineState(PipelineState& pipelineState, bool setRenderTargets)
{
    //assert(pipelineState.IsValid() && "Pipeline state is invalid");
    this->pipelineState = pipelineState;

    UnbindOutputTextures();
    SetInputLayout();
    SetVertexShader();
    SetPixelShader();
    SetComputeShader();
    SetBlendState();
    SetDepthStencilState();
    SetRasterizerState();
    SetPrimitiveTopology();

    if (setRenderTargets)
    {
        SetRenderTargets();
    }

    // Viewport
    //Viewport viewport = Viewport(0.0f, 0.0f, pipelineState.GetWidth(), pipelineState.GetHeight());
    SetViewport(Editor::GetInstance().GetDirectXRenderer()->GetViewport().GetD3DViewport());

    ClearPipelineStateRenderTargets(pipelineState);

    SceneRenderer::SetGlobalShaderResources();
}

void CommandList::ClearPipelineStateRenderTargets(PipelineState& pipelineState)
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    for (unsigned char i = 0; i < maxRenderTargetCount; i++)
    {
        if (pipelineState.clearColor[i] != colorLoad)
        {
            if (pipelineState.renderTargetSwapchain)
            {
                deviceContext->ClearRenderTargetView
                (
                    pipelineState.renderTargetSwapchain->GetRenderTargetView(),
                    pipelineState.clearColor[i].Data()
                );
            }
            else if (pipelineState.renderTargetColorTextures[i])
            {
                deviceContext->ClearRenderTargetView
                (
                    pipelineState.renderTargetColorTextures[i]->GetRenderTargetView(),
                    pipelineState.clearColor[i].Data()
                );
            }
        }
    }

    if (pipelineState.renderTargetDepthTexture)
    {
        UINT clearFlags = 0;
        clearFlags |= (pipelineState.clearDepth != depthLoad) ? D3D11_CLEAR_DEPTH : 0;
        clearFlags |= (pipelineState.clearStencil != stencilLoad) ? D3D11_CLEAR_STENCIL : 0;

        if (clearFlags != 0)
        {
            deviceContext->ClearDepthStencilView
            (
                pipelineState.renderTargetDepthTexture->GetDepthStencilView(),
                clearFlags,
                pipelineState.clearDepth,
                static_cast<UINT8>(pipelineState.clearStencil)
            );
        }
    }
}

void CommandList::ClearRenderTarget(RenderTexture* texture,
    const unsigned int colorIndex,
    const unsigned int depthStencilIndex,
    const bool storage,
    const LinearColor& clearColor,
    const float clearDepth,
    const unsigned int clearStencil
)
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    if (storage)
    {
        assert(texture->IsUAV());

        if (clearColor == colorLoad)
        {
            return;
        }

        deviceContext->ClearUnorderedAccessViewFloat(texture->GetUnorderedAccessView(), clearColor.Data());
    }
    else if (texture->IsRenderTargetColor() || texture->IsRenderTargetDepthStencil())
    {
        if (texture->IsRenderTargetColor())
        {
            if (clearColor == colorLoad)
            {
                return;
            }

            deviceContext->ClearRenderTargetView
            (
                texture->GetRenderTargetView(),
                clearColor.Data()
            );
        }
        else if (texture->IsRenderTargetDepthStencil())
        {
            UINT clearFlags = 0;
            clearFlags |= (clearDepth != depthLoad) ? D3D11_CLEAR_DEPTH : 0;
            clearFlags |= (clearStencil != stencilLoad) ? D3D11_CLEAR_STENCIL : 0;

            if (clearFlags != 0)
            {
                deviceContext->ClearDepthStencilView
                (
                    texture->GetDepthStencilView(),
                    clearFlags,
                    clearDepth,
                    static_cast<UINT8>(clearStencil)
                );
            }
        }
    }
}

void CommandList::DrawPrimitive(const unsigned int baseVertexIndex, const unsigned int vertexCount, const unsigned int instanceCount)
{
    /*ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    unsigned int vertexCount = D3D11Utility::GetVertexCountForPrimitiveCount(primitiveCount, pipelineState.primitiveType);

    if (instanceCount > 1)
    {
        deviceContext->DrawInstanced(vertexCount, instanceCount, baseVertexIndex, 0);
    }
    else
    {
        deviceContext->Draw(vertexCount, baseVertexIndex);
    }*/

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    if (instanceCount > 1)
    {
        deviceContext->DrawInstanced(vertexCount, instanceCount, baseVertexIndex, 0);
    }
    else
    {
        deviceContext->Draw(vertexCount, baseVertexIndex);
    }
}

void CommandList::DrawIndexedPrimitive(const IndexBuffer* indexBuffer, const unsigned int baseVertexIndex, const unsigned int firstInstance, const unsigned int startIndex, const unsigned int instanceCount)
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    const unsigned int indexCount = indexBuffer->GetIndexCount();

    // Verify that we are not trying to read outside the index buffer range
    // test is an optimized version of: StartIndex + IndexCount <= IndexBuffer->GetSize() / indexBuffer.GetStride()
    if ((startIndex + indexCount) * indexBuffer->GetStride() > indexBuffer->GetGPUSize())
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Reading outside the index buffer range!");

        return;
    }

    if (instanceCount > 1 || firstInstance != 0)
    {
        const unsigned long long totalIndexCount = static_cast<unsigned long long>(instanceCount) * static_cast<unsigned long long>(indexCount) + static_cast<unsigned long long>(startIndex);

        if (totalIndexCount > static_cast<unsigned long long>(0xFFFFFFFF))
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Instanced Index Draw exceeds maximum D3D11 limit!");

            return;
        }

        deviceContext->DrawIndexedInstanced(indexCount, instanceCount, startIndex, baseVertexIndex, firstInstance);
    }
    else
    {
        deviceContext->DrawIndexed(indexCount, startIndex, baseVertexIndex);
    }
}

void CommandList::Dispatch(unsigned int x, unsigned int y, unsigned int z)
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    deviceContext->Dispatch(x, y, z);

    // Make sure to clean the compute shader UAV slots after dispatching.
    // If we try to bind the resource but it's still bound as a computer shader output the runtime will automatically set the it to null.
    const void* resource_array[8] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
    deviceContext->CSSetUnorderedAccessViews(0, 8, reinterpret_cast<ID3D11UnorderedAccessView* const*>(&resource_array), nullptr);
}

void CommandList::SetViewport(const D3D11_VIEWPORT& viewport) const
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    deviceContext->RSSetViewports(1, &viewport);
}

void CommandList::SetVertexBuffer(const VertexBuffer* vertexBuffer)
{
    assert(vertexBuffer != nullptr);
    assert(vertexBuffer->GetBuffer() != nullptr);

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    ID3D11Buffer* buffer = vertexBuffer->GetBuffer();
    UINT stride = vertexBuffer->GetStride();
    UINT offsets[] = { 0 };

    ID3D11Buffer* setBuffer = nullptr;
    UINT setStride = vertexBuffer->GetStride();
    UINT setOffset = 0;

    deviceContext->IAGetVertexBuffers(0, 1, &setBuffer, &setStride, &setOffset);

    if (setBuffer == buffer)
    {
        return;
    }

    deviceContext->IASetVertexBuffers(0, 1, &buffer, &stride, offsets);
}

void CommandList::SetIndexBuffer(const IndexBuffer* indexBuffer)
{
    assert(indexBuffer != nullptr);
    assert(indexBuffer->GetBuffer() != nullptr);

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    ID3D11Buffer* buffer = indexBuffer->GetBuffer();
    const DXGI_FORMAT format = indexBuffer->Is16Bit() ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

    ID3D11Buffer* setBuffer = nullptr;
    DXGI_FORMAT setFormat = DXGI_FORMAT_UNKNOWN;
    UINT setOffset = 0;

    deviceContext->IAGetIndexBuffer(&setBuffer, &setFormat, &setOffset);

    if (setBuffer == buffer)
    {
        return;
    }

    deviceContext->IASetIndexBuffer(buffer, format, 0);
}

void CommandList::SetConstantBuffer(const ConstantBufferRendererBindings slot, const unsigned char scope, ConstantBuffer* constantBuffer) const
{
    ID3D11Buffer* buffer = constantBuffer ? constantBuffer->GetBuffer() : nullptr;
    const UINT startSlot = static_cast<UINT>(slot);
    const UINT range = 1;
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    if (scope & Shader::Type::Vertex)
    {
        ID3D11Buffer* setBuffer = nullptr;

        deviceContext->VSGetConstantBuffers(startSlot, range, &setBuffer);

        if (setBuffer != buffer)
        {
            deviceContext->VSSetConstantBuffers(startSlot, range, &buffer);
        }
    }

    if (scope & Shader::Type::Pixel)
    {
        ID3D11Buffer* setBuffer = nullptr;

        deviceContext->PSGetConstantBuffers(startSlot, range, &setBuffer);

        if (setBuffer != buffer)
        {
            deviceContext->PSSetConstantBuffers(startSlot, range, &buffer);
        }
    }

    if (scope & Shader::Type::Compute)
    {
        ID3D11Buffer* setBuffer = nullptr;

        deviceContext->CSGetConstantBuffers(startSlot, range, &setBuffer);

        if (setBuffer != buffer)
        {
            deviceContext->CSSetConstantBuffers(startSlot, range, &buffer);
        }
    }
}

void CommandList::SetSampler(const SamplerRendererBindings slot, Sampler* sampler) const
{
    ID3D11SamplerState* samplerState = sampler->GetSamplerState();
    const UINT startSlot = static_cast<UINT>(slot);
    const UINT range = 1;
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    ID3D11SamplerState* setSamplerState = nullptr;

    if (pipelineState.IsCompute())
    {
        deviceContext->CSGetSamplers(startSlot, range, &setSamplerState);

        if (setSamplerState != samplerState)
        {
            deviceContext->CSSetSamplers(startSlot, range, &samplerState);
        }
    }
    else
    {
        deviceContext->PSGetSamplers(startSlot, range, &setSamplerState);

        if (setSamplerState != samplerState)
        {
            deviceContext->PSSetSamplers(startSlot, range, &samplerState);
        }
    }
}

void CommandList::SetTexture(const ShaderResourceViewRendererBindings slot, RenderTexture* texture)
{
    SetTexture(static_cast<unsigned int>(slot), texture, false);
}

void CommandList::SetTexture(const UnorderedAccessViewRendererBindings slot, RenderTexture* texture)
{
    SetTexture(static_cast<unsigned int>(slot), texture, true);
}

void CommandList::SetTexture(const unsigned int slot, RenderTexture* texture, const bool uav)
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    if (uav)
    {
        ID3D11UnorderedAccessView* unorderedAccessView = texture->GetUnorderedAccessView();
        ID3D11UnorderedAccessView* setUnorderedAccessView = nullptr;

        deviceContext->CSGetUnorderedAccessViews(slot, 1, &setUnorderedAccessView);

        if (setUnorderedAccessView != unorderedAccessView)
        {
            deviceContext->CSSetUnorderedAccessViews(slot, 1, &unorderedAccessView, nullptr);

            // Keep track of output textures
            outputTextures[outputTexturesIndex].texture = texture;
            outputTextures[outputTexturesIndex].slot = slot;
            outputTexturesIndex++;
        }
    }
    else
    {
        ID3D11ShaderResourceView* shaderResourceView = texture->GetShaderResourceView();
        ID3D11ShaderResourceView* setShaderResourceView = nullptr;

        if (pipelineState.IsCompute())
        {
            deviceContext->CSGetShaderResources(slot, 1, &setShaderResourceView);

            if (setShaderResourceView != shaderResourceView)
            {
                deviceContext->CSSetShaderResources(slot, 1, &shaderResourceView);
            }
        }
        else
        {
            deviceContext->PSGetShaderResources(slot, 1, &setShaderResourceView);

            if (setShaderResourceView != shaderResourceView)
            {
                deviceContext->PSSetShaderResources(slot, 1, &shaderResourceView);
            }
        }
    }
}

void CommandList::UnbindTexture(const ShaderResourceViewRendererBindings slot)
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    ID3D11ShaderResourceView* nullSRV = nullptr;

    deviceContext->CSSetShaderResources(static_cast<UINT>(slot), 1, &nullSRV);
}

void CommandList::SetInputLayout()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11InputLayout* inputLayout = pipelineState.vertexShader ? pipelineState.vertexShader->GetInputLayout()->GetInputLayout() : nullptr;

    // Current state
    ID3D11InputLayout* inputLayoutSet = nullptr;
    deviceContext->IAGetInputLayout(&inputLayoutSet);

    // Set if dirty
    if (inputLayoutSet != inputLayout)
    {
        deviceContext->IASetInputLayout(inputLayout);
    }
}

void CommandList::SetVertexShader()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11VertexShader* shader = static_cast<ID3D11VertexShader*>(pipelineState.vertexShader ? pipelineState.vertexShader->GetResource() : nullptr);

    // Current state
    ID3D11VertexShader* setShader = nullptr;
    UINT instanceCount = 256;
    ID3D11ClassInstance* instances[256];
    deviceContext->VSGetShader(&setShader, instances, &instanceCount);

    // Set if dirty
    if (setShader != shader)
    {
        deviceContext->VSSetShader(shader, nullptr, 0);
    }
}

void CommandList::SetPixelShader()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11PixelShader* shader = static_cast<ID3D11PixelShader*>(pipelineState.pixelShader ? pipelineState.pixelShader->GetResource() : nullptr);

    // Current state
    ID3D11PixelShader* setShader = nullptr;
    UINT instanceCount = 256;
    ID3D11ClassInstance* instances[256];
    deviceContext->PSGetShader(&setShader, instances, &instanceCount);

    // Set if dirty
    if (setShader != shader)
    {
        deviceContext->PSSetShader(shader, nullptr, 0);
    }
}

void CommandList::SetComputeShader()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11ComputeShader* shader = static_cast<ID3D11ComputeShader*>(pipelineState.computeShader ? pipelineState.computeShader->GetResource() : nullptr);

    // Current state
    ID3D11ComputeShader* setShader = nullptr;
    UINT instanceCount = 256;
    ID3D11ClassInstance* instances[256];
    deviceContext->CSGetShader(&setShader, instances, &instanceCount);

    // Set if dirty
    if (setShader != shader)
    {
        deviceContext->CSSetShader(shader, nullptr, 0);
    }
}

void CommandList::SetBlendState()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11BlendState* blendStateSet = nullptr;
    std::array<FLOAT, 4> blendFactorSet = { 0.0f };
    UINT maskSet = 0;
    deviceContext->OMGetBlendState(&blendStateSet, blendFactorSet.data(), &maskSet);

    // Current state
    ID3D11BlendState* blendState = pipelineState.blendState ? pipelineState.blendState->GetBlendState() : nullptr;
    std::array<FLOAT, 4> blendFactor = { 1.f, 1.f, 1.f, 1.f };
    const UINT mask = 0xFFFFFFFF;

    // Set if dirty
    if (blendStateSet != blendState || blendFactorSet != blendFactor || maskSet != mask)
    {
        deviceContext->OMSetBlendState(blendState, blendFactor.data(), mask);
    }
}

void CommandList::SetDepthStencilState()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11DepthStencilState* depthStencilState = pipelineState.depthStencilState ? pipelineState.depthStencilState->GetDepthStencilState() : nullptr;

    // Current state
    ID3D11DepthStencilState* depthStencilStateSet = nullptr;
    UINT stencilRef = 0;
    deviceContext->OMGetDepthStencilState(&depthStencilStateSet, &stencilRef);

    // Set if dirty
    if (depthStencilStateSet != depthStencilState)
    {
        deviceContext->OMSetDepthStencilState(depthStencilState, 1);
    }
}

void CommandList::SetRasterizerState()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // New state
    ID3D11RasterizerState* rasterizerState = pipelineState.rasterizerState ? pipelineState.rasterizerState->GetRasterizerState() : nullptr;

    // Current state
    ID3D11RasterizerState* rasterizerStateSet = nullptr;

    // Set if dirty
    deviceContext->RSGetState(&rasterizerStateSet);

    if (rasterizerStateSet != rasterizerState)
    {
        deviceContext->RSSetState(rasterizerState);
    }
}

void CommandList::SetPrimitiveTopology()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    D3D11_PRIMITIVE_TOPOLOGY primitiveTopology = D3D11Utility::GetD3D11PrimitiveType(pipelineState.primitiveType);

    // New state
    const D3D11_PRIMITIVE_TOPOLOGY topology = primitiveTopology;

    // Current state
    D3D11_PRIMITIVE_TOPOLOGY topologySet;
    deviceContext->IAGetPrimitiveTopology(&topologySet);

    // Set if dirty
    if (topologySet != topology)
    {
        deviceContext->IASetPrimitiveTopology(primitiveTopology);
    }
}

void CommandList::SetRenderTargets()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    ID3D11DepthStencilView* depthStencil = nullptr;

    if (pipelineState.renderTargetDepthTexture)
    {
        assert(pipelineState.renderTargetDepthTexture->IsRenderTargetDepthStencil());

        depthStencil = pipelineState.renderTargetDepthTexture->GetDepthStencilView();
    }

    std::array<ID3D11RenderTargetView*, maxRenderTargetCount> renderTargets = { nullptr };

    if (pipelineState.renderTargetSwapchain)
    {
        renderTargets[0] = { pipelineState.renderTargetSwapchain->GetRenderTargetView() };
    }
    else
    {
        for (unsigned char i = 0; i < maxRenderTargetCount; i++)
        {
            if (pipelineState.renderTargetColorTextures[i])
            {
                assert(pipelineState.renderTargetColorTextures[i]->IsRenderTargetColor());

                ID3D11RenderTargetView* renderTarget = pipelineState.renderTargetColorTextures[i]->GetRenderTargetView();

                renderTargets[i] = renderTarget;
            }
        }
    }

    // Current state
    std::array<ID3D11RenderTargetView*, maxRenderTargetCount> setRenderTargetViews = { nullptr };
    ID3D11DepthStencilView* setDepthStencilView = nullptr;

    deviceContext->OMGetRenderTargets(maxRenderTargetCount, setRenderTargetViews.data(), &setDepthStencilView);

    // Set if dirty
    if (renderTargets != setRenderTargetViews || depthStencil != setDepthStencilView)
    {
        UINT renderTargetCount = 0;

        for (ID3D11RenderTargetView* renderTarget : renderTargets)
        {
            if (renderTarget)
            {
                renderTargetCount++;
            }
        }

        deviceContext->OMSetRenderTargets(renderTargetCount, renderTargets.data(), depthStencil);
    }
}

void CommandList::UnbindOutputTextures()
{
    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    std::array<void*, resourceArrayLengthMax> resources;
    resources.fill(nullptr);

    for (OutputTexture& texture : outputTextures)
    {
        if (texture.texture)
        {
            deviceContext->CSSetUnorderedAccessViews(texture.slot, 1, reinterpret_cast<ID3D11UnorderedAccessView* const*>(resources.data()), nullptr);

            texture.texture = nullptr;
        }
    }

    outputTexturesIndex = 0;
}
