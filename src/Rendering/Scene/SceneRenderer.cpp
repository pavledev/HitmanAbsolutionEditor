#include <filesystem>
#include <DirectXColors.h>

#include "directxtk/DirectXHelpers.h"
#include "directxtk/DDSTextureLoader.h"
#include "directxtk/SimpleMath.h"
#include "directxtk/WICTextureLoader.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "backends/imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "Rendering/Scene/SceneRenderer.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/Math.h"
#include "Rendering/Scene/PointLight.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/Scene/Grid.h"
#include "UI/Panels/SceneHierarchyPanel2.h"
#include <directxtk/ScreenGrab.h>
#include <wincodec.h>
#include "Rendering/RHIStaticStates.h"
#include "Rendering/Scene/Model.h"
#include "Editor.h"

std::shared_ptr<Camera> SceneRenderer::GetCamera()
{
    return camera;
}

std::shared_ptr<Entity> SceneRenderer::GetRootEntity()
{
    return rootEntity;
}

void SceneRenderer::Setup(const float width, const float height, std::shared_ptr<RenderPrimitive> renderPrimitive)
{
    CreateConstantBuffers();
    CreateShaders();
    CreateDepthStencilStates();
    CreateRasterizerStates();
    CreateBlendStates();
    CreateRenderTextures(width, height);
    CreateSamplers();

    const ImVec2 viewportPosition = ImGui::GetCursorScreenPos();
    Viewport& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();

    viewport.SetSize(Vector2(width, height));
    viewport.SetPosition(Vector2(viewportPosition.x, viewportPosition.y));
    transformGizmo.Update();
    this->renderPrimitive = renderPrimitive;

    rootEntity = SceneHierarchyPanel2::GetRootEntity();

    const std::vector<std::shared_ptr<Entity>>& children = rootEntity->GetChildren();

    for (size_t i = 0; i < children.size(); ++i)
    {
        if (children[i]->GetComponent<Model>())
        {
            children[i]->GetComponent<Model>()->Initialize(renderPrimitive);

            //Quaternion worldRotation = Quaternion::FromEulerAngles({ -90.f, 0.f, 0.f });

            //children[i]->GetComponent<Model>()->GetTransform()->SetWorldRotation(worldRotation);
        }
        else if (children[i]->GetComponent<Camera>())
        {
            children[i]->GetComponent<Camera>()->Initialize(75.f, width / height, 0.1f, 3000.0f);
            camera = children[i]->GetComponent<Camera>();
        }
        else if (children[i]->GetComponent<Grid>())
        {
            children[i]->GetComponent<Grid>()->Initialize();
        }
        else if (children[i]->GetComponent<PointLight>())
        {
            children[i]->GetComponent<PointLight>()->Initialize();
        }
    }

    /*const float aspectRatio = freeCamera->GetAspectRatio();
    const float verticalFov = freeCamera->GetFOV();
    const float horizontalFOV = 2 * std::atan(std::tan(verticalFov / 2.0f) * (width / height));
    const float wh = std::tan(verticalFov / 2.0f);
    const float hh = (1.0f / aspectRatio) * wh;

    const std::vector<RenderPrimitive::Mesh*>& meshes = renderPrimitive.GetMeshes();
    SVector3 boundingBoxMin = meshes[0]->GetBoundingBoxMin();
    SVector3 boundingBoxMax = meshes[0]->GetBoundingBoxMax();

    for (size_t i = 1; i < meshes.size(); ++i)
    {
        if (meshes[i]->GetBoundingBoxMin() < boundingBoxMin)
        {
            boundingBoxMin = meshes[i]->GetBoundingBoxMin();
        }

        if (meshes[i]->GetBoundingBoxMax() > boundingBoxMax)
        {
            boundingBoxMax = meshes[i]->GetBoundingBoxMax();
        }
    }

    DirectX::BoundingBox boundingBox;
    DirectX::BoundingSphere boundingSphere;

    DirectX::BoundingBox::CreateFromPoints(boundingBox, boundingBoxMin, boundingBoxMax);
    DirectX::BoundingSphere::CreateFromBoundingBox(boundingSphere, boundingBox);

    const float xzExtent = boundingSphere.Radius / tan(horizontalFOV / 2.0f);
    const float yExtent = boundingSphere.Radius / tan(verticalFov / 2.0f);
    const float distance = Math::Max(xzExtent, yExtent) * 2.4f;
    const DirectX::SimpleMath::Vector3 cameraPos = boundingSphere.Center - DirectX::SimpleMath::Vector3(0, 0, distance);

    freeCamera->SetPosition(SVector3(cameraPos.x, cameraPos.y, cameraPos.z));
    freeCamera->Update();
    freeCamera->UpdateView();*/

    /*Transform* transformComponent = renderPrimitive.GetTransform();

    transformComponent->SetRotation({ 90.f, 0.f, 0.f });*/
}

