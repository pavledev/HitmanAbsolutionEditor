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

    hasDiffuseMap = false;
    hasNormalMap = false;
    hasSpecularMap = false;
    hasEmissiveMap = false;
    hasAlphaMap = false;

    meshColor = { 0.721f, 0.709f, 0.709f, 1.0f };
}

void Mesh::Initialize(std::shared_ptr<RenderPrimitive::Mesh> mesh, std::shared_ptr<RenderMaterialInstance> matiResource)
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
    const std::vector<unsigned short>& indices = mesh->GetIndices();

    vertexPositions.resize(vertices.size());
    this->indices.resize(indices.size());

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        VertexPosition vertexPosition;

        vertexPosition.position = Vector3(vertices[i].position);

        vertexPositions[i] = vertexPosition;
    }

    for (size_t i = 0; i < indices.size(); ++i)
    {
        this->indices[i] = indices[i];
    }

    CreateGpuBuffers(mesh);
    CreateMaterial(matiResource);

    vertexShader = Renderer3D::GetShader(Renderer3D::Shaders::MeshDefaultVertex);
    pixelShader = Renderer3D::GetShader(Renderer3D::Shaders::MeshBlinnPhongPixel);

    primitiveType = PrimitiveType::TriangleList;
}

    vertexShader = Renderer3D::GetShader(Renderer3D::Shaders::MeshDefaultVertex);
    pixelShader = Renderer3D::GetShader(Renderer3D::Shaders::MeshBlinnPhongPixel);

    primitiveType = PrimitiveType::TriangleList;
}

const std::vector<VertexPosition>& Mesh::GetVertexPositions() const
{
    return vertexPositions;
}

const std::vector<unsigned int>& Mesh::GetIndices() const
{
    return indices;
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

const BoundingBox& Mesh::GetBoundingBox(const BoundingBoxType type)
{
    if (previousTransform != GetTransform()->GetWorldMatrix())
    {
        Matrix transform = GetTransform()->GetWorldMatrix();

        boundingBox = boundingBoxUntransformed.Transform(transform);
        previousTransform = transform;
    }

    if (type == BoundingBoxType::Untransformed)
    {
        return boundingBoxUntransformed;
    }
    else if (type == BoundingBoxType::Transformed)
    {
        return boundingBox;
    }

    return BoundingBox::Undefined;
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

void Mesh::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
    this->renderer3D = renderer3D;
}

void Mesh::CreateGpuBuffers(std::shared_ptr<RenderPrimitive::Mesh> mesh)
{
    mesh->CreateVertexBuffer();

    vertexBuffer = std::make_shared<VertexBuffer>();
    vertexBuffer->Create(mesh->GetVertexBuffer(), mesh->GetVertexCount(), mesh->GetStride());

    indexBuffer = std::make_shared<IndexBuffer>();
    indexBuffer->Create(mesh->GetIndices());
}

void Mesh::CreateBoundingBox(const std::shared_ptr<RenderPrimitive::Mesh> mesh)
{
    const Vector3 boundingBoxMin = mesh->GetBoundingBoxMin();
    const Vector3 boundingBoxMax = mesh->GetBoundingBoxMax();

    boundingBoxUntransformed = BoundingBox(boundingBoxMin, boundingBoxMax);
    boundingBox = boundingBoxUntransformed.Transform(GetTransform()->GetWorldMatrix());
}

void Mesh::CreateMaterial(std::shared_ptr<RenderMaterialInstance> matiResource)
{
    std::shared_ptr<RenderMaterialInstance> matiResource2;

    if (matiResource->GetHash() == 0x00983D6FA01A9AE9) //hitman_face_standard
    {
        matiResource2 = std::static_pointer_cast<RenderMaterialInstance>(ResourceUtility::CreateResource("MATI"));

        matiResource2->SetHash(0x00C2F21BB84A3AFF); //hitman_01_face
    }
    else
    {
        matiResource2 = matiResource;
    }

    const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(matiResource2->GetHash());
    std::vector<RenderMaterialInstance::Texture> textures;

    if (matiResource->GetHash() == 0x00983D6FA01A9AE9) //hitman_face_standard
    {
        matiResource2->SetResourceID(referenceInfo.resourceID);
    }

    matiResource2->SetHeaderLibraries(&referenceInfo.headerLibraries);
    matiResource2->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, true, false, true);
    matiResource2->Deserialize();
    matiResource2->GetTextures(matiResource, textures);

    std::string materialResourceName = ResourceUtility::GetResourceName(matiResource2->GetResourceID());
    material = Material(materialResourceName);

    std::vector<std::shared_ptr<Resource>>& matiReferences = matiResource2->GetReferences();

    for (size_t i = 0; i < textures.size(); ++i)
    {
        unsigned int textureReferenceIndex = textures[i].textureReferenceIndex;
        std::string textureResourceID = matiReferences[textureReferenceIndex]->GetResourceID();
        std::string textureResourceName = ResourceUtility::GetResourceName(textureResourceID);
        std::shared_ptr<Texture> textReference = std::static_pointer_cast<Texture>(matiReferences[textures[i].textureReferenceIndex]);
        const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(textReference->GetHash());

        textures[i].blob = new DirectX::Blob();
        textures[i].name = std::format("{}.dds", textureResourceName);

        textReference->SetHeaderLibraries(&referenceInfo.headerLibraries);
        textReference->LoadResource(0, referenceInfo.headerLibraries[0].chunkIndex, referenceInfo.headerLibraries[0].indexInLibrary, false, false, true);
        textReference->Deserialize();
        textReference->ConvertTextureToDDS(textures[i].blob);

        textReference->DeleteResourceData();

        material.AddTexture(textures[i]);

        delete textures[i].blob;
    }

    matiResource2->DeleteResourceData();

    if (material.HasNormalTexture())
    {
        hasNormalMap = true;
    }

    if (material.HasSpecularTexture())
    {
        hasSpecularMap = true;
    }

    if (material.HasEmissiveTexture())
    {
        hasEmissiveMap = true;
    }

    if (material.HasAlphaTexture())
    {
        hasAlphaMap = true;
    }
}

