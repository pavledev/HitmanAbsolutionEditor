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
    const Physics* physics = renderPrimitive->GetPhysics();
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
            /*case NX_SHAPE_CONVEX:
            {
                NxConvexShapeDesc* convexShapeDescriptor = static_cast<NxConvexShapeDesc*>(shapeDescriptor);
                NxConvexMesh* convexShape = convexShapeDescriptor->meshData;
                NxInternalFormat format = convexShape->getFormat(0, NxInternalArray::NX_ARRAY_VERTICES);
                const void* vertexBuffer = convexShape->getBase(0, NxInternalArray::NX_ARRAY_VERTICES);
                const NxU32 stride = convexShape->getStride(0, NxInternalArray::NX_ARRAY_VERTICES);

                break;
            }
            case NX_SHAPE_MESH:
            {
                NxTriangleMeshShapeDesc* triangleMeshShapeDesc = static_cast<NxTriangleMeshShapeDesc*>(shapeDescriptor);
                NxTriangleMesh* convexShape = triangleMeshShapeDesc->meshData;
                NxInternalFormat format = convexShape->getFormat(0, NxInternalArray::NX_ARRAY_VERTICES);
                const void* vertexBuffer = convexShape->getBase(0, NxInternalArray::NX_ARRAY_VERTICES);
                const NxU32 stride = convexShape->getStride(0, NxInternalArray::NX_ARRAY_VERTICES);

                break;
            }*/
            default:
            {
                //Logger::GetInstance().Log(Logger::Level::Error, "Shape type isn't supported!");

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
    std::vector<VertexPositionColor> cubeVertices;
    std::vector<unsigned short> cubeIndices;
    Vector4 color = Vector4(255.f, 0.f, 0.f, 1.f);

    //Geometry::CreateCube(cubeVertices, cubeIndices, Vector4(255.f, 0.f, 0.f, 1.f), boxShapeDescriptor->localPose.t, boxShapeDescriptor->dimensions);
    Geometry::CreateCube(cubeVertices, cubeIndices, Vector4(255.f, 0.f, 0.f, 1.f));

    //boxMesh->Initialize(cubeVertices, cubeIndices, SceneRenderer::Shaders::PhongVertex, SceneRenderer::Shaders::PhongPixel, Vector4(255.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);

    /*Vector3 v1 = Vector3(-0.0308475010, -0.0361977108, 0.0404765755);
    Vector3 v2 = Vector3(-0.0308475010, -0.0361977108, 0.221976563);
    Vector3 v3 = Vector3(-0.0308475010, -0.0361977108, 0.0404765755);
    Vector3 v4 = Vector3(0.0391524918, -0.0361977108, 0.0404765755);
    Vector3 v5 = Vector3(-0.0308475010, -0.0361977108, 0.0404765755);
    Vector3 v6 = Vector3(-0.0308475010, 0.0338022821, 0.0404765755);
    Vector3 v7 = Vector3(-0.0308475010, 0.0338022821, 0.0404765755);
    Vector3 v8 = Vector3(-0.0308475010, 0.0338022821, 0.221976563);
    Vector3 v9 = Vector3(-0.0308475010, -0.0361977108, 0.221976563);
    Vector3 v10 = Vector3(0.0391524918, -0.0361977108, 0.221976563);
    Vector3 v11 = Vector3(0.0391524918, -0.0361977108, 0.0404765755);
    Vector3 v12 = Vector3(0.0391524918, 0.0338022821, 0.0404765755);
    Vector3 v13 = Vector3(0.0391524918, -0.0361977108, 0.0404765755);
    Vector3 v14 = Vector3(0.0391524918, -0.0361977108, 0.221976563);
    Vector3 v15 = Vector3(-0.0308475010, 0.0338022821, 0.0404765755);
    Vector3 v16 = Vector3(0.0391524918, 0.0338022821, 0.0404765755);
    Vector3 v17 = Vector3(-0.0308475010, -0.0361977108, 0.221976563);
    Vector3 v18 = Vector3(-0.0308475010, 0.0338022821, 0.221976563);
    Vector3 v19 = Vector3(0.0391524918, 0.0338022821, 0.0404765755);
    Vector3 v20 = Vector3(0.0391524918, 0.0338022821, 0.221976563);
    Vector3 v21 = Vector3(-0.0308475010, 0.0338022821, 0.221976563);
    Vector3 v22 = Vector3(0.0391524918, 0.0338022821, 0.221976563);
    Vector3 v23 = Vector3(0.0391524918, -0.0361977108, 0.221976563);
    Vector3 v24 = Vector3(0.0391524918, 0.0338022821, 0.221976563);

    cubeVertices.push_back({ v1, color });
    cubeVertices.push_back({ v2, color });
    cubeVertices.push_back({ v3, color });
    cubeVertices.push_back({ v4, color });
    cubeVertices.push_back({ v5, color });
    cubeVertices.push_back({ v6, color });
    cubeVertices.push_back({ v7, color });
    cubeVertices.push_back({ v8, color });
    cubeVertices.push_back({ v9, color });
    cubeVertices.push_back({ v10, color });
    cubeVertices.push_back({ v11, color });
    cubeVertices.push_back({ v12, color });
    cubeVertices.push_back({ v13, color });
    cubeVertices.push_back({ v14, color });
    cubeVertices.push_back({ v15, color });
    cubeVertices.push_back({ v16, color });
    cubeVertices.push_back({ v17, color });
    cubeVertices.push_back({ v18, color });
    cubeVertices.push_back({ v19, color });
    cubeVertices.push_back({ v20, color });
    cubeVertices.push_back({ v21, color });
    cubeVertices.push_back({ v22, color });
    cubeVertices.push_back({ v23, color });
    cubeVertices.push_back({ v24, color });

    boxMesh->Initialize(cubeVertices, SceneRenderer::Shaders::PhongVertex, SceneRenderer::Shaders::PhongPixel, Vector4(255.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);*/

    NxQuat rotation;

    boxShapeDescriptor->localPose.M.toQuat(rotation);

    Vector3 r = Quaternion(rotation).ToEulerAngles();
    Quaternion newQuaternion = Quaternion::FromEulerAngles(Vector3(8.07611, 88.2883, -86.4235));

    boxMesh->GetTransform()->SetLocalPosition(boxShapeDescriptor->localPose.t);
    boxMesh->GetTransform()->SetLocalRotation(rotation);
    boxMesh->GetTransform()->SetLocalScale(boxShapeDescriptor->dimensions);
}

