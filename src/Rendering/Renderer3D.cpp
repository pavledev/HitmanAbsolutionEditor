#include <filesystem>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "backends/imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Rendering/Renderer3D.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/Math.h"
#include "Rendering/Scene/Mesh.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/RHIStaticStates.h"
#include "Editor.h"

void Renderer3D::Initialize()
{
    if (!shaders[0])
    {
        CreateShaders();
        CreateDepthStencilStates();
        CreateRasterizerStates();
        CreateBlendStates();
        CreateSamplers();
    }

    CreateConstantBuffers();

    isRendererSetup = true;
}

std::shared_ptr<Camera> Renderer3D::GetCamera()
{
    return camera;
}

std::shared_ptr<Entity> Renderer3D::GetRootEntity()
{
    return rootEntity;
}

void Renderer3D::SetCamera(std::shared_ptr<Camera> camera)
{
    this->camera = camera;
    transformGizmo.SetCamera(camera);
}

void Renderer3D::SetRootEntity(std::shared_ptr<Entity> rootEntity)
{
    this->rootEntity = rootEntity;
}

void Renderer3D::CreateRenderTextures(const float width, const float height)
{
    Color whiteColor = Color(255, 255, 255);

    renderTargets[RendererTextures::FrameOutput] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTexture::Flags::RenderTarget | RenderTexture::Flags::UAV | RenderTexture::Flags::SRV);
    renderTargets[RendererTextures::Depth] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R32_TYPELESS, RenderTexture::Flags::RenderTarget | RenderTexture::Flags::SRV);
    renderTargets[RendererTextures::Blur] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTexture::Flags::UAV | RenderTexture::Flags::SRV);
    renderTargets[RendererTextures::Outline] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTexture::Flags::RenderTarget | RenderTexture::Flags::SRV | RenderTexture::Flags::UAV);
    renderTargets[RendererTextures::WhiteTexture] = std::make_shared<RenderTexture>(1, 1, DXGI_FORMAT_R8G8B8A8_TYPELESS, RenderTexture::Flags::SRV, &whiteColor);
}

void Renderer3D::CreateConstantBuffers()
{
    cameraConstantBufferGpu = std::make_shared<ConstantBuffer>();
    meshConstantBufferGpu = std::make_shared<ConstantBuffer>();
    lightConstantBufferGpu = std::make_shared<ConstantBuffer>();

    cameraConstantBufferGpu->Create<CameraConstantBuffer>(1);
    meshConstantBufferGpu->Create<MeshConstantBuffer>(1);
    lightConstantBufferGpu->Create<LightConstantBuffer>(1);
}

