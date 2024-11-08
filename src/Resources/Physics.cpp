#include <vector>
#include <unordered_map>

#include "Physics/NxConvexShapeDesc.h"
#include "Physics/NxTriangleMeshShapeDesc.h"
#include "Physics/NxTriangleMesh.h"
#include "PhysXLoader/PhysXLoader.h"

#include "rapidjson/prettywriter.h"

#include "Glacier/Physics/EObjectEntity.h"
#include "Glacier/Physics/ECollisionPack.h"
#include "Glacier/Physics/G2NxShapeDesc.h"

#include "Resources/Physics.h"
#include "IO/PhysXStream.h"
#include "Registry/GlobalBoneRegistry.h"
#include "Logger.h"

Physics::Physics()
{
	physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION);
}

Physics::~Physics()
{
	NxReleasePhysicsSDK(physicsSDK);
}

const ZCollisionShape& Physics::GetCollisionShape() const
{
	return collisionShape;
}

void Physics::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	if (!DeserializeHeader(binaryReader))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize header!");

		return;
	}

	const ECollisionPack collisionPack = static_cast<ECollisionPack>(headerData.m_nCollFlags);

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_CONVEXMESH) == ECollisionPack::ECOLLISIONPACK_CONVEXMESH ||
		(collisionPack & ECollisionPack::ECOLLISIONPACK_TRIMESH) == ECollisionPack::ECOLLISIONPACK_TRIMESH ||
		(collisionPack & ECollisionPack::ECOLLISIONPACK_PRIMITIVE) == ECollisionPack::ECOLLISIONPACK_PRIMITIVE)
	{
		if (!DeserializeCollisionShape(binaryReader))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize collision shape!");

			return;
		}
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_BONEPRIMITIVE) == ECollisionPack::ECOLLISIONPACK_BONEPRIMITIVE)
	{
		if (!DeserializeShatterData(binaryReader))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize shatter data!");

			return;
		}
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE) == ECollisionPack::ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE)
	{
		if (!DeserializeKinematicLinkedData(binaryReader))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize kinematic linked data!");

			return;
		}
	}

	isResourceDeserialized = true;
}

bool Physics::DeserializeHeader(BinaryReader& binaryReader)
{
	headerData.m_nCollFlags = binaryReader.Read<unsigned int>();
	headerData.m_eObjType = binaryReader.Read<unsigned int>();

	const EObjectEntity objectEntity = static_cast<EObjectEntity>(headerData.m_eObjType);

	if (objectEntity == EObjectEntity::EOBJECTENTITY_BACKWARDSCOMPATIBLE)
	{
		headerData.m_eObjType = 1;

		if ((headerData.m_nCollFlags & 5) != 0)
		{
			headerData.m_eObjType |= 2;
		}
	}

	const std::string idString = binaryReader.ReadString(2, false);
	id = binaryReader.Read<unsigned int>();
	const std::string physXString = binaryReader.ReadString(5, false);

	if (idString != "ID" || physXString != "PhysX")
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Collision data is NOT packed using PhysX - Repack using PhysX is required!");

		return false;
	}

	return true;
}

bool Physics::DeserializeCollisionShape(BinaryReader& binaryReader)
{
	const ECollisionPack collisionPack = static_cast<ECollisionPack>(headerData.m_nCollFlags);

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_CONVEXMESH) == ECollisionPack::ECOLLISIONPACK_CONVEXMESH)
	{
		if (!DeserializeConvexMesh(binaryReader))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize convex mesh!");

			return false;
		}
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_TRIMESH) == ECollisionPack::ECOLLISIONPACK_TRIMESH)
	{
		if (!DeserializeTriangleMesh(binaryReader))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize triangle mesh!");

			return false;
		}
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_PRIMITIVE) == ECollisionPack::ECOLLISIONPACK_PRIMITIVE)
	{
		std::vector<G2NxShapeDesc*>& shapeDescriptors = collisionShape.GetShapeDescriptors();

		if (!DeserializeImplicitPrimitives(shapeDescriptors, binaryReader))
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to deserialize implicit primitives!");

			return false;
		}
	}

	return true;
}

