#include "rapidjson/stringbuffer.h"
#include "rapidjson/prettywriter.h"

#include "Resources/SoundBlendContainerExternalParametersBlueprint.h"
#include "Utility/StringUtility.h"

void SoundBlendContainerExternalParametersBlueprint::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		std::string line = binaryReader.ReadString('\n');

		if (line.starts_with("#"))
		{
			continue;
		}
		else if (line.starts_with("."))
		{
			nextID = std::atoi(line.substr(line.find(" ") + 1).c_str());
		}
		else
		{
			std::vector<std::string> parts = StringUtility::Split(line, '|');
			SoundBlendContainerExternalParameter soundBlendContainerExternalParameter;

			soundBlendContainerExternalParameter.name = parts[0];
			soundBlendContainerExternalParameter.id = std::atoi(parts[4].c_str());
			soundBlendContainerExternalParameter.properties.m_bIsDistanceControlled = StringUtility::ToLowerCase(soundBlendContainerExternalParameter.name) == "distance";
			soundBlendContainerExternalParameter.properties.m_fMin = std::stof(parts[1]);
			soundBlendContainerExternalParameter.properties.m_fMax = std::stof(parts[2]);
			soundBlendContainerExternalParameter.properties.m_fDefault = std::stof(parts[3]);
			soundBlendContainerExternalParameter.properties.m_fSustainPointValue = std::stof(parts[5]);
			soundBlendContainerExternalParameter.properties.m_fSustainPointVelocity = std::stof(parts[6]);

			soundBlendContainerExternalParameters.push_back(soundBlendContainerExternalParameter);
		}
	}
}

std::string SoundBlendContainerExternalParametersBlueprint::SerializeToJson()
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("NextID");
	writer.Uint(nextID);

	writer.String("Parameters");
	writer.StartArray();

	for (size_t i = 0; i < soundBlendContainerExternalParameters.size(); ++i)
	{
		soundBlendContainerExternalParameters[i].SerializeToJson(writer);
	}

	writer.EndArray();

	writer.EndObject();

	return stringBuffer.GetString();
}

unsigned int SoundBlendContainerExternalParametersBlueprint::GetNextID()
{
	return nextID;
}

std::vector<SoundBlendContainerExternalParametersBlueprint::SoundBlendContainerExternalParameter>& SoundBlendContainerExternalParametersBlueprint::GetSoundBlendContainerExternalParameters()
{
	return soundBlendContainerExternalParameters;
}

void SoundBlendContainerExternalParametersBlueprint::SoundBlendContainerExternalParameter::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("name");
	writer.String(name.c_str());

	writer.String("id");
	writer.Uint(id);

	writer.String("properties");
	writer.StartObject();

	writer.String("m_bIsDistanceControlled");
	writer.Bool(properties.m_bIsDistanceControlled);

	writer.String("m_fMin");
	writer.Double(properties.m_bIsDistanceControlled);

	writer.String("m_fMax");
	writer.Double(properties.m_bIsDistanceControlled);

	writer.String("m_fDefault");
	writer.Double(properties.m_bIsDistanceControlled);

	writer.String("m_fSustainPointValue");
	writer.Double(properties.m_bIsDistanceControlled);

	writer.String("m_fSustainPointVelocity");
	writer.Double(properties.m_bIsDistanceControlled);

	writer.EndObject();

	writer.EndObject();
}