void Renderer3D::CreateShaders()
{
    shaders[Shaders::BlurGaussianCompute] = std::make_shared<Shader>();
    shaders[Shaders::BlurGaussianCompute]->AddDefine("PASS_BLUR_GAUSSIAN");
    shaders[Shaders::BlurGaussianCompute]->Compile(Shader::Type::Compute, "assets/shaders/Blur.hlsl");

    shaders[Shaders::OutlineVertex] = std::make_shared<Shader>();
    shaders[Shaders::OutlineVertex]->Compile(Shader::Type::Vertex, "assets/shaders/Outline.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::OutlinePixel] = std::make_shared<Shader>();
    shaders[Shaders::OutlinePixel]->Compile(Shader::Type::Pixel, "assets/shaders/Outline.hlsl");
    shaders[Shaders::OutlineCompute] = std::make_shared<Shader>();
    shaders[Shaders::OutlineCompute]->Compile(Shader::Type::Compute, "assets/shaders/Outline.hlsl");

    /*shaders[Shaders::SimpleElementVertex] = std::make_shared<Shader>();
    shaders[Shaders::SimpleElementVertex]->Compile(Shader::Type::Vertex, "assets/shaders/SimpleElementVertexShader.hlsl", VertexType::PosUvCol);
    shaders[Shaders::SimpleElementPixel] = std::make_shared<Shader>();
    shaders[Shaders::SimpleElementPixel]->Compile(Shader::Type::Pixel, "assets/shaders/SimpleElementPixelShader.hlsl");

    shaders[Shaders::SolidVertex] = std::make_shared<Shader>();
    shaders[Shaders::SolidVertex]->Compile(Shader::Type::Vertex, "assets/shaders/Solid_VS.hlsl", VertexType::Pos);
    shaders[Shaders::SolidPixel] = std::make_shared<Shader>();
    shaders[Shaders::SolidPixel]->Compile(Shader::Type::Pixel, "assets/shaders/Solid_PS.hlsl");

    shaders[Shaders::PhongVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongVertex]->Compile(Shader::Type::Vertex, "assets/shaders/Phong_VS.hlsl", VertexType::PosNor);
    shaders[Shaders::PhongPixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongPixel]->Compile(Shader::Type::Pixel, "assets/shaders/Phong_PS.hlsl");

    shaders[Shaders::PhongDiffuseVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseVertex]->Compile(Shader::Type::Vertex, "assets/shaders/PhongDif_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::PhongDiffusePixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffusePixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDif_PS.hlsl");

    shaders[Shaders::PhongDiffuseMaskSpecularVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseMaskSpecularVertex]->Compile(Shader::Type::Vertex, "assets/shaders/PhongDifMskSpc_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::PhongDiffuseMaskSpecularPixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseMaskSpecularPixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDifMskSpc_PS.hlsl");

    shaders[Shaders::PhongDiffuseMaskSpecularNormalVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseMaskSpecularNormalVertex]->Compile(Shader::Type::Vertex, "assets/shaders/PhongDifMskSpcNrm_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::PhongDiffuseMaskSpecularNormalPixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseMaskSpecularNormalPixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDifMskSpcNrm_PS.hlsl");

    shaders[Shaders::PhongDiffuseNormalVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseNormalVertex]->Compile(Shader::Type::Vertex, "assets/shaders/PhongDifNrm_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::PhongDiffuseNormalPixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseNormalPixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDifNrm_PS.hlsl");

    shaders[Shaders::PhongDiffuseSpecularVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseSpecularVertex]->Compile(Shader::Type::Vertex, "assets/shaders/PhongDifSpc_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::PhongDiffuseSpecularPixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseSpecularPixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDifSpc_PS.hlsl");

    shaders[Shaders::PhongDiffuseSpecularNormalVertex] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseSpecularNormalVertex]->Compile(Shader::Type::Vertex, "assets/shaders/PhongDifSpcNrm_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::PhongDiffuseSpecularNormalPixel] = std::make_shared<Shader>();
    shaders[Shaders::PhongDiffuseSpecularNormalPixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDifSpcNrm_PS.hlsl");*/

    shaders[Shaders::MeshDefaultVertex] = std::make_shared<Shader>();
    shaders[Shaders::MeshDefaultVertex]->Compile(Shader::Type::Vertex, "assets/shaders/MeshDefault_VS.hlsl", VertexType::PosUvNorTan);
    shaders[Shaders::MeshBlinnPhongPixel] = std::make_shared<Shader>();
    shaders[Shaders::MeshBlinnPhongPixel]->Compile(Shader::Type::Pixel, "assets/shaders/MeshBlinnPhong_PS.hlsl");

    shaders[Shaders::SimpleVertex] = std::make_shared<Shader>();
    shaders[Shaders::SimpleVertex]->Compile(Shader::Type::Vertex, "assets/shaders/Simple_VS.hlsl", VertexType::Pos);
    shaders[Shaders::SimplePixel] = std::make_shared<Shader>();
    shaders[Shaders::SimplePixel]->Compile(Shader::Type::Pixel, "assets/shaders/Simple_PS.hlsl");
}

void Renderer3D::CreateDepthStencilStates()
{
    TStaticDepthStencilState<false>::InitRHI();
    TStaticDepthStencilState<true, DepthStencilState::CompareFunction::DepthNearOrEqual, true>::InitRHI();
}

void Renderer3D::CreateRasterizerStates()
{
    TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::None>::InitRHI();
    TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::Clockwise>::InitRHI();
    TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::CounterClockwise>::InitRHI();
    TStaticRasterizerState<RasterizerState::FillMode::Wireframe, RasterizerState::CullMode::None>::InitRHI();
}

void Renderer3D::CreateBlendStates()
{
    TStaticBlendState<>::InitRHI();
}

void Renderer3D::CreateSamplers()
{
    TStaticSamplerState<Sampler::Filter::MinMagLinearMipPoint, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap, Sampler::AddressMode::Clamp, 0.f, 0>::InitRHI();
    TStaticSamplerState<Sampler::Filter::MinMagLinearMipPoint>::InitRHI();
}

void Renderer3D::SetGlobalShaderResources()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Camera, Shader::Type::Vertex | Shader::Type::Pixel | Shader::Type::Compute, cameraConstantBufferGpu.get());
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Mesh, Shader::Type::Vertex | Shader::Type::Pixel | Shader::Type::Compute, meshConstantBufferGpu.get());
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Light, Shader::Type::Vertex | Shader::Type::Pixel, lightConstantBufferGpu.get());

    Sampler* surfaceSampler = TStaticSamplerState<Sampler::Filter::MinMagLinearMipPoint, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap, Sampler::AddressMode::Clamp, 0.f, 0>::GetRHI();
    Sampler* bilinearClampSampler = TStaticSamplerState<Sampler::Filter::MinMagLinearMipPoint>::GetRHI();

    commandList.SetSampler(SamplerRendererBindings::Surface, surfaceSampler);
    commandList.SetSampler(SamplerRendererBindings::BilinearClamp, bilinearClampSampler);
}

