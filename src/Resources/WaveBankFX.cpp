#include "Resources/WaveBankFX.h"
#include "Registry/TextListHashRegistry.h"
#include "Utility/StringUtility.h"

void WaveBankFX::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	faceFXAnimationReferences.reserve(entryCount);
	subtitleReferences.reserve(entryCount);

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		FaceFXAnimationReference faceFXAnimationReference;

		faceFXAnimationReference.animationSetResourceIndex = binaryReader.Read<unsigned int>();

		if (faceFXAnimationReference.animationSetResourceIndex != -1)
		{
			const unsigned int animationNameLength = binaryReader.Read<unsigned int>();
			faceFXAnimationReference.animationName = binaryReader.ReadString(static_cast<size_t>(animationNameLength) - 1);
		}

		faceFXAnimationReferences.push_back(faceFXAnimationReference);
	}

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		SubtitleReference subtitleReference;

		subtitleReference.textlistResourceIndex = binaryReader.Read<unsigned int>();

		if (subtitleReference.textlistResourceIndex != -1)
		{
			subtitleReference.hashedLoganID = binaryReader.Read<unsigned int>();

			const std::string name = TextListHashRegistry::GetInstance().GetName(subtitleReference.hashedLoganID);

			if (name.empty())
			{
				subtitleReference.textListEntryName = StringUtility::ConvertValueToHexString(subtitleReference.hashedLoganID);
			}
			else
			{
				subtitleReference.textListEntryName = name;
			}
		}

		subtitleReferences.push_back(subtitleReference);
	}

	isResourceDeserialized = true;
}

void WaveBankFX::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		SerializeToJson(outputPath);
	}
}

std::vector<WaveBankFX::FaceFXAnimationReference>& WaveBankFX::GetFaceFXAnimationReferences()
{
	return faceFXAnimationReferences;
}

std::vector<WaveBankFX::SubtitleReference>& WaveBankFX::GetSubtitleReferences()
{
	return subtitleReferences;
}

void WaveBankFX::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("faceFXAnimationReferences");
	writer.StartArray();

	for (size_t i = 0; i < faceFXAnimationReferences.size(); ++i)
	{
		writer.StartObject();

		const unsigned int animationSetResourceIndex = faceFXAnimationReferences[i].animationSetResourceIndex;

		writer.String("animationSetResourceID");

		if (animationSetResourceIndex != -1)
		{
			std::vector<std::shared_ptr<Resource>>& wbfxReferences = GetReferences();

			writer.String(wbfxReferences[animationSetResourceIndex]->GetResourceID().c_str());
		}
		else
		{
			writer.Null();
		}

		writer.String("animationName");
		writer.String(faceFXAnimationReferences[i].animationName.c_str());

		writer.EndObject();
	}

	writer.EndArray();

	writer.String("subtitleReferences");
	writer.StartArray();

	for (size_t i = 0; i < subtitleReferences.size(); ++i)
	{
		writer.StartObject();

		const unsigned int textlistResourceIndex = subtitleReferences[i].textlistResourceIndex;

		writer.String("textlistResourceResourceID");

		if (textlistResourceIndex != -1)
		{
			std::vector<std::shared_ptr<Resource>>& wbfxReferences = GetReferences();

			writer.String(wbfxReferences[textlistResourceIndex]->GetResourceID().c_str());
		}
		else
		{
			writer.Null();
		}

		writer.String("hashedLoganID");
		writer.String(StringUtility::ConvertValueToHexString(subtitleReferences[i].hashedLoganID, 8).c_str());

		writer.String("textListEntryName");
		writer.String(subtitleReferences[i].textListEntryName.c_str());

		writer.EndObject();
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}