void SceneRenderer::CreateRenderTextures(const float width, const float height)
{
    Color whiteColor = Color(255, 255, 255);

    renderTargets[RendererTextures::FrameOutput] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTexture::Flags::RenderTarget | RenderTexture::Flags::UAV | RenderTexture::Flags::SRV);
    renderTargets[RendererTextures::Depth] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R32_TYPELESS, RenderTexture::Flags::RenderTarget | RenderTexture::Flags::SRV);
    renderTargets[RendererTextures::Blur] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTexture::Flags::UAV | RenderTexture::Flags::SRV);
    renderTargets[RendererTextures::Outline] = std::make_shared<RenderTexture>(width, height, DXGI_FORMAT_R16G16B16A16_FLOAT, RenderTexture::Flags::RenderTarget | RenderTexture::Flags::SRV | RenderTexture::Flags::UAV);
    renderTargets[RendererTextures::WhiteTexture] = std::make_shared<RenderTexture>(1, 1, DXGI_FORMAT_R8G8B8A8_TYPELESS, RenderTexture::Flags::SRV, &whiteColor);
}

void SceneRenderer::CreateConstantBuffers()
{
    uberConstantBufferGpu = std::make_shared<ConstantBuffer>();
    pointLightConstantBufferGpu = std::make_shared<ConstantBuffer>();

    uberConstantBufferGpu->Create<UberConstantBuffer>(30000);
    pointLightConstantBufferGpu->Create<PointLightConstantBuffer>(30000);
}

void SceneRenderer::CreateShaders()
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

    shaders[Shaders::SimpleElementVertex] = std::make_shared<Shader>();
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
    shaders[Shaders::PhongDiffuseSpecularNormalPixel]->Compile(Shader::Type::Pixel, "assets/shaders/PhongDifSpcNrm_PS.hlsl");
}

void SceneRenderer::CreateDepthStencilStates()
{
    TStaticDepthStencilState<false>::InitRHI();
    TStaticDepthStencilState<true, DepthStencilState::CompareFunction::DepthNearOrEqual, true>::InitRHI();
}

void SceneRenderer::CreateRasterizerStates()
{
    TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::None>::InitRHI();
    TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::Clockwise>::InitRHI();
    TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::CounterClockwise>::InitRHI();
    TStaticRasterizerState<RasterizerState::FillMode::Wireframe, RasterizerState::CullMode::None>::InitRHI();
}

void SceneRenderer::CreateBlendStates()
{
    TStaticBlendState<>::InitRHI();
}

void SceneRenderer::CreateSamplers()
{
    TStaticSamplerState<Sampler::Filter::Bilinear>::InitRHI();
    TStaticSamplerState<Sampler::Filter::AnisotropicLinear, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap>::InitRHI();
    TStaticSamplerState<Sampler::Filter::Point, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap>::InitRHI();
}

void SceneRenderer::SetGlobalShaderResources()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Uber, Shader::Type::Vertex | Shader::Type::Pixel | Shader::Type::Compute, uberConstantBufferGpu.get());
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Light, Shader::Type::Vertex | Shader::Type::Pixel, pointLightConstantBufferGpu.get());

    Sampler* bilinearClampSampler = TStaticSamplerState<Sampler::Filter::Bilinear>::GetRHI();
    Sampler* anisotropicWrapSampler = TStaticSamplerState<Sampler::Filter::AnisotropicLinear, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap>::GetRHI();
    Sampler* pointWrapSampler = TStaticSamplerState<Sampler::Filter::Point, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap, Sampler::AddressMode::Wrap>::GetRHI();

    commandList.SetSampler(SamplerRendererBindings::BilinearClamp, bilinearClampSampler);
    commandList.SetSampler(SamplerRendererBindings::AnisotropicWrap, anisotropicWrapSampler);
    commandList.SetSampler(SamplerRendererBindings::PointWrap, pointWrapSampler);
}

void SceneRenderer::OnViewportSizeChanged(const float width, const float height)
{
    for (size_t i = 0; i < renderTargets.size(); ++i)
    {
        renderTargets[i].get()->Resize(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
    }

    camera->SetAspectRatio(width / height);
    camera->UpdateProjection();

    const ImVec2 viewportPosition = ImGui::GetCursorScreenPos();
    Viewport& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();

    viewport.SetSize(Vector2(width, height));
    viewport.SetPosition(Vector2(viewportPosition.x, viewportPosition.y));

    transformGizmo.Update();
}

void SceneRenderer::Render()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();
    uberConstantBufferGpu->ResetOffset();
    pointLightConstantBufferGpu->ResetOffset();

    ID3D11DeviceContext* d3dContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    D3D11_VIEWPORT& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport().GetD3DViewport();
    static float bgColor[4] = { 0.239f, 0.239f, 0.239f, 1.f };
    //static const Color clearColor = Color(0.239f, 0.239f, 0.239f, 1.f);

    ID3D11RenderTargetView* renderTargetView = renderTargets[RendererTextures::FrameOutput].get()->GetRenderTargetView();
    ID3D11DepthStencilView* depthStencilView = renderTargets[RendererTextures::Depth].get()->GetDepthStencilView();
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
        SceneHierarchyPanel2::SetSelectedEntity(camera->GetSelectedEntity());
    }

    transformGizmo.Tick();
    camera->Tick();
}

