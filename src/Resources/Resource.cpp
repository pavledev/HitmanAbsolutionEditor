#include <sstream>
#include <iomanip>

#include "Glacier/Resource/ZResourceHeaderReader.h"

#include "Resources/Resource.h"
#include "Utility/ResourceUtility.h"
#include "Logger.h"
#include "Registry/ResourceInfoRegistry.h"
#include "IO/BinaryWriter.h"

Resource::Resource()
{
	resourceID = "";
	offsetInHeaderLibrary = 0;
	offsetInResourceLibrary = 0;
	headerData = nullptr;
	headerDataSize = 0;
	resourceData = nullptr;
	resourceDataSize = 0;
	isResourceLoaded = false;
	isResourceDeserialized = false;
}

Resource::~Resource()
{
	if (headerData)
	{
		delete[] headerData;
	}

	if (resourceData)
	{
		delete[] resourceData;
	}
}

const std::string Resource::GetHeaderLibraryFilePath() const
{
	return headerLibraryFilePath;
}

const std::string Resource::GetResourceLibraryFilePath() const
{
	return resourceLibraryFilePath;
}

const std::string Resource::GetName() const
{
	return name;
}

const SResourceHeaderHeader& Resource::GetResourceHeaderHeader() const
{
	return resourceHeaderHeader;
}

const ZRuntimeResourceID& Resource::GetRuntimeResourceID() const
{
	return runtimeResourceID;
}

const unsigned long long Resource::GetHash() const
{
	return hash;
}

std::string Resource::GetResourceID() const
{
	return resourceID;
}

std::string Resource::GetResourceID()
{
	return resourceID;
}

std::string Resource::GetHeaderLibraryChunkResourceID() const
{
	return headerLibraryChunkResourceID;
}

unsigned int Resource::GetIndexInLibrary() const
{
	return indexInLibrary;
}

const EResourceReferenceFlags Resource::GetResourceReferenceFlags() const
{
	return resourceReferenceFlags;
}

const unsigned int Resource::GetOffsetInHeaderLibrary() const
{
	return offsetInHeaderLibrary;
}

const unsigned int Resource::GetOffsetInResourceLibrary() const
{
	return offsetInResourceLibrary;
}

const void* Resource::GetHeaderData() const
{
	return headerData;
}

const unsigned int Resource::GetHeaderDataSize() const
{
	return headerDataSize;
}

const void* Resource::GetResourceData() const
{
	return resourceData;
}

void* Resource::GetResourceData()
{
	return resourceData;
}

const unsigned int Resource::GetResourceDataSize() const
{
	return resourceDataSize;
}

const std::vector<std::shared_ptr<Resource>>& Resource::GetReferences() const
{
	return references;
}

std::vector<std::shared_ptr<Resource>>& Resource::GetReferences()
{
	return references;
}

const std::vector<std::shared_ptr<Resource>>& Resource::GetBackReferences() const
{
	return backReferences;
}

std::vector<std::shared_ptr<Resource>>& Resource::GetBackReferences()
{
	return backReferences;
}

void Resource::SetHeaderLibraryFilePath(std::string headerLibraryFilePath)
{
	this->headerLibraryFilePath = headerLibraryFilePath;
}

void Resource::SetResourceLibraryFilePath(std::string resourceLibraryFilePath)
{
	this->resourceLibraryFilePath = resourceLibraryFilePath;
}

void Resource::SetName(std::string name)
{
	this->name = name;
}

void Resource::SetResourceHeaderHeader(const SResourceHeaderHeader& resourceHeaderHeader)
{
	this->resourceHeaderHeader = resourceHeaderHeader;
}

void Resource::SetHash(const unsigned long long hash)
{
	this->hash = hash;
}

void Resource::SetRuntimeResourceID(const ZRuntimeResourceID& runtimeResourceID)
{
	this->runtimeResourceID = runtimeResourceID;
}

void Resource::SetResourceID(std::string resourceID)
{
	this->resourceID = resourceID;
}

