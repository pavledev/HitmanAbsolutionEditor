#include "Utility/UI.h"
#include "Rendering/Scene/Mesh.h"
#include "Rendering/DirectXRenderer.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Utility/ResourceUtility.h"
#include "Rendering/Scene/Transform.h"
#include "Utility/Math.h"
#include "Rendering/RHIStaticStates.h"
#include "Resources/Texture.h"
#include "Editor.h"

Mesh::Mesh(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
    lodMask = 0;
    isLODInRange = true;
    wireframe = false;

    blurRadius = 30.f;
    blurSigma = 32.f;
    outlineColor = Vector4(0.96f, 0.6f, 0.0f, 1.f);

    useGlossAlpha = false;
    useSpecularMap = false;
    useNormalMap = false;
    normalMapWeight = 1.0f;
    materialColor = { 0.f, 0.f, 0.f };
    specularColor = { 0.18f, 0.18f, 0.18f };
    specularWeight = 1.0f;
    specularGloss = 8.0f;
}

void Mesh::Initialize(RenderPrimitive::Mesh* mesh, std::shared_ptr<Resource> matiReference)
{
    lodMask = mesh->GetLODMask();

    CreateBoundingBox(mesh);

    /*constexpr float angle = DirectX::XMConvertToRadians(-90.0f);
    Vector3 rotationCenter = boundingBox.GetCenter();
    Vector3 axis = Vector3(1.0f, 0.0f, 0.0f);
    Matrix translationMatrix = Matrix::Translate(Matrix::Identity, -rotationCenter);
    Matrix rotationMatrix = Matrix::Rotate(Matrix::Identity, angle, axis);
    Matrix reverseTranslationMatrix = Matrix::Translate(Matrix::Identity, rotationCenter);

    std::vector<RenderPrimitive::Vertex>& vertices = mesh->GetVertices();

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        Vector3 rotatedPosition(reverseTranslationMatrix * rotationMatrix * translationMatrix * Vector4(vertices[i].position, 1.0f));

        vertices[i].position = rotatedPosition;
    }*/

    /*std::vector<RenderPrimitive::Vertex>& vertices = mesh->GetVertices();

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].position = Vector3(vertices[i].position.x, -vertices[i].position.z, vertices[i].position.y);
    }*/

    const std::vector<RenderPrimitive::Vertex>& vertices = mesh->GetVertices();

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        VertexPosition vertexPosition;

        vertexPosition.position = Vector3(vertices[i].position);

        vertexPositions.push_back(vertexPosition);
    }

    indices = &mesh->GetIndices();

    CreateGpuBuffers(mesh);
    CreateMaterial(mesh, matiReference);

    std::string shaderFileName = "PhongDif";

    /*if (material.GetTexture(Material::TextureType::Diffuse)->HasAlpha())
    {
        shaderFileName += "Msk";
    }*/

    if (material.HasSpecularTexture())
    {
        shaderFileName += "Spc";
    }

    if (material.HasNormalTexture())
    {
        shaderFileName += "Nrm";
    }

    std::string vertexShaderFilePath = std::format("assets/shaders/{}_VS.hlsl", shaderFileName);
    std::string pixelShaderFilePath = std::format("assets/shaders/{}_PS.hlsl", shaderFileName);

    vertexShader = std::make_shared<Shader>();
    vertexShader->Compile(Shader::Type::Vertex, vertexShaderFilePath, VertexType::PosUvNorTan);

    pixelShader = std::make_shared<Shader>();
    pixelShader->Compile(Shader::Type::Pixel, pixelShaderFilePath);

    primitiveType = PrimitiveType::TriangleList;
}

const std::vector<VertexPosition>& Mesh::GetVertexPositions() const
{
    return vertexPositions;
}

const std::vector<unsigned short>& Mesh::GetIndices() const
{
    return *indices;
}

const VertexBuffer* Mesh::GetVertexBuffer() const
{
    return vertexBuffer.get();
}

const IndexBuffer* Mesh::GetIndexBuffer() const
{
    return indexBuffer.get();
}

Shader* Mesh::GetVertexShader() const
{
    return vertexShader.get();
}

Shader* Mesh::GetPixelShader() const
{
    return pixelShader.get();
}

