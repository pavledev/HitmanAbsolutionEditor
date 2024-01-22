#include "Physics/NxConvexShapeDesc.h"
#include "Physics/NxConvexMesh.h"
#include "Physics/NxTriangleMeshShapeDesc.h"
#include "Physics/NxTriangleMesh.h"

#include "Rendering/Scene/Collision.h"
#include "Rendering/Scene/Geometry.h"
#include "Rendering/Scene/Transform.h"

Collision::Collision(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
}

void Collision::Initialize(std::shared_ptr<RenderPrimitive> renderPrimitive)
{
    std::shared_ptr<Physics> physics = renderPrimitive->GetPhysics();
    const std::vector<G2NxShapeDesc*>& shapeDescriptors = physics->GetCollisionShape().GetShapeDescriptors();

    for (size_t i = 0; i < shapeDescriptors.size(); ++i)
    {
        NxShapeDesc* shapeDescriptor = shapeDescriptors[i]->m_pDesc;
        const NxShapeType shapeType = shapeDescriptor->getType();

        switch (shapeType)
        {
            case NX_SHAPE_BOX:
            {
                CreateBoxMesh(shapeDescriptor, meshes[i]);

                break;
            }
            case NX_SHAPE_SPHERE:
            {
                CreateSphereMesh(shapeDescriptor, meshes[i]);

                break;
            }
            case NX_SHAPE_CAPSULE:
            {
                CreateCapsuleMesh(shapeDescriptor, meshes[i]);

                break;
            }
            case NX_SHAPE_CONVEX:
            {
                CreateConvexMesh(shapeDescriptor, meshes[i]);

                break;
            }
            case NX_SHAPE_MESH:
            {
                CreateTriangleMesh(shapeDescriptor, meshes[i]);

                break;
            }
            default:
            {
                Logger::GetInstance().Log(Logger::Level::Error, "Shape type isn't supported!");

                break;
            }
        }

        meshes[i]->GetTransform()->SetParent(GetTransform());

        GetTransform()->AddChild(meshes[i]->GetTransform());
    }
}

void Collision::AddMesh(std::shared_ptr<Mesh> mesh)
{
    meshes.push_back(mesh);
}

const std::vector<std::shared_ptr<Mesh>>& Collision::GetMeshes() const
{
    return meshes;
}

void Collision::Render()
{
}

void Collision::RenderProperties()
{
}

