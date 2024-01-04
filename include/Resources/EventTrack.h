#pragma once

#include <string>

#include "Glacier/Morpheme/Morpheme.h"

#include "Resources/Resource.h"

class EventTrack : public Resource
{
public:
	void Deserialize() override;
	void SerializeToJson(std::string& jsonOutput);

private:
	MR::EventSequenceHeader* eventSequenceHeader;
};
