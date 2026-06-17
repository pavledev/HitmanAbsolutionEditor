#include <format>
#include <fstream>
#include <sstream>
#include <filesystem>

#include "Resources/Localization.h"
#include "Utility/ResourceUtility.h"
#include "Logger.h"

void Localization::Deserialize()
{
	indices.clear();
	locales.clear();

	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	localizationCategory = binaryReader.Read<char>();
	const unsigned int count = (GetResourceDataSize() - 1) >> 3;
	const char* data = static_cast<const char*>(GetResourceData());

	for (unsigned int i = 0; i < count; ++i)
	{
		const int index = *reinterpret_cast<const int*>(&data[8 * i + 5]);
		const std::string locale = &data[8 * i + 1];

		indices.push_back(index);
		locales.push_back(locale);
	}

	isResourceDeserialized = true;
}

void Localization::Export(const std::string& outputPath, const std::string& exportOption)
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

char Localization::GetLocalizationCategory()
{
	return localizationCategory;
}

std::vector<int>& Localization::GetIndices()
{
	return indices;
}

std::vector<std::string>& Localization::GetLocales()
{
	return locales;
}

void Localization::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("localizationCategory");
	writer.Uint(localizationCategory);

	writer.String("locales");
	writer.StartArray();

	for (size_t i = 0; i < locales.size(); ++i)
	{
		writer.String(locales[i].c_str());
	}

	writer.EndArray();

	writer.String("indices");
	writer.StartArray();

	for (size_t i = 0; i < indices.size(); ++i)
	{
		writer.Int(indices[i]);
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}

void Localization::ImportFromJson(const std::string& jsonFilePath)
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

	if (document.HasMember("localizationCategory") && document["localizationCategory"].IsUint())
	{
		localizationCategory = static_cast<char>(document["localizationCategory"].GetUint());
	}

	if (document.HasMember("locales") && document["locales"].IsArray() && document.HasMember("indices") && document["indices"].IsArray())
	{
		const rapidjson::Value& jsonLocales = document["locales"];
		const rapidjson::Value& jsonIndices = document["indices"];

		locales.clear();
		indices.clear();

		rapidjson::SizeType count = jsonLocales.Size() < jsonIndices.Size() ? jsonLocales.Size() : jsonIndices.Size();

		locales.reserve(count);
		indices.reserve(count);

		for (rapidjson::SizeType i = 0; i < count; ++i)
		{
			if (jsonLocales[i].IsString())
			{
				locales.push_back(jsonLocales[i].GetString());
			}

			if (jsonIndices[i].IsInt())
			{
				indices.push_back(jsonIndices[i].GetInt());
			}
		}
	}

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Imported localization from {} ({} entries)", jsonFilePath, locales.size()));
}

bool Localization::SerializeToBinary(std::vector<unsigned char>& outputBuffer)
{
	std::vector<unsigned char> buffer;

	buffer.push_back(static_cast<unsigned char>(localizationCategory));

	for (size_t i = 0; i < locales.size(); ++i)
	{
		const std::string& locale = locales[i];
		size_t copyLen = locale.size() < 4 ? locale.size() : 4;
		buffer.insert(buffer.end(), locale.begin(), locale.begin() + copyLen);
		
		// Pad with null bytes until the string block is exactly 4 bytes long
		while (buffer.size() % 8 != 5)
		{
			buffer.push_back('\0');
		}

		const int index = indices[i];
		buffer.insert(buffer.end(), reinterpret_cast<const unsigned char*>(&index), reinterpret_cast<const unsigned char*>(&index) + sizeof(index));
	}

	outputBuffer = buffer;

	return true;
}

bool Localization::PatchResourceLibrary()
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
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to serialize localization to binary");

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

	delete[] static_cast<unsigned char*>(resourceData);

	resourceDataSize = static_cast<unsigned int>(newResourceData.size());
	resourceData = new unsigned char[resourceDataSize];
	memcpy(resourceData, newResourceData.data(), resourceDataSize);

	isResourceDeserialized = false;
	Deserialize();

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Patched resource {} in {} (old size: {}, new size: {})", resourceID, resourceLibraryFilePath, resourceDataSize, newResourceData.size()));

	return true;
}
