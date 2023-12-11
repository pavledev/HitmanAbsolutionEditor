#pragma once

enum class ECollisionPack
{
	ECOLLISIONPACK_VOID = 0,
	ECOLLISIONPACK_CONVEXMESH = 1,
	ECOLLISIONPACK_TRIMESH = 2,
	ECOLLISIONPACK_PRIMITIVE = 4,
	ECOLLISIONPACK_BONEPRIMITIVE = 16,
	ECOLLISIONPACK_KINEMATIC_BONEPRIMITIVE = 64,
	ECOLLISIONPACK_ALL = 2147483646,
	ECOLLISIONPACK_AUTO = 2147483647
};

inline ECollisionPack operator&(ECollisionPack lhs, ECollisionPack rhs)
{
	return static_cast<ECollisionPack>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