bool Physics::DeserializeConvexMesh(BinaryReader& binaryReader)
{
	const std::string tag = binaryReader.ReadString(3, true);

	if (tag != "CVX")
	{
		Logger::GetInstance().Log(Logger::Level::Error, "CVX tag expected!");

		return false;
	}

	G2NxShapeDesc* g2ShapeDescriptor = new G2NxShapeDesc();
	const unsigned int shapeCount = binaryReader.Read<unsigned int>();

	for (unsigned int i = 0; i < shapeCount; ++i)
	{
		NxConvexShapeDesc* convexShapeDescriptor = new NxConvexShapeDesc();
		PhysXStream physXStream = PhysXStream(&binaryReader);

		convexShapeDescriptor->group = binaryReader.Read<unsigned int>();
		convexShapeDescriptor->skinWidth = skinWidth;

		NxConvexMesh* convexMesh = physicsSDK->createConvexMesh(physXStream);

		convexShapeDescriptor->meshData = convexMesh;

		g2ShapeDescriptor->m_pDesc = convexShapeDescriptor;
		g2ShapeDescriptor->m_bIsOpaque = true;

		collisionShape.AddShapeDescriptor(g2ShapeDescriptor);
	}

	return true;
}

bool Physics::DeserializeTriangleMesh(BinaryReader& binaryReader)
{
	const std::string tag = binaryReader.ReadString(3, true);

	if (tag != "TRI")
	{
		Logger::GetInstance().Log(Logger::Level::Error, "TRI tag expected!");

		return false;
	}

	G2NxShapeDesc* g2ShapeDescriptor = new G2NxShapeDesc();
	const unsigned int shapeCount = binaryReader.Read<unsigned int>();

	for (unsigned int i = 0; i < shapeCount; ++i)
	{
		NxTriangleMeshShapeDesc* triangleMeshShapeDescriptor = new NxTriangleMeshShapeDesc();
		PhysXStream physXStream = PhysXStream(&binaryReader);

		triangleMeshShapeDescriptor->group = binaryReader.Read<unsigned int>();

		NxTriangleMesh* triangleMesh = physicsSDK->createTriangleMesh(physXStream);

		triangleMeshShapeDescriptor->meshData = triangleMesh;

		g2ShapeDescriptor->m_pDesc = triangleMeshShapeDescriptor;
		g2ShapeDescriptor->m_bIsOpaque = true;
		g2ShapeDescriptor->m_pStatic = nullptr;
		g2ShapeDescriptor->m_pDynamic = nullptr;

		collisionShape.AddShapeDescriptor(g2ShapeDescriptor);
	}

	return true;
}

bool Physics::DeserializeImplicitPrimitives(std::vector<G2NxShapeDesc*>& shapeDescriptors, BinaryReader& binaryReader)
{
	const std::string tag = binaryReader.ReadString(3, true);

	if (tag != "ICP")
	{
		Logger::GetInstance().Log(Logger::Level::Error, "ICP tag expected!");

		return false;
	}

	const unsigned int shapeCount = binaryReader.Read<unsigned int>();

	shapeDescriptors.resize(shapeCount);

	for (unsigned int i = 0; i < shapeCount; ++i)
	{
		const std::string shapeType = binaryReader.ReadString(3, true);
		NxShapeDesc* shapeDescriptor;
		G2NxShapeDesc* g2ShapeDescriptor = new G2NxShapeDesc();

		if (shapeType == "SPH")
		{
			NxSphereShapeDesc* sphereShapeDescriptor = new NxSphereShapeDesc();

			DeserializeSphereShape(sphereShapeDescriptor, binaryReader);

			sphereShapeDescriptor->radius += 0.0099999998f;
			shapeDescriptor = sphereShapeDescriptor;
		}
		else if (shapeType == "CAP")
		{
			NxCapsuleShapeDesc* capsuleShapeDescriptor = new NxCapsuleShapeDesc();

			DeserializeCapsuleShape(capsuleShapeDescriptor, binaryReader);

			capsuleShapeDescriptor->radius += 0.0099999998f;
			shapeDescriptor = capsuleShapeDescriptor;
		}
		else if (shapeType == "BOX")
		{
			NxBoxShapeDesc* boxShapeDescriptor = new NxBoxShapeDesc();

			DeserializeBoxShape(boxShapeDescriptor, binaryReader);

			boxShapeDescriptor->dimensions += NxVec3(skinWidth, skinWidth, skinWidth);
			shapeDescriptor = boxShapeDescriptor;
		}
		else
		{
			break;
		}

		shapeDescriptor->skinWidth = skinWidth;
		g2ShapeDescriptor->m_pDesc = shapeDescriptor;
		g2ShapeDescriptor->m_bIsOpaque = IsCollisionLayerOpaque(static_cast<ECollidableLayer>(shapeDescriptor->group));

		shapeDescriptors[i] = g2ShapeDescriptor;
	}

	return true;
}

void Physics::DeserializeSphereShape(NxSphereShapeDesc* sphereShapeDescriptor, BinaryReader& binaryReader)
{
	sphereShapeDescriptor->radius = binaryReader.Read<float>();
	sphereShapeDescriptor->group = binaryReader.Read<unsigned int>();
	sphereShapeDescriptor->localPose = DeserializeNxMat34(binaryReader);
}

