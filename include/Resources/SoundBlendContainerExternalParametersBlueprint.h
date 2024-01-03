#pragma once

#include <string>
#include <vector>

#include "Glacier/Sound/SSBCExternalParametersProperties.h"

#include "Resource.h"

class SoundBlendContainerExternalParametersBlueprint : public Resource
{
public:
	struct SoundBlendContainerExternalParameter
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		std::string name;
		unsigned int id;
		SSBCExternalParametersProperties properties;
	};

	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(const std::string& outputFilePath);
	unsigned int GetNextID();
	std::vector<SoundBlendContainerExternalParameter>& GetSoundBlendContainerExternalParameters();

private:
	unsigned int nextID;
	std::vector<SoundBlendContainerExternalParameter> soundBlendContainerExternalParameters;
};