void Resource::SetResourceReferenceFlags(const EResourceReferenceFlags resourceReferenceFlags)
{
	this->resourceReferenceFlags = resourceReferenceFlags;
}

void Resource::SetHeaderData(void* headerData)
{
	this->headerData = headerData;
}

void Resource::SetHeaderDataSize(const unsigned int headerDataSize)
{
	this->headerDataSize = headerDataSize;
}

void Resource::SetResourceData(void* resourceData)
{
	this->resourceData = resourceData;
}

void Resource::SetResourceDataSize(const unsigned int resourceDataSize)
{
	this->resourceDataSize = resourceDataSize;
}

void Resource::LoadResourceData()
{
	const std::string resourceFilePath = ResourceUtility::ConvertResourceIDToFilePath(resourceID);
	BinaryReader resourceBinaryReader = BinaryReader(resourceFilePath);

	resourceHeaderHeader = resourceBinaryReader.Read<SResourceHeaderHeader>();
	resourceDataSize = resourceBinaryReader.GetSize() - sizeof(SResourceHeaderHeader);
	resourceData = resourceBinaryReader.Read<void>(resourceDataSize);
}

void Resource::LoadResourceData(BinaryReader& headerLibraryBinaryReader, const unsigned int resourceHeadersStartOffset, unsigned int resourceHeaderOffset)
{
	headerLibraryBinaryReader.Seek(resourceHeadersStartOffset - 4, SeekOrigin::Begin);

	unsigned int resourceHeaderCount = headerLibraryBinaryReader.Read<unsigned int>();
	unsigned int resourceLibraryOffset = 0x18;
	unsigned int currentPosition = 0;

	for (unsigned int i = 0; i < resourceHeaderCount; ++i)
	{
		currentPosition = headerLibraryBinaryReader.GetPosition();
		resourceHeaderOffset = currentPosition + headerLibraryBinaryReader.Read<unsigned int>();
		headerLibraryBinaryReader.Seek(resourceHeaderOffset, SeekOrigin::Begin);

		unsigned int type = headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Skip(8);

		unsigned int dataSize = headerLibraryBinaryReader.Read<unsigned int>();

		if (type == 'FSBM' || type == 'FSBS')
		{
			dataSize += 24;
		}

		if (i == indexInLibrary)
		{
			BinaryReader resourceLibraryBinaryReader = BinaryReader(resourceLibraryFilePath);

			resourceLibraryBinaryReader.Seek(resourceLibraryOffset);

			resourceDataSize = dataSize;
			resourceData = resourceLibraryBinaryReader.Read<void>(resourceDataSize);

			break;
		}

		resourceLibraryOffset += dataSize;

		headerLibraryBinaryReader.Seek(currentPosition + 0xC, SeekOrigin::Begin);
	}

	offsetInResourceLibrary = resourceLibraryOffset;
}

void Resource::DeleteResourceData()
{
	delete[] resourceData;

	resourceData = nullptr;
}

const bool Resource::HasReferences() const
{
	return resourceHeaderHeader.m_nReferencesChunkSize > 0;
}