void Physics::DeserializeCapsuleShape(NxCapsuleShapeDesc* capsuleShapeDescriptor, BinaryReader& binaryReader)
{
	capsuleShapeDescriptor->radius = binaryReader.Read<float>();
	capsuleShapeDescriptor->height = binaryReader.Read<float>();
	capsuleShapeDescriptor->group = binaryReader.Read<unsigned int>();
	capsuleShapeDescriptor->localPose = DeserializeNxMat34(binaryReader);
}

void Physics::DeserializeBoxShape(NxBoxShapeDesc* boxShapeDescriptor, BinaryReader& binaryReader)
{
	boxShapeDescriptor->dimensions = binaryReader.Read<NxVec3>();
	boxShapeDescriptor->group = binaryReader.Read<unsigned int>();
	boxShapeDescriptor->localPose = DeserializeNxMat34(binaryReader);
}

bool Physics::DeserializeShatterData(BinaryReader& binaryReader)
{
	const std::string tag = binaryReader.ReadString(3, true);

	if (tag != "BCP")
	{
		Logger::GetInstance().Log(Logger::Level::Error, "BCP tag expected!");

		return false;
	}

	std::unordered_map<unsigned int, int> boneMapper;
	const unsigned int collisionPrimitiveCount = binaryReader.Read<unsigned int>();

	if (collisionPrimitiveCount == 0)
	{
		return false;
	}

	shatterData.m_sShatterConfig.m_aBoneShards.resize(collisionPrimitiveCount);

	for (unsigned int i = 0; i < collisionPrimitiveCount; ++i)
	{
		const unsigned int boneID = binaryReader.Read<unsigned int>();
		const unsigned int parentID = binaryReader.Read<unsigned int>();
		const std::string boneName = binaryReader.ReadString();
		unsigned int globalBoneID = -1;

		if (GlobalBoneRegistry::GetInstance().GetBoneID(boneName) == -1)
		{
			globalBoneID = GlobalBoneRegistry::GetInstance().RegisterBoneID(boneName);
		}

		boneMapper[boneID] = globalBoneID;

		const unsigned int remainFlag = binaryReader.Read<unsigned int>();
		const bool remain = remainFlag || i == 0;
		const unsigned int adjacentBoneCount = binaryReader.Read<unsigned int>();
		std::vector<unsigned int> adjacentBones;

		adjacentBones.resize(adjacentBoneCount);

		for (unsigned int j = 0; j < adjacentBoneCount; ++j)
		{
			unsigned int id = binaryReader.Read<unsigned int>();

			adjacentBones[j] = id;
		}

		const unsigned int collisionDataSize = binaryReader.Read<unsigned int>();
		ZPhysicsResourceData::SShatterData::SShardData& shardData = shatterData.m_shardMap[globalBoneID];
		SShatterBoneShard& shatterBoneShard = shatterData.m_sShatterConfig.m_aBoneShards[i];

		shardData.m_nShardIndex = i;

		shatterBoneShard.m_nGlobalBoneID = globalBoneID;
		shatterBoneShard.m_nShardIndex = i;
		shatterBoneShard.m_bNoCollision = collisionDataSize == 0;
		shatterBoneShard.m_bRemain = remain;

		if (boneMapper.contains(parentID))
		{
			shatterBoneShard.m_nParentIndex = shatterData.m_shardMap[boneMapper[parentID]].m_nShardIndex;
		}
		else
		{
			shatterBoneShard.m_nParentIndex = 0;
		}

		shatterBoneShard.m_aConnections = adjacentBones;

		if (i > shatterBoneShard.m_nParentIndex)
		{
			shatterData.m_sShatterConfig.m_aBoneShards[shatterBoneShard.m_nParentIndex].m_aChildIndices.push_back(i);
		}

		if (collisionDataSize > 0)
		{
			std::vector<G2NxShapeDesc*>& shapeDescriptors = shatterData.m_shardMap[globalBoneID].m_shapes;

			if (!DeserializeImplicitPrimitives(shapeDescriptors, binaryReader))
			{
				return false;
			}

			for (size_t j = 0; j < shapeDescriptors.size(); ++j)
			{
				shatterBoneShard.m_pShape.AddShapeDescriptor(shapeDescriptors[j]);
			}
		}
	}

	for (unsigned int i = 0; i < collisionPrimitiveCount; ++i)
	{
		std::vector<unsigned int>& connections = shatterData.m_sShatterConfig.m_aBoneShards[i].m_aConnections;

		for (size_t j = 0; j < connections.size(); ++j)
		{
			const unsigned int globalBoneID = boneMapper[connections[j]];

			connections[j] = shatterData.m_shardMap[globalBoneID].m_nShardIndex;
		}
	}

	return true;
}

