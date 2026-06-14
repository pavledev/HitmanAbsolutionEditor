#include <format>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Resources/TextList.h"
#include "Utility/StringUtility.h"
#include "Registry/TextListHashRegistry.h"
#include "Logger.h"

void TextList::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
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

void TextList::ImportFromJson(const std::string& jsonFilePath)
{
	std::ifstream inputFileStream = std::ifstream(jsonFilePath);

	if (!inputFileStream.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open JSON file: {}", jsonFilePath));

		return;
	}

	std::string jsonString((std::istreambuf_iterator<char>(inputFileStream)), std::istreambuf_iterator<char>());

	inputFileStream.close();

	rapidjson::Document document;

	document.Parse(jsonString.c_str());

	if (document.HasParseError())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to parse JSON file: {}", jsonFilePath));

		return;
	}

	if (!document.HasMember("entries") || !document["entries"].IsArray())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "JSON file does not contain 'entries' array");

		return;
	}

	const rapidjson::Value& jsonEntries = document["entries"];
	unsigned int importCount = 0;

	for (rapidjson::SizeType i = 0; i < jsonEntries.Size(); ++i)
	{
		const rapidjson::Value& jsonEntry = jsonEntries[i];

		if (!jsonEntry.HasMember("hash") || !jsonEntry["hash"].IsString())
		{
			continue;
		}

		if (!jsonEntry.HasMember("text") || !jsonEntry["text"].IsString())
		{
			continue;
		}

		const std::string hashString = jsonEntry["hash"].GetString();
		const std::string text = jsonEntry["text"].GetString();

		unsigned int hashValue = 0;
		std::istringstream stringstream(hashString);

		stringstream >> std::hex >> hashValue;

		bool found = false;

		for (size_t j = 0; j < entries.size(); ++j)
		{
			if (entries[j].key == hashValue)
			{
				entries[j].text = text;
				found = true;
				importCount++;

				break;
			}
		}

		if (!found)
		{
			Logger::GetInstance().Log(Logger::Level::Warning, std::format("Hash {} not found in text list, skipping", hashString));
		}
	}

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Imported {} entries from {}", importCount, jsonFilePath));
}

bool TextList::SerializeToBinary(std::vector<unsigned char>& outputBuffer)
{
	std::vector<unsigned char> buffer;

	const unsigned int entryCount = static_cast<unsigned int>(entries.size());

	buffer.insert(buffer.end(), reinterpret_cast<const unsigned char*>(&entryCount), reinterpret_cast<const unsigned char*>(&entryCount) + sizeof(entryCount));

	for (size_t i = 0; i < entries.size(); ++i)
	{
		const unsigned int key = entries[i].key;
		const unsigned int textLength = static_cast<unsigned int>(entries[i].text.size());

		buffer.insert(buffer.end(), reinterpret_cast<const unsigned char*>(&key), reinterpret_cast<const unsigned char*>(&key) + sizeof(key));
		buffer.insert(buffer.end(), reinterpret_cast<const unsigned char*>(&textLength), reinterpret_cast<const unsigned char*>(&textLength) + sizeof(textLength));
		buffer.insert(buffer.end(), entries[i].text.begin(), entries[i].text.end());
		buffer.push_back('\0');
	}

	outputBuffer = buffer;

	return true;
}

bool TextList::PatchResourceLibrary()
{
	if (resourceLibraryFilePath.empty())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Resource library file path is empty");

		return false;
	}

	if (!std::filesystem::exists(resourceLibraryFilePath))
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Resource library file not found: {}", resourceLibraryFilePath));

		return false;
	}

	std::string backupPath = resourceLibraryFilePath + ".bak";

	if (!std::filesystem::exists(backupPath))
	{
		std::filesystem::copy_file(resourceLibraryFilePath, backupPath, std::filesystem::copy_options::overwrite_existing);
		Logger::GetInstance().Log(Logger::Level::Info, std::format("Created backup: {}", backupPath));
	}

	std::vector<unsigned char> newResourceData;

	if (!SerializeToBinary(newResourceData))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to serialize text list to binary");

		return false;
	}

	if (newResourceData.size() != resourceDataSize)
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("New data size ({}) does not match original size ({}). Cannot patch without corrupting the resource library. Please ensure text lengths match the original.", newResourceData.size(), resourceDataSize));

		return false;
	}

	std::fstream fileStream(resourceLibraryFilePath, std::ios::binary | std::ios::in | std::ios::out);

	if (!fileStream.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open resource library file for writing: {}", resourceLibraryFilePath));

		return false;
	}

	fileStream.seekp(offsetInResourceLibrary);
	fileStream.write(reinterpret_cast<const char*>(newResourceData.data()), newResourceData.size());

	if (fileStream.fail())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to write data to resource library at offset {}", offsetInResourceLibrary));

		fileStream.close();

		return false;
	}

	fileStream.close();

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Patched resource {} in {} at offset {}", resourceID, resourceLibraryFilePath, offsetInResourceLibrary));

	return true;
}
