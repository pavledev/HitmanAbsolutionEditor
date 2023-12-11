#include "Glacier/BoneRig/SBoneDefinition.h"

void SBoneDefinition::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("Center");
	Center.SerializeToJson(writer);

	writer.String("lPrevBoneNr");
	writer.Uint(lPrevBoneNr);

	writer.String("Size");
	Size.SerializeToJson(writer);

	writer.String("Name");
	writer.String(Name);

	writer.String("nBodyPart");
	writer.Uint(nBodyPart);

	writer.EndObject();
}
