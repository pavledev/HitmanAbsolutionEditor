#pragma once

#include <string>
#include <vector>

#include "Glacier/Sound/SSBCExternalParametersProperties.h"

#include "Resource.h"

class SoundBlendContainerExternalParametersType : public Resource
{
public:
	struct SoundBlendContainerExternalParameter
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		std::string name;
		unsigned int id;
		SSBCExternalParametersProperties properties;
	};

	void Deserialize();
	std::string SerializeToJson();
	unsigned int GetNextID();
	std::vector<SoundBlendContainerExternalParameter>& GetSoundBlendContainerExternalParameters();

private:
	unsigned int nextID;
	std::vector<SoundBlendContainerExternalParameter> soundBlendContainerExternalParameters;
};