void Resource::LoadResource(const unsigned int headerLibraryIndex, const unsigned int chunkIndex, const unsigned int indexInLibrary, const bool loadReferences, const bool loadBackReferences, const bool loadResourceData)
{
	try
	{
		if (chunkIndex != -1)
		{
			std::string headerLibraryResourceID;

			if ((*headerLibraries)[headerLibraryIndex].resourceID.starts_with("["))
			{
				headerLibraryResourceID = (*headerLibraries)[headerLibraryIndex].resourceID;
			}
			else
			{
				headerLibraryResourceID = std::format("[[assembly:/Common/PC.layoutconfig].pc_layoutdef](assembly:/scenes/{}/{}_main.entity).pc_headerlib", (*headerLibraries)[headerLibraryIndex].resourceID, (*headerLibraries)[headerLibraryIndex].resourceID);
			}

			isResourceLoaded = false;
			headerLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(headerLibraryResourceID);

			if (!std::filesystem::exists(headerLibraryFilePath))
			{
				headerLibraryFilePath = ResourceUtility::FindDLCFilePath(headerLibraryResourceID);
			}

			this->indexInLibrary = indexInLibrary;

			BinaryReader headerLibraryBinaryReader = BinaryReader(headerLibraryFilePath);

			headerLibraryBinaryReader.Seek(4, SeekOrigin::Begin);

			unsigned int referencesChunkSize = headerLibraryBinaryReader.Read<unsigned int>();

			headerLibraryBinaryReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

			unsigned int chunksStartOffset = headerLibraryBinaryReader.GetPosition();
			chunksStartOffset += headerLibraryBinaryReader.Read<unsigned int>();
			unsigned int chunkOffset = chunksStartOffset + 0x50 * chunkIndex; //0x50 is size of SHeaderLibraryChunk

			headerLibraryBinaryReader.Seek(chunkOffset + 0x4, SeekOrigin::Begin);

			unsigned int headerLibraryChunkResourceIDOffset = headerLibraryBinaryReader.GetPosition();
			headerLibraryChunkResourceIDOffset += headerLibraryBinaryReader.Read<unsigned int>();

			headerLibraryBinaryReader.Seek(headerLibraryChunkResourceIDOffset - 4, SeekOrigin::Begin);

			unsigned int resourceIDLength = headerLibraryBinaryReader.Read<unsigned int>();
			headerLibraryChunkResourceID = headerLibraryBinaryReader.ReadString(static_cast<size_t>(resourceIDLength - 1));

			resourceLibraryFilePath = ResourceUtility::ConvertResourceIDToFilePath(headerLibraryChunkResourceID);

			if (!std::filesystem::exists(resourceLibraryFilePath))
			{
				resourceLibraryFilePath = ResourceUtility::FindDLCFilePath(headerLibraryChunkResourceID);
			}

			headerLibraryBinaryReader.Seek(chunkOffset + 0x44, SeekOrigin::Begin);

			unsigned int runtimeResourceIDsStartOffset = headerLibraryBinaryReader.GetPosition();
			runtimeResourceIDsStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

			headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset + indexInLibrary * 0x8, SeekOrigin::Begin);

			runtimeResourceID = headerLibraryBinaryReader.Read<unsigned long long>();

			headerLibraryBinaryReader.Seek(chunkOffset + 0x2C, SeekOrigin::Begin);

			unsigned int resourceHeadersStartOffset = headerLibraryBinaryReader.GetPosition();
			resourceHeadersStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

			headerLibraryBinaryReader.Seek(resourceHeadersStartOffset + indexInLibrary * 0xC, SeekOrigin::Begin);

			unsigned int resourceHeaderOffset = headerLibraryBinaryReader.GetPosition();
			resourceHeaderOffset += headerLibraryBinaryReader.Read<unsigned int>();
			offsetInHeaderLibrary = resourceHeaderOffset;

			headerLibraryBinaryReader.Seek(resourceHeaderOffset + 4, SeekOrigin::Begin);

			unsigned int referencesChunkSize2 = headerLibraryBinaryReader.Read<unsigned int>();
			headerDataSize = sizeof(SResourceHeaderHeader) + referencesChunkSize2;

			headerLibraryBinaryReader.Seek(resourceHeaderOffset, SeekOrigin::Begin);

			headerData = headerLibraryBinaryReader.Read<void>(headerDataSize);

			BinaryReader headerDataBinaryReader = BinaryReader(headerData, headerDataSize);

			resourceHeaderHeader = headerDataBinaryReader.Read<SResourceHeaderHeader>();

			if (loadReferences && resourceHeaderHeader.m_nReferencesChunkSize > 0)
			{
				LoadReferences(headerDataBinaryReader);
			}

			if (loadBackReferences)
			{
				LoadBackReferences(headerLibraryBinaryReader, referencesChunkSize);
			}

			if (loadResourceData)
			{
				LoadResourceData(headerLibraryBinaryReader, resourceHeadersStartOffset, resourceHeaderOffset);
			}
		}
		else
		{
			if (loadResourceData)
			{
				LoadResourceData();
			}
		}

		isResourceLoaded = true;

		if (resourceLoadedCallback)
		{
			resourceLoadedCallback();
		}
	}
	catch (std::ios_base::failure& ex)
	{
		Logger::GetInstance().Log(Logger::Level::Error, ex.what());
	}
}

