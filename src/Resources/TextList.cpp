#include "Resources/TextList.h"
#include "Utility/StringUtility.h"
#include "Registry/TextListHashRegistry.h"

void TextList::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	entries.reserve(entryCount);

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		Entry entry;

		const unsigned int key = binaryReader.Read<unsigned int>();
		const unsigned int textLength = binaryReader.Read<unsigned int>();
		const std::string text = binaryReader.ReadString(static_cast<size_t>(textLength));
		const std::string name = TextListHashRegistry::GetInstance().GetName(key);

		entry.key = key;
		entry.text = text;

		if (name.empty())
		{
			entry.name = StringUtility::ConvertValueToHexString(key);
		}
		else
		{
			entry.name = name;
		}

		entries.push_back(entry);
	}

	isResourceDeserialized = true;
}

void TextList::Export(const std::string& outputPath, const std::string& exportOption)
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

std::vector<TextList::Entry>& TextList::GetEntries()
{
	return entries;
}

void TextList::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("entries");
	writer.StartArray();

	for (size_t i = 0; i < entries.size(); ++i)
	{
		writer.StartObject();

		writer.String("hash");
		writer.String(StringUtility::ConvertValueToHexString(entries[i].key, 8).c_str());

		writer.String("name");

		const std::string& name = TextListHashRegistry::GetInstance().GetName(entries[i].key);

		if (!name.empty())
		{
			writer.String(entries[i].name.c_str());
		}
		else
		{
			writer.Null();
		}

		writer.String("text");
		writer.String(entries[i].text.c_str());

		writer.EndObject();
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}
