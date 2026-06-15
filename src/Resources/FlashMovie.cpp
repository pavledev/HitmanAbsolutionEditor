#include <DDSTextureLoader.h>
#include <WICTextureLoader.h>
#include <format>
#include <fstream>
#include <filesystem>

#include "Resources/FlashMovie.h"
#include "Utility/ResourceUtility.h"
#include "Logger.h"
#include "Editor.h"

void FlashMovie::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		void* textureData = nullptr;
		unsigned int textureDataSize;
		bool isDDSTexture;

		GetTextureData(textureData, textureDataSize, isDDSTexture);

		BinaryWriter binaryWriter = BinaryWriter(outputPath);

		binaryWriter.Write(textureData, textureDataSize);
	}
}

void FlashMovie::GetTextureData(void*& textureData, unsigned int& textureDataSize, bool& isDDSTexture)
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	const unsigned char type = binaryReader.Read<unsigned char>();
	const unsigned int fileOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(fileOffset + 1);

	size_t currentPosition = binaryReader.GetPosition();

	binaryReader.Seek(0, SeekOrigin::End);

	textureDataSize = binaryReader.GetPosition() - currentPosition;

	binaryReader.Seek(fileOffset + 1);

	textureData = binaryReader.GetBuffer(true);
	isDDSTexture = type == 'd';
}

void FlashMovie::CreateTextureFromMemory()
{
	void* textureData = nullptr;
	unsigned int textureDataSize;
	bool isDDSTexture;

	GetTextureData(textureData, textureDataSize, isDDSTexture);

	if (isDDSTexture)
	{
		DirectX::CreateDDSTextureFromMemory(Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), static_cast<unsigned char*>(textureData), textureDataSize, &texture, &textureView);
	}
	else
	{
		DirectX::CreateWICTextureFromMemory(Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), static_cast<unsigned char*>(textureData), textureDataSize, &texture, &textureView);
	}

	ID3D11Texture2D* texture2D = nullptr;
	D3D11_TEXTURE2D_DESC textureDesc;

	texture->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture2D));
	texture2D->GetDesc(&textureDesc);

	textureWidth = textureDesc.Width;
	textureHeight = textureDesc.Height;

	texture2D->Release();
}

const FlashMovie::Format FlashMovie::GetFormat() const
{
	const unsigned char type = *static_cast<unsigned char*>(resourceData);

	switch (type)
	{
		case 'd':
			return Format::DDS;
		case 'p':
			return Format::PNG;
		case 's':
			return Format::SWF;
		default:
			return Format::None;
	}
}

const unsigned int FlashMovie::GetTextureWidth() const
{
	return textureWidth;
}

const unsigned int FlashMovie::GetTextureHeight() const
{
	return textureHeight;
}

ID3D11Resource* FlashMovie::GetTexture() const
{
	return texture;
}

ID3D11ShaderResourceView* FlashMovie::GetTextureView() const
{
	return textureView;
}

void FlashMovie::ImportSwf(const std::string& swfFilePath)
{
	// No-op: data is read directly in PatchResourceLibrary
}

bool FlashMovie::PatchResourceLibrary(const std::string& swfFilePath)
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

	std::ifstream swfFile(swfFilePath, std::ios::binary | std::ios::ate);

	if (!swfFile.is_open())
	{
		Logger::GetInstance().Log(Logger::Level::Error, std::format("Failed to open SWF file: {}", swfFilePath));

		return false;
	}

	std::streamsize swfFileSize = swfFile.tellg();

	swfFile.seekg(0, std::ios::beg);

	std::vector<unsigned char> swfData(swfFileSize);
	swfFile.read(reinterpret_cast<char*>(swfData.data()), swfFileSize);
	swfFile.close();

	std::vector<unsigned char> newResourceData(5 + swfData.size());
	newResourceData[0] = 's';
	const unsigned int fileOffset = 4;
	memcpy(newResourceData.data() + 1, &fileOffset, sizeof(fileOffset));
	memcpy(newResourceData.data() + 5, swfData.data(), swfData.size());

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Loaded SWF from {} ({} bytes)", swfFilePath, swfData.size()));

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

	Logger::GetInstance().Log(Logger::Level::Info, std::format("Patched resource {} in {} (old size: {}, new size: {})", resourceID, resourceLibraryFilePath, resourceDataSize, newResourceData.size()));

	return true;
}