const BoundingBox& Mesh::GetAABB()
{
    // Updated if dirty
    if (lastTransform != GetTransform()->GetWorldMatrix() || !aabb.Defined())
    {
        aabb = boundingBox.Transform(GetTransform()->GetWorldMatrix());
        lastTransform = GetTransform()->GetWorldMatrix();
    }

    return aabb;
}

const unsigned char Mesh::GetLODMask() const
{
    return lodMask;
}

const float Mesh::GetBlurRadius() const
{
    return blurRadius;
}

const float Mesh::GetBlurSigma() const
{
    return blurSigma;
}

const Vector4& Mesh::GetOutlineColor() const
{
    return outlineColor;
}

const PrimitiveType Mesh::GetPrimitiveType() const
{
    return primitiveType;
}

void Mesh::CreateGpuBuffers(RenderPrimitive::Mesh* mesh)
{
    mesh->CreateVertexBuffer();

    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->Create(mesh->GetVertexBuffer(), mesh->GetVertexCount(), mesh->GetStride());

    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->Create(mesh->GetIndices());
}

void Mesh::CreateBoundingBox(const RenderPrimitive::Mesh* mesh)
{
    const Vector3 boundingBoxMin = mesh->GetBoundingBoxMin();
    const Vector3 boundingBoxMax = mesh->GetBoundingBoxMax();

    boundingBox = BoundingBox(boundingBoxMin, boundingBoxMax);
}

void Mesh::CreateMaterial(const RenderPrimitive::Mesh* mesh, std::shared_ptr<Resource> matiResource)
{
    const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(matiResource->GetHash());
    RenderMaterialInstance renderMaterialInstance;
    std::vector<RenderMaterialInstance::Texture> textures;

    matiResource->SetHeaderLibraries(&referenceInfo.headerLibraries);
    matiResource->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, true, false, true);

    //renderMaterialInstance.Deserialize(matiResource->GetResourceData(), matiResource->GetResourceDataSize());
    renderMaterialInstance.GetTextures(matiResource, textures);

    std::string materialResourceName = ResourceUtility::GetResourceName(matiResource->GetResourceID());
    material = Material(materialResourceName);

    std::vector<std::shared_ptr<Resource>>& matiReferences = matiResource->GetReferences();

    for (size_t i = 0; i < textures.size(); ++i)
    {
        unsigned int textureReferenceIndex = textures[i].textureReferenceIndex;
        std::string textureResourceID = matiReferences[textureReferenceIndex]->GetResourceID();
        std::string textureResourceName = ResourceUtility::GetResourceName(textureResourceID);
        std::shared_ptr<Resource> textReference = matiReferences[textures[i].textureReferenceIndex];
        const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(textReference->GetHash());
        Texture texture;

        textures[i].blob = new DirectX::Blob();
        textures[i].name = std::format("{}.dds", textureResourceName);

        textReference->SetHeaderLibraries(&referenceInfo.headerLibraries);
        textReference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
        //texture.Deserialize(textReference->GetResourceData(), textReference->GetResourceDataSize());
        texture.ConvertTextureToDDS(textures[i].blob);

        textReference->DeleteResourceData();

        material.AddTexture(textures[i]);

        delete textures[i].blob;
    }

    matiResource->DeleteResourceData();

    if (material.HasNormalTexture())
    {
        useNormalMap = true;
    }

    if (material.HasSpecularTexture())
    {
        if (material.GetTexture(Material::TextureType::Specular)->HasAlpha())
        {
            useGlossAlpha = true;
        }

        useSpecularMap = true;
    }
}

