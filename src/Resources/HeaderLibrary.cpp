#include <vector>

#include "Resources/HeaderLibrary.h"
#include "Utility/ResourceUtility.h"
#include "Glacier/Resource/ZResourceHeaderReader.h"
#include "Logger.h"

const std::string& HeaderLibrary::GetFilePath() const
{
	return filePath;
}

const SResourceHeaderHeader& HeaderLibrary::GetResourceHeaderHeader() const
{
	return resourceHeaderHeader;
}

const std::vector<Resource>& HeaderLibrary::GetReferences() const
{
	return references;
}

const std::vector<std::string>& HeaderLibrary::GetChunkResourceIDs() const
{
	return chunkResourceIDs;
}

const std::vector<std::string>& HeaderLibrary::GetExternalResourceIDs() const
{
	return externalResourceIDs;
}

bool HeaderLibrary::ParseHeaderLibrary(std::string& resourceID)
{
	filePath = ResourceUtility::ConvertResourceIDToFilePath(resourceID);

	if (!std::filesystem::exists(filePath))
	{
		filePath = ResourceUtility::FindDLCFilePath(resourceID);
	}

	references.clear();
	chunkResourceIDs.clear();
	externalResourceIDs.clear();

	try
	{
		BinaryReader binaryReader = BinaryReader(filePath);

		resourceHeaderHeader = binaryReader.Read<SResourceHeaderHeader>();

		ParseReferencesChunk(binaryReader);

		binaryReader.Skip(0x30); //Size of BIN2 header + size of SHeaderLibrary

		unsigned int headerLibraryChunkCount = binaryReader.Read<unsigned int>();

		chunkResourceIDs.resize(headerLibraryChunkCount);
		chunkOffsets.resize(headerLibraryChunkCount);

		for (unsigned int i = 0; i < headerLibraryChunkCount; ++i)
		{
			chunkOffsets[i] = binaryReader.GetPosition();

			binaryReader.Skip(4);

			unsigned int resourceIDOffset = static_cast<unsigned int>(binaryReader.GetPosition());
			resourceIDOffset += binaryReader.Read<unsigned int>();
			size_t currentPosition = binaryReader.GetPosition();

			binaryReader.Seek(resourceIDOffset - 4, SeekOrigin::Begin);

			unsigned int resourceIDLength = binaryReader.Read<unsigned int>();
			std::string resourceID = binaryReader.ReadString(static_cast<size_t>(resourceIDLength - 1));

			chunkResourceIDs[i] = resourceID;

			binaryReader.Seek(currentPosition, SeekOrigin::Begin);
			binaryReader.Seek(0x48, SeekOrigin::Current);
		}

		binaryReader.Seek(resourceHeaderHeader.m_nReferencesChunkSize + 0x18 + 0x10 + 0xC, SeekOrigin::Begin);

		unsigned int currentPosition = static_cast<unsigned int>(binaryReader.GetPosition());

		binaryReader.Seek(currentPosition + binaryReader.Read<int>() - 4, SeekOrigin::Begin);

		unsigned int externalResourceIDCount = binaryReader.Read<unsigned int>();

		externalResourceIDs.resize(externalResourceIDCount);

		for (unsigned int i = 0; i < externalResourceIDCount; ++i)
		{
			binaryReader.Skip(4);

			unsigned int externalResourceIDOffset = static_cast<unsigned int>(binaryReader.GetPosition());
			externalResourceIDOffset += binaryReader.Read<unsigned int>();
			size_t currentPosition = binaryReader.GetPosition();

			binaryReader.Seek(externalResourceIDOffset - 4, SeekOrigin::Begin);

			unsigned int externalResourceIDLength = binaryReader.Read<unsigned int>();
			std::string externalResourceID = binaryReader.ReadString(static_cast<size_t>(externalResourceIDLength - 1));

			externalResourceIDs[i] = externalResourceID;

			binaryReader.Seek(currentPosition);
		}
	}
	catch (std::ios_base::failure& ex)
	{
		Logger::GetInstance().Log(Logger::Level::Error, ex.what());

		return false;
	}

	return true;
}

void HeaderLibrary::ParseReferencesChunk(const BinaryReader& binaryReader)
{
	unsigned char* referencesChunk = nullptr;

	if (resourceHeaderHeader.m_nReferencesChunkSize > 0)
	{
		referencesChunk = binaryReader.Read<unsigned char>(resourceHeaderHeader.m_nReferencesChunkSize);
	}

	const ZResourceHeaderReader resourceHeaderReader = ZResourceHeaderReader(&resourceHeaderHeader, referencesChunk);
	const unsigned int numberOfReferences = resourceHeaderReader.GetNumResourceIdentifiers();

	for (unsigned int i = 0; i < numberOfReferences; ++i)
	{
		const ZRuntimeResourceID runtimeResourceID = resourceHeaderReader.GetResourceIdentifier(i);
		const std::string resourceID = ZRuntimeResourceID::QueryResourceID(runtimeResourceID);
		const EResourceReferenceFlags flags = resourceHeaderReader.GetResourceFlags(i);
		Resource reference;

		reference.SetRuntimeResourceID(runtimeResourceID);
		reference.SetHash(Hash::GetMD5(resourceID));
		reference.SetResourceID(resourceID);
		reference.SetResourceReferenceFlags(flags);

		references.push_back(reference);
	}

	if (referencesChunk)
	{
		delete[] referencesChunk;
	}
}

