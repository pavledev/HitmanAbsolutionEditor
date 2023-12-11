#include "Glacier/BoneRig/SBoneConstraint.h"

std::string SBoneConstraint::ConvertTypeToString(const EBoneConstrainType boneConstrainType)
{
	switch (boneConstrainType)
	{
		case EBoneConstrainType::BC_LOOKAT:
			return "Look At";
		case EBoneConstrainType::BC_ROTATION:
			return "Rotation";
		default:
			return "";
	}
}