bool Physics::DeserializeKinematicLinkedData(BinaryReader& binaryReader)
{
	const ECollisionPack collisionPack = static_cast<ECollisionPack>(headerData.m_nCollFlags);

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE) != ECollisionPack::ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE)
	{
		return false;
	}

	const std::string tag = binaryReader.ReadString(3, true);

	if (tag != "KBP")
	{
		Logger::GetInstance().Log(Logger::Level::Error, "KBP tag expected!");

		return false;
	}

	std::unordered_map<unsigned int, int> boneMapper;
	const unsigned int boneCount = binaryReader.Read<unsigned int>();

	if (boneCount == 0)
	{
		return false;
	}

	linkedPrimData.m_aBonePrimitives.resize(boneCount);

	for (unsigned int i = 0; i < boneCount; ++i)
	{
		SKinematicBoneData& kinematicBoneData = linkedPrimData.m_aBonePrimitives[i];
		const unsigned int localBoneID = binaryReader.Read<unsigned int>();
		const unsigned int localParentID = binaryReader.Read<unsigned int>();
		const std::string boneName = binaryReader.ReadString();
		const unsigned int collisionDataSize = binaryReader.Read<unsigned int>();

		if (collisionDataSize > 0)
		{
			std::vector<G2NxShapeDesc*>& shapeDescriptors = kinematicBoneData.m_pShape.GetShapeDescriptors();

			if (!DeserializeImplicitPrimitives(shapeDescriptors, binaryReader))
			{
				return false;
			}
		}

		int globalBoneID = -1;

		if (GlobalBoneRegistry::GetInstance().GetBoneID(boneName) == -1)
		{
			globalBoneID = GlobalBoneRegistry::GetInstance().RegisterBoneID(boneName);
		}

		boneMapper[localBoneID] = globalBoneID;

		kinematicBoneData.m_nGlobalBoneId = globalBoneID;
		kinematicBoneData.m_nParentBoneId = localParentID;
	}

	return true;
}

NxMat33 Physics::DeserializeNxMat33(BinaryReader& binaryReader)
{
	const float m11 = binaryReader.Read<float>();
	const float m21 = binaryReader.Read<float>();
	const float m31 = binaryReader.Read<float>();

	const float m12 = binaryReader.Read<float>();
	const float m22 = binaryReader.Read<float>();
	const float m32 = binaryReader.Read<float>();

	const float m13 = binaryReader.Read<float>();
	const float m23 = binaryReader.Read<float>();
	const float m33 = binaryReader.Read<float>();

	const NxVec3 row0 = NxVec3(m11, m12, m13);
	const NxVec3 row1 = NxVec3(m21, m22, m23);
	const NxVec3 row2 = NxVec3(m31, m32, m33);

	return NxMat33(row0, row1, row2);
}

NxMat34 Physics::DeserializeNxMat34(BinaryReader& binaryReader)
{
	NxMat34 matrix;

	matrix.t = binaryReader.Read<NxVec3>();
	matrix.M = DeserializeNxMat33(binaryReader);

	return matrix;
}

bool Physics::IsCollisionLayerOpaque(const ECollidableLayer collidableLayer)
{
	switch (collidableLayer)
	{
		case ECollidableLayer::eCollLayer_STATIC_COLLIDABLES_ONLY_TRANSPARENT:
			return collidableLayer == ECollidableLayer::eCollLayer_STATIC_COLLIDABLES_ONLY;
		case ECollidableLayer::eCollLayer_DYNAMIC_COLLIDABLES_ONLY_TRANSPARENT:
			return collidableLayer == ECollidableLayer::eCollLayer_DYNAMIC_COLLIDABLES_ONLY;
		case ECollidableLayer::eCollLayer_KINEMATIC_COLLIDABLES_ONLY_TRANSPARENT:
			return collidableLayer == ECollidableLayer::eCollLayer_KINEMATIC_COLLIDABLES_ONLY;
		case ECollidableLayer::eCollLayer_DYNAMIC_COLLIDABLES_ONLY_NO_CHARACTER_TRANSPARENT:
			return collidableLayer == ECollidableLayer::eCollLayer_DYNAMIC_COLLIDABLES_ONLY_NO_CHARACTER;
		default:
			return true;
	}
}

void Physics::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		SerializeToJson(outputPath);
	}
}

void Physics::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("id");
	writer.Uint(id);

	writer.String("headerData");
	headerData.SerializeToJson(writer);

	writer.String("collisionShape");
	collisionShape.SerializeToJson(writer);

	writer.String("shatterData");
	shatterData.SerializeToJson(writer);

	writer.String("linkedPrimData");
	linkedPrimData.SerializeToJson(writer);

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();
}