void Resource::LoadReferences()
{
	BinaryReader headerDataBinaryReader = BinaryReader(headerData, headerDataSize);

	LoadReferences(headerDataBinaryReader);
}

void Resource::LoadReferences(const BinaryReader& headerDataBinaryReader)
{
	references.clear();

	unsigned char* referencesChunk = headerDataBinaryReader.Read<unsigned char>(resourceHeaderHeader.m_nReferencesChunkSize);
	const ZResourceHeaderReader resourceHeaderReader = ZResourceHeaderReader(&resourceHeaderHeader, referencesChunk);
	const unsigned int numberOfReferences = resourceHeaderReader.GetNumResourceIdentifiers();

	for (unsigned int i = 0; i < numberOfReferences; ++i)
	{
		const ZRuntimeResourceID runtimeResourceID = resourceHeaderReader.GetResourceIdentifier(i);
		const EResourceReferenceFlags flags = resourceHeaderReader.GetResourceFlags(i);
		const std::string referenceResourceID = ZRuntimeResourceID::QueryResourceID(runtimeResourceID);
		const unsigned long long referenceHash = Hash::GetMD5(referenceResourceID);

		const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(referenceHash);
		std::shared_ptr<Resource> resourceReference = ResourceUtility::CreateResource(resourceInfo.type);

		resourceReference->runtimeResourceID = runtimeResourceID;
		resourceReference->resourceReferenceFlags = flags;
		resourceReference->resourceID = referenceResourceID;
		resourceReference->hash = referenceHash;
		resourceReference->headerLibraries = &resourceInfo.headerLibraries;

		references.push_back(resourceReference);
	}

	delete[] referencesChunk;
}

