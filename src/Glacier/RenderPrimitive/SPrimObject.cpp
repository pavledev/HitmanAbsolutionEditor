#include "Glacier/RenderPrimitive/SPrimObject.h"

std::string SPrimObject::ConvertSubTypeToString(const SUBTYPE subType)
{
	switch (subType)
	{
		case SUBTYPE::SUBTYPE_STANDARD:
			return "Standard";
		case SUBTYPE::SUBTYPE_LINKED:
			return "Linked";
		case SUBTYPE::SUBTYPE_WEIGHTED:
			return "Weighted";
		case SUBTYPE::SUBTYPE_STANDARDUV2:
			return "Standard UV2";
		case SUBTYPE::SUBTYPE_STANDARDUV3:
			return "Standard UV3";
		case SUBTYPE::SUBTYPE_STANDARDUV4:
			return "Standard UV4";
		default:
			return "";
	}
}