void Renderer3D::OnViewportSizeChanged(const float width, const float height)
{
    if (!renderTargets[0])
    {
        CreateRenderTextures(width, height);
    }

    for (size_t i = 0; i < renderTargets.size(); ++i)
    {
        renderTargets[i]->Resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    camera->SetAspectRatio(width / height);
    camera->UpdateProjection();

    const ImVec2 viewportPosition = ImGui::GetCursorScreenPos();
    Viewport& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();

    viewport.SetSize(Vector2(width, height));
    viewport.SetPosition(Vector2(viewportPosition.x, viewportPosition.y));

    transformGizmo.Update();
}

void Renderer3D::Render()
{
    if (!isRendererSetup || !renderTargets[0])
    {
        return;
    }

    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();
    cameraConstantBufferGpu->ResetOffset();
    meshConstantBufferGpu->ResetOffset();
    lightConstantBufferGpu->ResetOffset();

    ID3D11DeviceContext* d3dContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    D3D11_VIEWPORT& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport().GetD3DViewport();
    static float bgColor[4] = { 0.239f, 0.239f, 0.239f, 1.f };
    //static const Color clearColor = Color(0.239f, 0.239f, 0.239f, 1.f);

    ID3D11RenderTargetView* renderTargetView = renderTargets[RendererTextures::FrameOutput]->GetRenderTargetView();
    ID3D11DepthStencilView* depthStencilView = renderTargets[RendererTextures::Depth]->GetDepthStencilView();
    Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->ClearRenderTargetView(renderTargetView, bgColor);
    Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

    d3dContext->RSSetViewports(1, &viewport);

    shaderResourceView = renderTargets[RendererTextures::FrameOutput]->GetShaderResourceView();

    ImGui::Image(shaderResourceView, ImVec2(viewport.Width, viewport.Height));

    rootEntity->Render();
    PostProcessPass();

    if (ImGui::IsMouseClicked(0) && ImGui::IsItemHovered() && TransformGizmo::IsPickingAllowed())
    {
        camera->Pick();

        selectedEntityCallback(camera->GetSelectedEntity());
    }

    transformGizmo.Tick();
    camera->Tick();
}

void Renderer3D::PostProcessPass()
{
    OutlinePass(renderTargets[RendererTextures::FrameOutput].get());
}

void Renderer3D::OutlinePass(RenderTexture* outputTexture)
{
    Shader* vertexShader = shaders[Shaders::OutlineVertex].get();
    Shader* pixelShader = shaders[Shaders::OutlinePixel].get();
    Shader* computeShader = shaders[Shaders::OutlineCompute].get();

    if (!vertexShader->IsCompiled() || !pixelShader->IsCompiled() || !computeShader->IsCompiled())
    {
        return;
    }

    const std::shared_ptr<Entity> selectedEntity = camera->GetSelectedEntity();

    if (!selectedEntity)
    {
        return;
    }

    std::shared_ptr<Mesh> mesh = selectedEntity->GetComponent<Mesh>();

    if (!mesh)
    {
        return;
    }

    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();
    RenderTexture* outlineTexture = renderTargets[RendererTextures::Outline].get();
    static const Color clearColor = Color(0.0f, 0.0f, 0.0f, 0.0f);

    static PipelineState pso;
    pso.vertexShader = vertexShader;
    pso.pixelShader = pixelShader;
    pso.rasterizerState = TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::CounterClockwise>::GetRHI();
    pso.blendState = TStaticBlendState<>::GetRHI();
    pso.depthStencilState = TStaticDepthStencilState<true, DepthStencilState::CompareFunction::Less>::GetRHI();
    pso.renderTargetColorTextures[0] = outlineTexture;
    pso.clearColor[0] = clearColor;
    pso.primitiveType = mesh->GetPrimitiveType();

    commandList.SetPipelineState(pso, this);

    std::shared_ptr<Transform> transform = selectedEntity->GetComponent<Transform>();
    meshConstantBufferCpu.world = transform->GetWorldMatrix();
    meshConstantBufferCpu.modelViewProjection = transform->GetWorldMatrix() * (camera->GetView() * camera->GetProjection());
    meshConstantBufferCpu.blurRadius = mesh->GetBlurRadius();
    meshConstantBufferCpu.blurSigma = mesh->GetBlurSigma();
    meshConstantBufferCpu.outlineColor = mesh->GetOutlineColor();
    meshConstantBufferCpu.renderTargetResolution = Vector2(static_cast<float>(outlineTexture->GetWidth()), static_cast<float>(outlineTexture->GetHeight()));

    UpdateMeshConstantBuffer();

    commandList.SetVertexBuffer(mesh->GetVertexBuffer());

    if (mesh->GetIndexBuffer())
    {
        commandList.SetIndexBuffer(mesh->GetIndexBuffer());
        commandList.DrawIndexedPrimitive(mesh->GetIndexBuffer(), 0, 0, 0, 1);
    }
    else
    {
        commandList.DrawPrimitive(0, mesh->GetVertexBuffer()->GetVertexCount(), 1);
    }

    // Blur the color silhouette
    GaussianBlurPass(outlineTexture);

    // Combine color silhouette with frame
    static PipelineState pso2;
    pso2.computeShader = computeShader;

    commandList.SetPipelineState(pso2, this);

    commandList.SetTexture(UnorderedAccessViewRendererBindings::Texture, outputTexture);
    commandList.SetTexture(ShaderResourceViewRendererBindings::Texture, outlineTexture);

    const unsigned int threadGroupCountX = static_cast<unsigned int>(std::ceil(static_cast<float>(outputTexture->GetWidth()) / threadGroupCount));
    const unsigned int threadGroupCountY = static_cast<unsigned int>(std::ceil(static_cast<float>(outputTexture->GetHeight()) / threadGroupCount));

    commandList.Dispatch(threadGroupCountX, threadGroupCountY);

    commandList.UnbindTexture(ShaderResourceViewRendererBindings::Texture);
}

void Renderer3D::GaussianBlurPass(RenderTexture* inputTexture)
{
    Shader* computeShader = shaders[Shaders::BlurGaussianCompute].get();
    ID3D11DeviceContext* d3dContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    if (!computeShader->IsCompiled())
    {
        return;
    }

    const float pixelStride = 1.0f;
    RenderTexture* blurTexture = renderTargets[RendererTextures::Blur].get();
    const unsigned int threadGroupCountX = static_cast<unsigned int>(std::ceil(static_cast<float>(inputTexture->GetWidth()) / threadGroupCount));
    const unsigned int threadGroupCountY = static_cast<unsigned int>(std::ceil(static_cast<float>(inputTexture->GetHeight()) / threadGroupCount));

    // Horizontal pass
    {
        static PipelineState pso;
        pso.computeShader = computeShader;

        commandList.SetPipelineState(pso, this);

        meshConstantBufferCpu.blurDirection = Vector2(pixelStride, 0.0f);

        UpdateMeshConstantBuffer();

        commandList.SetTexture(UnorderedAccessViewRendererBindings::Texture, blurTexture);
        commandList.SetTexture(ShaderResourceViewRendererBindings::Texture, inputTexture);

        commandList.Dispatch(threadGroupCountX, threadGroupCountY);
    }

    // Vertical pass
    {
        static PipelineState pso;
        pso.computeShader = computeShader;

        commandList.SetPipelineState(pso, this);

        meshConstantBufferCpu.blurDirection = Vector2(0.0f, pixelStride);

        UpdateMeshConstantBuffer();

        commandList.UnbindTexture(ShaderResourceViewRendererBindings::Texture);

        commandList.SetTexture(UnorderedAccessViewRendererBindings::Texture, inputTexture);
        commandList.SetTexture(ShaderResourceViewRendererBindings::Texture, blurTexture);

        commandList.Dispatch(threadGroupCountX, threadGroupCountY);
    }
}

void Renderer3D::UpdateCameraConstantBuffer()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    cameraConstantBufferGpu->Update(&cameraConstantBufferCpu);

    // Bind because the offset just changed
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Camera, Shader::Type::Vertex | Shader::Type::Pixel | Shader::Type::Compute, cameraConstantBufferGpu.get());
}

