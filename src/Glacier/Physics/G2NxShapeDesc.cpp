#include "Glacier/Physics/G2NxShapeDesc.h"

G2NxShapeDesc::~G2NxShapeDesc()
{
	delete m_pDesc;
}

void G2NxShapeDesc::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("m_pDesc");
	SerializeNxShapeDescToJson(*m_pDesc, writer);

	writer.String("m_bIsOpaque");
	writer.Bool(m_bIsOpaque);

	writer.String("m_pStatic");
	writer.Null();

	writer.String("m_pDynamic");
	writer.Null();

	writer.EndObject();
}

void G2NxShapeDesc::SerializeNxVec3ToJson(const NxVec3& vector, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("x");
	writer.Double(vector.x);

	writer.String("y");
	writer.Double(vector.y);

	writer.String("z");
	writer.Double(vector.z);

	writer.EndObject();
}

void G2NxShapeDesc::SerializeNxMat33ToJson(const NxMat33& matrix, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	NxVec3 xAxis = matrix.getColumn(0);
	NxVec3 yAxis = matrix.getColumn(1);
	NxVec3 zAxis = matrix.getColumn(2);

	writer.StartObject();

	writer.String("xAxis");
	SerializeNxVec3ToJson(xAxis, writer);

	writer.String("yAxis");
	SerializeNxVec3ToJson(yAxis, writer);

	writer.String("zAxis");
	SerializeNxVec3ToJson(zAxis, writer);

	writer.EndObject();
}

void G2NxShapeDesc::SerializeNxMat34ToJson(const NxMat34& matrix, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("rotation");
	SerializeNxMat33ToJson(matrix.M, writer);

	writer.String("translation");
	SerializeNxVec3ToJson(matrix.t, writer);

	writer.EndObject();
}

void G2NxShapeDesc::SerializeNxGroupsMaskToJson(const NxGroupsMask& groupsMask, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("bits0");
	writer.Uint(groupsMask.bits0);

	writer.String("bits1");
	writer.Uint(groupsMask.bits1);

	writer.String("bits2");
	writer.Uint(groupsMask.bits2);

	writer.String("bits3");
	writer.Uint(groupsMask.bits3);

	writer.EndObject();
}

void G2NxShapeDesc::SerializeNxShapeDescToJson(const NxShapeDesc& shapeDesc, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("type");
	writer.String(ConvertShapeTypeToString(shapeDesc.getType()).c_str());

	writer.String("localPose");
	SerializeNxMat34ToJson(shapeDesc.localPose, writer);

	writer.String("shapeFlags");
	writer.String(ConvertShapeTypeToString(shapeDesc.getType()).c_str());

	writer.String("group");
	writer.Uint(shapeDesc.group);

	writer.String("materialIndex");
	writer.Uint(shapeDesc.materialIndex);

	writer.String("ccdSkeleton");
	writer.Null();

	writer.String("density");
	writer.Double(shapeDesc.density);

	writer.String("mass");
	writer.Double(shapeDesc.mass);

	writer.String("skinWidth");
	writer.Double(shapeDesc.skinWidth);

	writer.String("userData");
	writer.Null();

	writer.String("name");

	if (shapeDesc.name)
	{
		writer.String(shapeDesc.name);
	}
	else
	{
		writer.Null();
	}

	writer.String("groupsMask");
	SerializeNxGroupsMaskToJson(shapeDesc.groupsMask, writer);

	writer.String("nonInteractingCompartmentTypes");
	writer.Uint(shapeDesc.nonInteractingCompartmentTypes);

	writer.EndObject();
}

std::string G2NxShapeDesc::ConvertShapeTypeToString(const NxShapeType shapeType)
{
	switch (shapeType)
	{
		case NX_SHAPE_PLANE:
			return "Plane";
		case NX_SHAPE_SPHERE:
			return "Sphere";
		case NX_SHAPE_BOX:
			return "Box";
		case NX_SHAPE_CAPSULE:
			return "Capsule";
		case NX_SHAPE_WHEEL:
			return "Wheel";
		case NX_SHAPE_CONVEX:
			return "Convex";
		case NX_SHAPE_MESH:
			return "Mesh";
		case NX_SHAPE_HEIGHTFIELD:
			return "Height Field";
		case NX_SHAPE_RAW_MESH:
			return "Raw Mesh";
		case NX_SHAPE_COMPOUND:
			return "Compound";
		default:
			return "";
	}
}

