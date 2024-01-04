#pragma once

#include <string>

#include "Glacier/Morpheme/Morpheme.h"

#include "Resources/Resource.h"

class Rig : public Resource
{
public:
	void Deserialize() override;
	void SerializeToJson(std::string& jsonOutput);

private:
	MR::Rig* rig;
};