void Collision::CreateSphereMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> sphereMesh)
{
    NxSphereShapeDesc* sphereShapeDescriptor = static_cast<NxSphereShapeDesc*>(shapeDescriptor);
}

void Collision::CreateCapsuleMesh(NxShapeDesc* shapeDescriptor, std::shared_ptr<Mesh> capsuleMesh)
{
    NxCapsuleShapeDesc* capsuleShapeDescriptor = static_cast<NxCapsuleShapeDesc*>(shapeDescriptor);
    std::vector<VertexPositionColor> vertices;
    std::vector<unsigned short> indices;
    const int capsuleSides = Math::Clamp<int>(capsuleShapeDescriptor->radius / 4.f, 16, 64);

    Geometry::CreateWireCapsule(vertices, capsuleShapeDescriptor->localPose.t, Vector3(1.f, 0.f, 0.f), Vector3(0.f, 1.f, 0.f), Vector3(0.f, 0.f, 1.f), Vector4(255.f, 0.f, 0.f, 1.f),
        capsuleShapeDescriptor->radius, capsuleShapeDescriptor->height / 2, capsuleSides);

    //capsuleMesh->Initialize(vertices, SceneRenderer::Shaders::PhongVertex, SceneRenderer::Shaders::PhongPixel, Vector4(255.f, 0.f, 0.f, 1.f), PrimitiveType::LineList);

    NxQuat rotation;

    capsuleShapeDescriptor->localPose.M.toQuat(rotation);

    Vector3 r = Quaternion(rotation).ToEulerAngles();

    capsuleMesh->GetTransform()->SetLocalPosition(capsuleShapeDescriptor->localPose.t);
    capsuleMesh->GetTransform()->SetLocalRotation(rotation);
}