void Mesh::Render()
{
    if (!isLODInRange)
    {
        return;
    }

    UberConstantBuffer& uberConstantBufferCpu = SceneRenderer::GetUberConstantBufferCpu();
    Matrix44 worldView = GetTransform()->GetWorldMatrix() * SceneRenderer::GetCamera()->GetView();
    //Quaternion rotation = Quaternion::FromEulerAngles(-90.f, 0.f, 0.f);
    //Matrix44 worldView = (GetTransform()->GetWorldMatrix() * Matrix44(Vector3(0.f, 0.f, 0.f), rotation, Vector3(1.f, 1.f, 1.f))) * SceneRenderer::GetCamera()->GetView();

    uberConstantBufferCpu.model = GetTransform()->GetWorldMatrix();
    uberConstantBufferCpu.modelView = worldView;
    uberConstantBufferCpu.modelViewProjection = worldView * SceneRenderer::GetCamera()->GetProjection();
    uberConstantBufferCpu.projection = SceneRenderer::GetCamera()->GetProjection();
    uberConstantBufferCpu.sphereMatrix = worldView.Inverted();
    uberConstantBufferCpu.useGlossAlpha = useGlossAlpha;
    uberConstantBufferCpu.useSpecularMap = useSpecularMap;
    uberConstantBufferCpu.useNormalMap = useNormalMap;
    uberConstantBufferCpu.materialColor = materialColor;
    uberConstantBufferCpu.specularColor = specularColor;
    uberConstantBufferCpu.specularWeight = specularWeight;
    uberConstantBufferCpu.specularGloss = specularGloss;
    uberConstantBufferCpu.normalMapWeight = normalMapWeight;

    SceneRenderer::UpdateUberConstantBuffer();

    CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();
    static PipelineState pipelineState;

    pipelineState.vertexShader = vertexShader.get();
    pipelineState.pixelShader = pixelShader.get();

    if (wireframe)
    {
        pipelineState.rasterizerState = TStaticRasterizerState<RasterizerState::FillMode::Wireframe, RasterizerState::CullMode::None>::GetRHI();
    }
    else
    {
        if (material.HasDiffuseTexture() && material.GetTexture(Material::TextureType::Diffuse)->HasAlpha())
        {
            pipelineState.rasterizerState = TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::None>::GetRHI();
        }
        else
        {
            pipelineState.rasterizerState = TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::CounterClockwise>::GetRHI();
        }
    }

    if (material.HasDiffuseTexture() && material.GetTexture(Material::TextureType::Diffuse)->HasAlpha())
    {
        pipelineState.blendState = TStaticBlendState<
            BlendState::ColorWriteMask::RGBA,
            BlendState::BlendOperation::Add,
            BlendState::BlendFactor::SourceAlpha,
            BlendState::BlendFactor::InverseSourceAlpha,
            BlendState::BlendOperation::Add,
            BlendState::BlendFactor::One,
            BlendState::BlendFactor::One>::GetRHI();
    }
    else
    {
        pipelineState.blendState = TStaticBlendState<>::GetRHI();
    }

    pipelineState.depthStencilState = TStaticDepthStencilState<true, DepthStencilState::CompareFunction::DepthNearOrEqual, true>::GetRHI();
    pipelineState.primitiveType = primitiveType;

    commandList.SetPipelineState(pipelineState, false);
    commandList.SetVertexBuffer(vertexBuffer.get());

    if (indices)
    {
        commandList.SetIndexBuffer(indexBuffer.get());
    }

    if (material.HasDiffuseTexture())
    {
        commandList.SetTexture(ShaderResourceViewRendererBindings::DiffuseTexture, material.GetTexture(Material::TextureType::Diffuse));
    }

    if (material.HasNormalTexture())
    {
        commandList.SetTexture(ShaderResourceViewRendererBindings::NormalTexture, material.GetTexture(Material::TextureType::Normal));
    }

    if (material.HasSpecularTexture())
    {
        commandList.SetTexture(ShaderResourceViewRendererBindings::SpecularTexture, material.GetTexture(Material::TextureType::Specular));
    }

    if (indices)
    {
        commandList.DrawIndexedPrimitive(indexBuffer.get(), 0, 0, 0, 1);
    }
    else
    {
        commandList.DrawPrimitive(0, vertexBuffer.get()->GetVertexCount(), 1);
    }
}

void Mesh::RenderProperties()
{
    static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
        | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

    //if (ImGui::TreeNodeEx("Outline", treeNodeFlags))
    //{
    //    UI::BeginProperties();

    //    UI::Property("Blur Radius", blurRadius);
    //    UI::Property("Blur Sigma", blurSigma);
    //    //UI::PropertyVector("Outline Color", outlineColor, true);

    //    UI::EndProperties();

    //    ImGui::TreePop();
    //}
}

void Mesh::SetWireframe(const bool wireframe)
{
    this->wireframe = wireframe;
}

void Mesh::SetIsLODInRange(const bool isLODInRange)
{
    this->isLODInRange = isLODInRange;
}