void Renderer3D::UpdateMeshConstantBuffer()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    meshConstantBufferGpu->Update(&meshConstantBufferCpu);

    // Bind because the offset just changed
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Mesh, Shader::Type::Vertex | Shader::Type::Pixel | Shader::Type::Compute, meshConstantBufferGpu.get());
}

void Renderer3D::UpdateLightConstantBuffer()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    lightConstantBufferGpu->Update(&lightConstantBufferCpu);

    // Bind because the offset just changed
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Light, Shader::Type::Vertex | Shader::Type::Pixel, lightConstantBufferGpu.get());
}

CameraConstantBuffer& Renderer3D::GetCameraConstantBufferCpu()
{
    return cameraConstantBufferCpu;
}

MeshConstantBuffer& Renderer3D::GetMeshConstantBufferCpu()
{
    return meshConstantBufferCpu;
}

LightConstantBuffer& Renderer3D::GetLightConstantBufferCpu()
{
    return lightConstantBufferCpu;
}

std::shared_ptr<Shader> Renderer3D::GetShader(const Shaders shader)
{
    return shaders[shader];
}

void Renderer3D::SetSelectedEntityCallback(SelectedEntityCallback selectedEntityCallback)
{
    this->selectedEntityCallback = selectedEntityCallback;
}
