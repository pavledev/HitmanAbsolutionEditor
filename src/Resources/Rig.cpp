#include <unordered_map>

#include "Resources/Rig.h"

void Rig::Deserialize()
{
	rig = static_cast<MR::Rig*>(resourceData);

	rig->Locate();
}

{
}

void Rig::SerializeToJson(std::string& jsonOutput)
{
	jsonOutput = rig->SerializeToJson();
}
