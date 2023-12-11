#pragma once

#include <string>

#include "Glacier/Morpheme/Morpheme.h"

class MorphemeRuntimeEventTrack
{
public:
	void Deserialize(const std::string& filePath);
	void Deserialize(void* buffer);
	void SerializeToJson(std::string& jsonOutput);

private:
	MR::EventSequenceHeader* eventSequenceHeader;
};
