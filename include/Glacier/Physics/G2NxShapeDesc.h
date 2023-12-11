#pragma once

#include <string>

#include "Foundation/NxSimpleTypes.h"
#include "Physics/NxBoxShapeDesc.h"
#include "Foundation/NxMat33.h"
#include "Foundation/NxMat34.h"

#include "rapidjson/prettywriter.h"

struct G2NxShapeDesc
{
	G2NxShapeDesc() = default;
	~G2NxShapeDesc();
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static void SerializeNxVec3ToJson(const NxVec3& vector, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static void SerializeNxMat33ToJson(const NxMat33& matrix, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static void SerializeNxMat34ToJson(const NxMat34& matrix, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static void SerializeNxGroupsMaskToJson(const NxGroupsMask& groupsMask, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static void SerializeNxShapeDescToJson(const NxShapeDesc& shapeDesc, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
	static std::string ConvertShapeTypeToString(const NxShapeType shapeType);
	static std::string ConvertShapeFlagsToString(const NxShapeFlag shapeType);

	NxShapeDesc* m_pDesc;
	bool m_bIsOpaque;
	NxCCDSkeleton* m_pStatic;
	NxCCDSkeleton* m_pDynamic;
};