void SceneRenderer::PostProcessPass()
{
    OutlinePass(renderTargets[RendererTextures::FrameOutput].get());
}

void SceneRenderer::OutlinePass(RenderTexture* outputTexture)
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
    pso.depthStencilState = TStaticDepthStencilState<true, DepthStencilState::CompareFunction::DepthNearOrEqual, true>::GetRHI();
    pso.renderTargetColorTextures[0] = outlineTexture;
    pso.clearColor[0] = clearColor;
    pso.primitiveType = mesh.get()->GetPrimitiveType();

    commandList.SetPipelineState(pso);

    std::shared_ptr<Transform> transform = selectedEntity->GetComponent<Transform>();
    uberConstantBufferCpu.modelViewProjection = transform->GetWorldMatrix() * (camera->GetView() * camera->GetProjection());
    uberConstantBufferCpu.blurRadius = mesh->GetBlurRadius();
    uberConstantBufferCpu.blurSigma = mesh->GetBlurSigma();
    uberConstantBufferCpu.outlineColor = mesh->GetOutlineColor();
    uberConstantBufferCpu.renderTargetResolution = Vector2(static_cast<float>(outlineTexture->GetWidth()), static_cast<float>(outlineTexture->GetHeight()));

    UpdateUberConstantBuffer();

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

    commandList.SetPipelineState(pso2);

    commandList.SetTexture(UnorderedAccessViewRendererBindings::Texture, outputTexture);
    commandList.SetTexture(ShaderResourceViewRendererBindings::Texture, outlineTexture);

    const unsigned int threadGroupCountX = static_cast<unsigned int>(std::ceil(static_cast<float>(outputTexture->GetWidth()) / threadGroupCount));
    const unsigned int threadGroupCountY = static_cast<unsigned int>(std::ceil(static_cast<float>(outputTexture->GetHeight()) / threadGroupCount));

    commandList.Dispatch(threadGroupCountX, threadGroupCountY);

    commandList.UnbindTexture(ShaderResourceViewRendererBindings::Texture);
}

void SceneRenderer::GaussianBlurPass(RenderTexture* inputTexture)
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

        commandList.SetPipelineState(pso);

        uberConstantBufferCpu.blurDirection = Vector2(pixelStride, 0.0f);

        UpdateUberConstantBuffer();

        commandList.SetTexture(UnorderedAccessViewRendererBindings::Texture, blurTexture);
        commandList.SetTexture(ShaderResourceViewRendererBindings::Texture, inputTexture);

        commandList.Dispatch(threadGroupCountX, threadGroupCountY);
    }

    // Vertical pass
    {
        static PipelineState pso;
        pso.computeShader = computeShader;

        commandList.SetPipelineState(pso);

        uberConstantBufferCpu.blurDirection = Vector2(0.0f, pixelStride);

        UpdateUberConstantBuffer();

        commandList.UnbindTexture(ShaderResourceViewRendererBindings::Texture);

        commandList.SetTexture(UnorderedAccessViewRendererBindings::Texture, inputTexture);
        commandList.SetTexture(ShaderResourceViewRendererBindings::Texture, blurTexture);

        commandList.Dispatch(threadGroupCountX, threadGroupCountY);
    }
}

void SceneRenderer::UpdateUberConstantBuffer()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    uberConstantBufferGpu->Update(&uberConstantBufferCpu);

    // Bind because the offset just changed
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Uber, Shader::Type::Vertex | Shader::Type::Pixel | Shader::Type::Compute, uberConstantBufferGpu.get());
}

void SceneRenderer::UpdateLightConstantBuffer()
{
    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();

    pointLightConstantBufferGpu->Update(&pointLightConstantBufferCpu);

    // Bind because the offset just changed
    commandList.SetConstantBuffer(ConstantBufferRendererBindings::Light, Shader::Type::Vertex | Shader::Type::Pixel, pointLightConstantBufferGpu.get());
}

UberConstantBuffer& SceneRenderer::GetUberConstantBufferCpu()
{
    return uberConstantBufferCpu;
}

PointLightConstantBuffer& SceneRenderer::GetPointLightConstantBufferCpu()
{
    return pointLightConstantBufferCpu;
}

std::shared_ptr<Shader> SceneRenderer::GetShader(const Shaders shader)
{
    return shaders[shader];
}
