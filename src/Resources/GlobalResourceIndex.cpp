#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/GlobalResourceIndex.h"
#include "Utility/ResourceUtility.h"

void GlobalResourceIndex::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* resourceIndex = binaryDeserializer.Deserialize(resourceData, resourceDataSize);

	Parse(resourceIndex);

	operator delete(resourceIndex, std::align_val_t(binaryDeserializer.GetAlignment()));

	isResourceDeserialized = true;
}

void GlobalResourceIndex::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		resourceIndex->SerializeToJson(outputPath);
	}
}

void GlobalResourceIndex::Parse(void* resourceIndex)
{
	this->resourceIndex = std::make_shared<SResourceIndex>();

	const unsigned int foldersStartAddress = *reinterpret_cast<unsigned int*>(resourceIndex);
	const unsigned int foldersEndAddress = *(reinterpret_cast<unsigned int*>(resourceIndex) + 1);
	const unsigned int folderCount = ResourceUtility::CalculateArrayElementsCount(foldersStartAddress, foldersEndAddress, 0x14); //0x14 is size of SResourceIndexFolder

	this->resourceIndex->folders.Resize(folderCount);

	for (unsigned int i = 0; i < folderCount; i++)
	{
		const unsigned int folderAddress = foldersStartAddress + 0x14 * i;
		SResourceIndexFolder* resourceIndexFolder = ResourceUtility::Convert4ByteAddressTo8BytePointer<SResourceIndexFolder>(resourceIndex, folderAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(resourceIndexFolder) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(resourceIndex, charsAddress);

		this->resourceIndex->folders[i].name = ZString(chars);

		const unsigned int resourceIndicesStartAddress = *(reinterpret_cast<unsigned int*>(resourceIndexFolder) + 2);
		const unsigned int resourceIndicesEndAddress = *(reinterpret_cast<unsigned int*>(resourceIndexFolder) + 3);
		const unsigned int resourceIndexCount = ResourceUtility::CalculateArrayElementsCount(resourceIndicesStartAddress, resourceIndicesEndAddress, sizeof(int));

		this->resourceIndex->folders[i].resourceIndices.Resize(resourceIndexCount);

		for (unsigned int j = 0; j < resourceIndexCount; j++)
		{
			const unsigned int resourceIndexAddress = resourceIndicesStartAddress + sizeof(int) * j;
			int* resourceIndex2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<int>(resourceIndex, resourceIndexAddress);

			this->resourceIndex->folders[i].resourceIndices[j] = *resourceIndex2;
		}
	}
}

std::shared_ptr<SResourceIndex> GlobalResourceIndex::GetResourceIndex()
{
	return resourceIndex;
}