HeaderLibrary::HeaderLibraryChunk HeaderLibrary::ParseHeaderLibraryChunk(const int index)
{
	HeaderLibrary::HeaderLibraryChunk result = HeaderLibrary::HeaderLibraryChunk();

	try
	{
		BinaryReader binaryReader = BinaryReader(filePath);

		binaryReader.Seek(chunkOffsets[index] + 8);

		result.offset = binaryReader.Read<unsigned int>();
		result.flags = static_cast<HeaderLibraryChunkFlags>(binaryReader.Read<char>());

		binaryReader.Skip(3); //Skip padding

		result.localizationCategory = binaryReader.Read<unsigned int>();

		size_t currentPosition = binaryReader.GetPosition();
		unsigned int languagesStartOffset = static_cast<unsigned int>(binaryReader.GetPosition());
		languagesStartOffset += binaryReader.Read<unsigned int>();

		binaryReader.Seek(languagesStartOffset - 4);

		unsigned int languageCount = binaryReader.Read<unsigned int>();

		for (unsigned int i = 0; i < languageCount; ++i)
		{
			binaryReader.Skip(4);

			unsigned int languageOffset = static_cast<unsigned int>(binaryReader.GetPosition());
			languageOffset += binaryReader.Read<unsigned int>();
			size_t currentPosition2 = binaryReader.GetPosition();

			binaryReader.Seek(languageOffset - 4);

			unsigned int languageLength = binaryReader.Read<unsigned int>();
			std::string language = binaryReader.ReadString(static_cast<size_t>(languageLength - 1));

			result.languages.push_back(language);

			binaryReader.Seek(currentPosition2);
		}

		binaryReader.Seek(currentPosition + 0xC);

		currentPosition = binaryReader.GetPosition();
		unsigned int statesStartOffset = static_cast<unsigned int>(binaryReader.GetPosition());
		statesStartOffset += binaryReader.Read<unsigned int>();
		unsigned int statesEndOffset = static_cast<unsigned int>(binaryReader.GetPosition());
		statesEndOffset += binaryReader.Read<unsigned int>();
		unsigned int stateCount = (statesEndOffset - statesStartOffset) / sizeof(int);

		binaryReader.Seek(statesStartOffset);

		for (unsigned int i = 0; i < stateCount; ++i)
		{
			result.states.push_back(binaryReader.Read<int>());
		}

		binaryReader.Seek(currentPosition + 0x18);

		currentPosition = binaryReader.GetPosition();
		unsigned int ridMappingIndicesStartOffset = static_cast<unsigned int>(binaryReader.GetPosition());
		ridMappingIndicesStartOffset += binaryReader.Read<unsigned int>();

		binaryReader.Seek(ridMappingIndicesStartOffset - 4);

		unsigned int ridMappingIndicesCount = binaryReader.Read<unsigned int>();

		for (unsigned int i = 0; i < ridMappingIndicesCount; ++i)
		{
			result.ridMappingIndices.push_back(binaryReader.Read<unsigned int>());
		}

		binaryReader.Seek(currentPosition + 0xC);

		unsigned int ridMappingIDsStartOffset = static_cast<unsigned int>(binaryReader.GetPosition());
		ridMappingIDsStartOffset += binaryReader.Read<unsigned int>();

		binaryReader.Seek(ridMappingIDsStartOffset - 4);

		unsigned int ridMappingIDsCount = binaryReader.Read<unsigned int>();

		for (unsigned int i = 0; i < ridMappingIDsCount; ++i)
		{
			result.ridMappingIDs.push_back(binaryReader.Read<unsigned long long>());
		}

		result.filePath = ResourceUtility::ConvertResourceIDToFilePath(chunkResourceIDs[index]);

		if (!std::filesystem::exists(result.filePath))
		{
			result.filePath = ResourceUtility::FindDLCFilePath(chunkResourceIDs[index]);
		}

		if (std::filesystem::exists(result.filePath))
		{
			BinaryReader binaryReader2 = BinaryReader(result.filePath);

			result.resourceHeaderHeader = binaryReader2.Read<SResourceHeaderHeader>();
		}
	}
	catch (std::ios_base::failure& ex)
	{
		Logger::GetInstance().Log(Logger::Level::Error, ex.what());
	}

	return result;
}
