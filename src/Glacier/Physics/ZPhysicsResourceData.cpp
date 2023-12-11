#include "Glacier/Physics/ZPhysicsResourceData.h"

void ZPhysicsResourceData::SHeaderData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_eObjType");
	writer.String(ConvertObjectTypeToString(static_cast<EObjectEntity>(m_eObjType)).c_str());

	writer.String("m_nCollFlags");
	writer.String(ConvertCollisionFlagsToString(static_cast<ECollisionPack>(m_nCollFlags)).c_str());

	writer.EndObject();
}

std::string ZPhysicsResourceData::SHeaderData::ConvertObjectTypeToString(const EObjectEntity objectEntity)
{
	switch (objectEntity)
	{
		case EObjectEntity::EOBJECTENTITY_VOID:
			return "Void";
		case EObjectEntity::EOBJECTENTITY_STATICCOLLISION:
			return "Static Collision";
		case EObjectEntity::EOBJECTENTITY_RIGIDBODY:
			return "Rigid Body";
		case EObjectEntity::EOBJECTENTITY_PRESSURESOFTBODY:
			return "Pressure Soft Body";
		case EObjectEntity::EOBJECTENTITY_SHATTER:
			return "Shatter";
		case EObjectEntity::EOBJECTENTITY_KINEMATIC_LINKED:
			return "Kinematic Linked";
		case EObjectEntity::EOBJECTENTITY_BACKWARDSCOMPATIBLE:
			return "Backward Compatible";
		default:
			return "";
	}
}

std::string ZPhysicsResourceData::SHeaderData::ConvertCollisionFlagsToString(const ECollisionPack collisionPack)
{
	std::string flags;

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_CONVEXMESH) == ECollisionPack::ECOLLISIONPACK_CONVEXMESH)
	{
		flags += "Convex Mesh, ";
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_TRIMESH) == ECollisionPack::ECOLLISIONPACK_TRIMESH)
	{
		flags += "Triangle Mesh, ";
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_PRIMITIVE) == ECollisionPack::ECOLLISIONPACK_PRIMITIVE)
	{
		flags += "Primitive, ";
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_BONEPRIMITIVE) == ECollisionPack::ECOLLISIONPACK_BONEPRIMITIVE)
	{
		flags += "Bone Primitive, ";
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE) == ECollisionPack::ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE)
	{
		flags += "Kinematic Bone Primitive, ";
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_ALL) == ECollisionPack::ECOLLISIONPACK_ALL)
	{
		flags += "All, ";
	}

	if ((collisionPack & ECollisionPack::ECOLLISIONPACK_AUTO) == ECollisionPack::ECOLLISIONPACK_AUTO)
	{
		flags += "Auto";
	}

	if (flags.empty())
	{
		flags = "Void";
	}
	else if (flags.ends_with(", "))
	{
		flags = flags.erase(flags.length() - 2, 2);
	}

	return flags;
}

void ZPhysicsResourceData::SShatterData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_sShatterConfig");
	writer.StartArray();

	for (auto it = m_shardMap.begin(); it != m_shardMap.end(); ++it)
	{
		writer.StartObject();

		writer.String("globalBoneID");
		writer.Uint(it->first);

		writer.String("shardData");
		it->second.SerializeToJson(writer);

		writer.EndObject();
	}

	writer.EndArray();

	writer.EndObject();
}

void ZPhysicsResourceData::SShatterData::SShardData::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_nShardIndex");
	writer.Uint(m_nShardIndex);

	writer.String("m_shapes");
	writer.StartArray();

	for (size_t i = 0; i < m_shapes.size(); ++i)
	{
		m_shapes[i]->SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();
}
