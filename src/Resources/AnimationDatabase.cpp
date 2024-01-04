#include "Resources/AnimationDatabase.h"

void AnimationDatabase::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	entries.reserve(entryCount);

	const std::vector<std::shared_ptr<Resource>>& references = GetReferences();

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		Entry entry;

		const unsigned int referenceIndex = binaryReader.Read<unsigned int>();
		entry.resourceID = references[referenceIndex]->GetResourceID();

		entries.push_back(entry);
	}

	const unsigned int animationNamesOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(animationNamesOffset - 4, SeekOrigin::Current);

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		entries[i].animationName = binaryReader.ReadString();
	}

	isResourceDeserialized = true;
}

void AnimationDatabase::Export(const std::string& outputPath, const std::string& exportOption)
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

std::vector<AnimationDatabase::Entry>& AnimationDatabase::GetEntries()
{
	return entries;
}

void AnimationDatabase::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("entries");
	writer.StartArray();

	for (size_t i = 0; i < entries.size(); ++i)
	{
		writer.StartObject();

		writer.String("animationName");
		writer.String(entries[i].animationName.c_str());

		writer.String("resourceID");
		writer.String(entries[i].resourceID.c_str());

		writer.EndObject();
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}