std::string G2NxShapeDesc::ConvertShapeFlagsToString(const NxShapeFlag shapeType)
{
	std::string flags;

	if ((shapeType & NX_TRIGGER_ON_ENTER) == NX_TRIGGER_ON_ENTER)
	{
		flags += "Trigger On Enter, ";
	}

	if ((shapeType & NX_TRIGGER_ON_LEAVE) == NX_TRIGGER_ON_LEAVE)
	{
		flags += "Trigger On Leave, ";
	}

	if ((shapeType & NX_TRIGGER_ON_STAY) == NX_TRIGGER_ON_STAY)
	{
		flags += "Trigger On Stay, ";
	}

	if ((shapeType & NX_SF_VISUALIZATION) == NX_SF_VISUALIZATION)
	{
		flags += "Visualization, ";
	}

	if ((shapeType & NX_SF_DISABLE_COLLISION) == NX_SF_DISABLE_COLLISION)
	{
		flags += "Disable Collision, ";
	}

	if ((shapeType & NX_SF_FEATURE_INDICES) == NX_SF_FEATURE_INDICES)
	{
		flags += "Feature Indices, ";
	}

	if ((shapeType & NX_SF_DISABLE_RAYCASTING) == NX_SF_DISABLE_RAYCASTING)
	{
		flags += "Disable Raycasting, ";
	}

	if ((shapeType & NX_SF_POINT_CONTACT_FORCE) == NX_SF_POINT_CONTACT_FORCE)
	{
		flags += "Point Contact Force, ";
	}

	if ((shapeType & NX_SF_FLUID_DRAIN) == NX_SF_FLUID_DRAIN)
	{
		flags += "Fluid Drain, ";
	}

	if ((shapeType & NX_SF_FLUID_DISABLE_COLLISION) == NX_SF_FLUID_DISABLE_COLLISION)
	{
		flags += "Fluid Disable Collision, ";
	}

	if ((shapeType & NX_SF_FLUID_TWOWAY) == NX_SF_FLUID_TWOWAY)
	{
		flags += "Fluid Twoway, ";
	}

	if ((shapeType & NX_SF_DISABLE_RESPONSE) == NX_SF_DISABLE_RESPONSE)
	{
		flags += "Disable Response, ";
	}

	if ((shapeType & NX_SF_DYNAMIC_DYNAMIC_CCD) == NX_SF_DYNAMIC_DYNAMIC_CCD)
	{
		flags += "Dynamic CCD, ";
	}

	if ((shapeType & NX_SF_DISABLE_SCENE_QUERIES) == NX_SF_DISABLE_SCENE_QUERIES)
	{
		flags += "Disable Scene Queries, ";
	}

	if ((shapeType & NX_SF_CLOTH_DRAIN) == NX_SF_CLOTH_DRAIN)
	{
		flags += "Cloth Drain, ";
	}

	if ((shapeType & NX_SF_CLOTH_DISABLE_COLLISION) == NX_SF_CLOTH_DISABLE_COLLISION)
	{
		flags += "Cloth Disable Collision, ";
	}

	if ((shapeType & NX_SF_CLOTH_TWOWAY) == NX_SF_CLOTH_TWOWAY)
	{
		flags += "Cloth Twoway, ";
	}

	if ((shapeType & NX_SF_SOFTBODY_DRAIN) == NX_SF_SOFTBODY_DRAIN)
	{
		flags += "Soft Body Drain, ";
	}

	if ((shapeType & NX_SF_SOFTBODY_DISABLE_COLLISION) == NX_SF_SOFTBODY_DISABLE_COLLISION)
	{
		flags += "Soft Body Disable Collision, ";
	}

	if ((shapeType & NX_SF_SOFTBODY_TWOWAY) == NX_SF_SOFTBODY_TWOWAY)
	{
		flags += "Soft Body Twoway";
	}

	if (flags.ends_with(", "))
	{
		flags = flags.erase(flags.length() - 2, 2);
	}

	return flags;
}
