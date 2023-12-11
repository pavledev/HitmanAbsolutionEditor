#include "Glacier/RenderPrimitive/SPrimHeader.h"

std::string SPrimHeader::ConvertPrimTypeToString(const EPrimType primType)
{
	switch (primType)
	{
		case EPrimType::PTOBJECTHEADER:
			return "Object Header";
		case EPrimType::PTMESH:
			return "Mesh";
		case EPrimType::PTDECAL:
			return "Decal";
		case EPrimType::PTSPRITES:
			return "Sprites";
		case EPrimType::PTSHAPE:
			return "Shape";
		case EPrimType::PTUNUSED:
			return "Unused";
		default:
			return "";
	}
}