void Mesh::Render()
{
    if (!renderer3D)
    {
        return;
    }

    if (!isLODInRange)
    {
        return;
    }

    MeshConstantBuffer& meshConstantBufferCpu = renderer3D->GetMeshConstantBufferCpu();
    Matrix worldView = GetTransform()->GetWorldMatrix() * renderer3D->GetCamera()->GetView();
    //Quaternion rotation = Quaternion::FromEulerAngles(-90.f, 0.f, 0.f);
    //Matrix worldView = (GetTransform()->GetWorldMatrix() * Matrix(Vector3(0.f, 0.f, 0.f), rotation, Vector3(1.f, 1.f, 1.f))) * SceneRenderer::GetCamera()->GetView();

    meshConstantBufferCpu.materialDiffuse = Vector4(1.f, 1.f, 1.f, 1.f);
    meshConstantBufferCpu.materialAmbient = Vector4(1.f, 1.f, 1.f, 1.f);
    meshConstantBufferCpu.materialEmissive = Vector4(0.f, 0.f, 0.f, 1.f);
    meshConstantBufferCpu.materialReflect = Vector4(0.f, 0.f, 0.f, 1.f);
    meshConstantBufferCpu.materialSpecular = Vector4(0.f, 0.f, 0.f, 1.f);

    meshConstantBufferCpu.world = GetTransform()->GetWorldMatrix();
    meshConstantBufferCpu.modelViewProjection = worldView * renderer3D->GetCamera()->GetProjection();
    meshConstantBufferCpu.hasDiffuseMap = true;
    meshConstantBufferCpu.hasNormalMap = hasNormalMap;
    meshConstantBufferCpu.hasSpecularMap = hasSpecularMap;
    meshConstantBufferCpu.hasEmissiveMap = hasEmissiveMap;
    meshConstantBufferCpu.hasAlphaMap = hasAlphaMap;

    std::shared_ptr<Entity> selectedEntity = renderer3D->GetCamera()->GetSelectedEntity();

    if (selectedEntity && selectedEntity->GetComponent<Mesh>().get() == this)
    {
        meshConstantBufferCpu.meshColor = outlineColor;
    }
    else
    {
        meshConstantBufferCpu.meshColor = meshColor;
    }

    renderer3D->UpdateMeshConstantBuffer();

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

    //pipelineState.depthStencilState = TStaticDepthStencilState<true, DepthStencilState::CompareFunction::DepthNearOrEqual, true>::GetRHI();
    pipelineState.depthStencilState = TStaticDepthStencilState<true, DepthStencilState::CompareFunction::Less>::GetRHI();
    pipelineState.primitiveType = primitiveType;

    commandList.SetPipelineState(pipelineState, renderer3D.get(), false);
    commandList.SetVertexBuffer(vertexBuffer.get());

    if (indices.size() > 0)
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

    if (indices.size() > 0)
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
    static std::vector<UI::TableColumn> tableColumns;

    if (tableColumns.empty())
    {
        tableColumns.push_back({ "Name" , 0, 1.f });
        tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
    }

    if (primitiveType == PrimitiveType::LineList)
    {
        UI::BeginProperties("MeshProperties", tableColumns, false);

        UI::ColorRGBAProperty("Mesh Color", meshColor);

        UI::EndProperties();
    }

    if (ImGui::TreeNodeEx("Outline", treeNodeFlags))
    {
        UI::BeginProperties("OutlineProperties", tableColumns, false);

        UI::Property("Blur Radius", blurRadius);
        UI::Property("Blur Sigma", blurSigma);
        UI::ColorRGBAProperty("Outline Color", outlineColor);

        UI::EndProperties();

        ImGui::TreePop();
    }
}

void Mesh::SetWireframe(const bool wireframe)
{
    this->wireframe = wireframe;
}

void Mesh::SetIsLODInRange(const bool isLODInRange)
{
    this->isLODInRange = isLODInRange;
}
