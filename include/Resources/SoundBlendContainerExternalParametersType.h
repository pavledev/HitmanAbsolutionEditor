#pragma once

#include <string>
#include <vector>

#include "Glacier/Sound/SSBCExternalParametersProperties.h"

#include "IO/BinaryReader.h"

class SoundBlendContainerExternalParametersType
{
public:
	void Deserialize(const std::string& filePath);
	void Deserialize(void* buffer, const unsigned int dataSize);
	void Deserialize(BinaryReader& binaryReader);
	std::string SerializeToJson();

private:
	struct SoundBlendContainerExternalParameter
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		std::string name;
		unsigned int id;
		SSBCExternalParametersProperties properties;
	};

	unsigned int nextID;
	std::vector<SoundBlendContainerExternalParameter> soundBlendContainerExternalParameters;
};
