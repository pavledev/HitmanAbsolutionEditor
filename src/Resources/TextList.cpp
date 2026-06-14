#include <format>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Resources/TextList.h"
#include "Utility/StringUtility.h"
#include "Utility/ResourceUtility.h"
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

	if (headerLibraryFilePath.empty())
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Header library file path is empty");

		return false;
	}

	if (!std::filesystem::exists(headerLibraryFilePath))
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Header library file not found: {}", headerLibraryFilePath));

		return false;
	}

	std::string backupResLibPath = resourceLibraryFilePath + ".bak";
	std::string backupHdrLibPath = headerLibraryFilePath + ".bak";

	if (!std::filesystem::exists(backupResLibPath))
	{
		std::filesystem::copy_file(resourceLibraryFilePath, backupResLibPath, std::filesystem::copy_options::overwrite_existing);
		Logger::GetInstance().Log(Logger::Level::Info, std::format("Created resource library backup: {}", backupResLibPath));
	}

	if (!std::filesystem::exists(backupHdrLibPath))
	{
		std::filesystem::copy_file(headerLibraryFilePath, backupHdrLibPath, std::filesystem::copy_options::overwrite_existing);
		Logger::GetInstance().Log(Logger::Level::Info, std::format("Created header library backup: {}", backupHdrLibPath));
	}

	std::vector<unsigned char> newResourceData;

	if (!SerializeToBinary(newResourceData))
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to serialize text list to binary");

		return false;
	}

	BinaryReader headerLibReader = BinaryReader(headerLibraryFilePath);

	headerLibReader.Seek(4, SeekOrigin::Begin);

	unsigned int referencesChunkSize = headerLibReader.Read<unsigned int>();

	headerLibReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

	unsigned int chunksStartOffset = headerLibReader.GetPosition();
	chunksStartOffset += headerLibReader.Read<unsigned int>();

	headerLibReader.Seek(chunksStartOffset - 4, SeekOrigin::Begin);

	unsigned int chunkCount = headerLibReader.Read<unsigned int>();

	unsigned int targetChunkResourceHeadersStartOffset = 0;
	unsigned int targetChunkResourceHeaderCount = 0;

	for (unsigned int c = 0; c < chunkCount; ++c)
	{
		unsigned int chunkOffset = chunksStartOffset + 0x50 * c;

		headerLibReader.Seek(chunkOffset + 0x4, SeekOrigin::Begin);

		unsigned int headerLibraryChunkResourceIDOffset = headerLibReader.GetPosition();
		headerLibraryChunkResourceIDOffset += headerLibReader.Read<unsigned int>();

		headerLibReader.Seek(headerLibraryChunkResourceIDOffset - 4, SeekOrigin::Begin);

		unsigned int resourceIDLength = headerLibReader.Read<unsigned int>();
		std::string chunkResourceID = headerLibReader.ReadString(static_cast<size_t>(resourceIDLength - 1));

		std::string expectedChunkResourceID = ResourceUtility::ConvertResourceIDToFilePath(headerLibraryChunkResourceID);

		if (chunkResourceID != headerLibraryChunkResourceID)
		{
			continue;
		}

		headerLibReader.Seek(chunkOffset + 0x2C, SeekOrigin::Begin);

		unsigned int resourceHeadersStartOffset = headerLibReader.GetPosition();
		resourceHeadersStartOffset += headerLibReader.Read<unsigned int>();

		headerLibReader.Seek(resourceHeadersStartOffset - 4, SeekOrigin::Begin);

		unsigned int resourceHeaderCount = headerLibReader.Read<unsigned int>();

		targetChunkResourceHeadersStartOffset = resourceHeadersStartOffset;
		targetChunkResourceHeaderCount = resourceHeaderCount;

		break;
	}

	if (targetChunkResourceHeadersStartOffset == 0)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Could not find target chunk in header library");

		return false;
	}

	std::vector<std::pair<unsigned int, unsigned int>> resourceSizes;

	unsigned int resourceLibraryOffset = 0x18;

	headerLibReader.Seek(targetChunkResourceHeadersStartOffset, SeekOrigin::Begin);

	for (unsigned int i = 0; i < targetChunkResourceHeaderCount; ++i)
	{
		unsigned int currentPosition = headerLibReader.GetPosition();
		unsigned int relativeOffset = headerLibReader.Read<unsigned int>();
		unsigned int absoluteOffset = currentPosition + relativeOffset;

		headerLibReader.Seek(absoluteOffset, SeekOrigin::Begin);

		unsigned int type = headerLibReader.Read<unsigned int>();

		headerLibReader.Skip(8);

		unsigned int dataSize = headerLibReader.Read<unsigned int>();

		if (type == 'FSBM' || type == 'FSBS')
		{
			dataSize += 24;
		}

		resourceSizes.push_back({ absoluteOffset + 12, dataSize });
		resourceLibraryOffset += dataSize;

		headerLibReader.Seek(currentPosition + 0xC, SeekOrigin::Begin);
	}

	std::ifstream resLibFile(resourceLibraryFilePath, std::ios::binary | std::ios::ate);

	if (!resLibFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open resource library for reading: {}", resourceLibraryFilePath));

		return false;
	}

	std::streamsize totalSize = resLibFile.tellg();

	resLibFile.seekg(0, std::ios::beg);

	std::vector<unsigned char> resLibData(totalSize);
	resLibFile.read(reinterpret_cast<char*>(resLibData.data()), totalSize);
	resLibFile.close();

	unsigned int chunkStartOffset = 0x18;

	for (unsigned int i = 0; i < indexInLibrary; ++i)
	{
		chunkStartOffset += resourceSizes[i].second;
	}

	std::vector<unsigned char> newResLibData;
	newResLibData.insert(newResLibData.end(), resLibData.begin(), resLibData.begin() + chunkStartOffset);
	newResLibData.insert(newResLibData.end(), newResourceData.begin(), newResourceData.end());
	newResLibData.insert(newResLibData.end(), resLibData.begin() + chunkStartOffset + resourceDataSize, resLibData.end());

	unsigned int newTotalDataSize = static_cast<unsigned int>(newResLibData.size() - 0x18);
	newResLibData[0xC] = newTotalDataSize & 0xFF;
	newResLibData[0xD] = (newTotalDataSize >> 8) & 0xFF;
	newResLibData[0xE] = (newTotalDataSize >> 16) & 0xFF;
	newResLibData[0xF] = (newTotalDataSize >> 24) & 0xFF;

	std::ofstream resLibOutFile(resourceLibraryFilePath, std::ios::binary | std::ios::trunc);

	if (!resLibOutFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open resource library for writing: {}", resourceLibraryFilePath));

		return false;
	}

	resLibOutFile.write(reinterpret_cast<const char*>(newResLibData.data()), newResLibData.size());
	resLibOutFile.close();

	unsigned int dataSizeOffset = resourceSizes[indexInLibrary].first;

	std::fstream headerLibFile(headerLibraryFilePath, std::ios::binary | std::ios::in | std::ios::out);

	if (!headerLibFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open header library for writing: {}", headerLibraryFilePath));

		return false;
	}

	unsigned int newDataSetSize = static_cast<unsigned int>(newResourceData.size());
	headerLibFile.seekp(dataSizeOffset);
	headerLibFile.write(reinterpret_cast<const char*>(&newDataSetSize), sizeof(newDataSetSize));
	headerLibFile.close();

	delete[] resourceData;

	resourceDataSize = static_cast<unsigned int>(newResourceData.size());
	resourceData = new unsigned char[resourceDataSize];
	memcpy(resourceData, newResourceData.data(), resourceDataSize);

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Patched resource {} in {} (old size: {}, new size: {})", resourceID, resourceLibraryFilePath, resourceDataSize, newResourceData.size()));

	return true;
}