void Collision::CreateBoxMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> boxMesh)
{
    NxBoxShapeDesc* boxShapeDescriptor = static_cast<NxBoxShapeDesc*>(shapeDescriptor);
    static std::vector<VertexPosition> cubeVertices;
    static std::vector<unsigned short> cubeIndices;

    if (cubeVertices.empty())
    {
        Geometry::CreateCube(cubeVertices, cubeIndices);
    }

    boxMesh->Initialize(cubeVertices, cubeIndices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector4(1.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);

    NxQuat rotation;

    boxShapeDescriptor->localPose.M.toQuat(rotation);

    boxMesh->GetTransform()->SetLocalPosition(boxShapeDescriptor->localPose.t);
    boxMesh->GetTransform()->SetLocalRotation(rotation);
    boxMesh->GetTransform()->SetLocalScale(boxShapeDescriptor->dimensions);
}

void Collision::CreateSphereMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> sphereMesh)
{
    NxSphereShapeDesc* sphereShapeDescriptor = static_cast<NxSphereShapeDesc*>(shapeDescriptor);
    std::vector<VertexPosition> sphereVertices;
    std::vector<unsigned short> sphereIndices;

    Geometry::CreateSphere(sphereVertices, sphereIndices, sphereShapeDescriptor->radius);

    sphereMesh->Initialize(sphereVertices, sphereIndices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector4(1.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);

    NxQuat rotation;

    sphereShapeDescriptor->localPose.M.toQuat(rotation);

    sphereMesh->GetTransform()->SetLocalPosition(sphereShapeDescriptor->localPose.t);
    sphereMesh->GetTransform()->SetLocalRotation(rotation);
}

void Collision::CreateCapsuleMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> capsuleMesh)
{
    NxCapsuleShapeDesc* capsuleShapeDescriptor = static_cast<NxCapsuleShapeDesc*>(shapeDescriptor);
    std::vector<VertexPosition> vertices;
    std::vector<unsigned short> indices;
    const float halfHeight = capsuleShapeDescriptor->height / 2.f;

    static const Vector3 base = Vector3(0.f, 0.f, 0.f);
    static const Vector3 xAxis = Vector3(1.f, 0.f, 0.f);
    static const Vector3 yAxis = Vector3(0.f, 1.f, 0.f);
    static const Vector3 zAxis = Vector3(0.f, 0.f, 1.f);

    Geometry::CreateWireCapsule(vertices, base, xAxis, yAxis, zAxis, capsuleShapeDescriptor->radius, halfHeight, 16);

    capsuleMesh->Initialize(vertices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector4(1.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);

    NxQuat rotation;

    capsuleShapeDescriptor->localPose.M.toQuat(rotation);

    capsuleMesh->GetTransform()->SetLocalPosition(capsuleShapeDescriptor->localPose.t);
    capsuleMesh->GetTransform()->SetLocalRotation(rotation);
}

void Collision::CreateConvexMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> convexMesh)
{
    NxConvexShapeDesc* convexShapeDescriptor = static_cast<NxConvexShapeDesc*>(shapeDescriptor);
    NxConvexMesh* convexMesh2 = convexShapeDescriptor->meshData;
    const NxU32 vertexCount = convexMesh2->getCount(0, NX_ARRAY_VERTICES);
    const NxVec3* vertexBase = static_cast<const NxVec3*>(convexMesh2->getBase(0, NX_ARRAY_VERTICES));
    std::vector<VertexPosition> vertices;

    vertices.resize(vertexCount);

    for (NxU32 i = 0; i < vertexCount; ++i)
    {
        vertices[i].position = vertexBase[i];
    }

    const NxU32 triangleCount = convexMesh2->getCount(0, NX_ARRAY_TRIANGLES);
    const NxU32* triangleBase = static_cast<const NxU32*>(convexMesh2->getBase(0, NX_ARRAY_TRIANGLES));
    const unsigned int indexCount = triangleCount * 3;
    std::vector<unsigned short> indices;

    indices.resize(indexCount);

    for (NxU32 i = 0; i < indexCount; ++i)
    {
        indices[i] = triangleBase[i];
    }

    convexMesh->Initialize(vertices, indices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector4(1.f, 0.f, 0.f, 1.f), PrimitiveType::TriangleList);

    NxQuat rotation;

    convexShapeDescriptor->localPose.M.toQuat(rotation);

    convexMesh->GetTransform()->SetLocalPosition(convexShapeDescriptor->localPose.t);
    convexMesh->GetTransform()->SetLocalRotation(rotation);
}

void Collision::CreateTriangleMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> triangleMesh)
{
    NxTriangleMeshShapeDesc* triangleMeshShapeDescriptor = static_cast<NxTriangleMeshShapeDesc*>(shapeDescriptor);
    NxTriangleMesh* triangleMesh2 = triangleMeshShapeDescriptor->meshData;
    const NxU32 vertexCount = triangleMesh2->getCount(0, NX_ARRAY_VERTICES);
    const NxVec3* vertexBase = static_cast<const NxVec3*>(triangleMesh2->getBase(0, NX_ARRAY_VERTICES));
    std::vector<VertexPosition> vertices;

    vertices.resize(vertexCount);

    for (NxU32 i = 0; i < vertexCount; ++i)
    {
        vertices[i].position = vertexBase[i];
    }

    const NxU32 triangleCount = triangleMesh2->getCount(0, NX_ARRAY_TRIANGLES);
    const NxU32* triangleBase = static_cast<const NxU32*>(triangleMesh2->getBase(0, NX_ARRAY_TRIANGLES));
    const unsigned int indexCount = triangleCount * 3;
    std::vector<unsigned short> indices;

    indices.resize(indexCount);

    for (NxU32 i = 0; i < indexCount; ++i)
    {
        indices[i] = triangleBase[i];
    }

    triangleMesh->Initialize(vertices, indices, Renderer3D::Shaders::SimpleVertex, Renderer3D::Shaders::SimplePixel, Vector4(1.f, 0.f, 0.f, 1.f), PrimitiveType::TriangleList);

    NxQuat rotation;

    triangleMeshShapeDescriptor->localPose.M.toQuat(rotation);

    triangleMesh->GetTransform()->SetLocalPosition(triangleMeshShapeDescriptor->localPose.t);
    triangleMesh->GetTransform()->SetLocalRotation(rotation);
}