void Resource::LoadBackReferences(BinaryReader& headerLibraryBinaryReader, const unsigned int referencesChunkSize)
{
	backReferences.clear();

	headerLibraryBinaryReader.Seek(referencesChunkSize + 24 + 16, SeekOrigin::Begin);

	unsigned int chunksStartOffset = headerLibraryBinaryReader.GetPosition();
	chunksStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

	headerLibraryBinaryReader.Seek(chunksStartOffset - 4, SeekOrigin::Begin);

	unsigned int chunkCount = headerLibraryBinaryReader.Read<unsigned int>();

	for (unsigned int i = 0; i < chunkCount; ++i)
	{
		unsigned int chunkOffset = chunksStartOffset + 0x50 * i; //0x50 is size of SHeaderLibraryChunk

		headerLibraryBinaryReader.Seek(chunkOffset + 0x2C, SeekOrigin::Begin);

		unsigned int resourceHeadersStartOffset = headerLibraryBinaryReader.GetPosition();
		resourceHeadersStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

		headerLibraryBinaryReader.Seek(resourceHeadersStartOffset - 4, SeekOrigin::Begin);

		unsigned int resourceHeaderCount = headerLibraryBinaryReader.Read<unsigned int>();

		for (unsigned int j = 0; j < resourceHeaderCount; ++j)
		{
			unsigned int resourceHeaderOffset = resourceHeadersStartOffset + 0xC * j; //0xC is size of TArray

			headerLibraryBinaryReader.Seek(resourceHeaderOffset, SeekOrigin::Begin);

			size_t currentPosition = headerLibraryBinaryReader.GetPosition();

			headerLibraryBinaryReader.Seek(currentPosition + headerLibraryBinaryReader.Read<unsigned int>(), SeekOrigin::Begin);

			SResourceHeaderHeader resourceHeaderHeader2 = headerLibraryBinaryReader.Read<SResourceHeaderHeader>();

			if (resourceHeaderHeader2.m_nReferencesChunkSize == 0)
			{
				continue;
			}

			unsigned char* referencesChunk = headerLibraryBinaryReader.Read<unsigned char>(resourceHeaderHeader2.m_nReferencesChunkSize);
			const ZResourceHeaderReader resourceHeaderReader = ZResourceHeaderReader(&resourceHeaderHeader2, referencesChunk);
			const unsigned int numberOfReferences = resourceHeaderReader.GetNumResourceIdentifiers();

			for (unsigned int k = 0; k < numberOfReferences; ++k)
			{
				const ZRuntimeResourceID runtimeResourceID = resourceHeaderReader.GetResourceIdentifier(k);

				if (runtimeResourceID != this->runtimeResourceID)
				{
					continue;
				}

				size_t currentPosition = headerLibraryBinaryReader.GetPosition();

				headerLibraryBinaryReader.Seek(chunkOffset + 0x44, SeekOrigin::Begin);

				unsigned int runtimeResourceIDsStartOffset = headerLibraryBinaryReader.GetPosition();
				runtimeResourceIDsStartOffset += headerLibraryBinaryReader.Read<unsigned int>();

				headerLibraryBinaryReader.Seek(runtimeResourceIDsStartOffset + j * 0x8, SeekOrigin::Begin);

				const unsigned long long backReferenceRuntimeResourceID = headerLibraryBinaryReader.Read<unsigned long long>();
				const std::string backReferenceResourceID = ZRuntimeResourceID::QueryResourceID(backReferenceRuntimeResourceID);
				const unsigned long long backReferenceHash = Hash::GetMD5(backReferenceResourceID);
				const EResourceReferenceFlags flags = resourceHeaderReader.GetResourceFlags(k);

				const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(backReferenceHash);
				std::shared_ptr<Resource> backReference = ResourceUtility::CreateResource(resourceInfo.type);

				backReference->runtimeResourceID = ZRuntimeResourceID(backReferenceRuntimeResourceID);
				backReference->resourceReferenceFlags = flags;
				backReference->resourceID = backReferenceResourceID;
				backReference->hash = backReferenceHash;
				backReference->headerLibraries = &resourceInfo.headerLibraries;

				backReferences.push_back(backReference);

				headerLibraryBinaryReader.Seek(currentPosition);
			}

			delete[] referencesChunk;
		}
	}
}

bool Resource::IsResourceLoaded() const
{
	return isResourceLoaded;
}

const std::vector<Resource::HeaderLibrary>* Resource::GetHeaderLibraries() const
{
	return headerLibraries;
}

void Resource::SetHeaderLibraries(const std::vector<HeaderLibrary>* headerLibraries)
{
	this->headerLibraries = headerLibraries;
}

void Resource::AddReference(const ZRuntimeResourceID& runtimeResourceID, const EResourceReferenceFlags resourceReferenceFlags)
{
	const std::string referenceResourceID = ZRuntimeResourceID::QueryResourceID(runtimeResourceID);
	const unsigned long long referenceHash = Hash::GetMD5(referenceResourceID);

	const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(referenceHash);
	std::shared_ptr<Resource> resourceReference = ResourceUtility::CreateResource(resourceInfo.type);

	resourceReference->runtimeResourceID = runtimeResourceID;
	resourceReference->resourceReferenceFlags = resourceReferenceFlags;
	resourceReference->resourceID = referenceResourceID;
	resourceReference->hash = referenceHash;
	resourceReference->headerLibraries = &resourceInfo.headerLibraries;

	references.push_back(resourceReference);
}

void Resource::SetResourceLoadedCallback(ResourceLoadedCallback resourceLoadedCallback)
{
	this->resourceLoadedCallback = resourceLoadedCallback;
}

const bool Resource::IsResourceDeserialized() const
{
	return isResourceDeserialized;
}

void Resource::ExportRawData(const std::string& outputFilePath)
{
	BinaryWriter binaryWriter = BinaryWriter(outputFilePath);

	binaryWriter.Write(resourceData, resourceDataSize);
}
