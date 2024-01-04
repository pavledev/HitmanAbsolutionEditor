#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/PackageList.h"
#include "Utility/ResourceUtility.h"

void PackageList::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* packageListData = binaryDeserializer.Deserialize(resourceData, resourceDataSize);

	Parse(packageListData);

	operator delete(packageListData, std::align_val_t(binaryDeserializer.GetAlignment()));

	isResourceDeserialized = true;
}

void PackageList::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		packageListData->SerializeToJson(outputPath);
	}
}

void PackageList::Parse(void* packageListData)
{
	this->packageListData = std::make_shared<SPackageListData>();

	const unsigned int foldersStartAddress = *reinterpret_cast<unsigned int*>(packageListData);
	const unsigned int foldersEndAddress = *(reinterpret_cast<unsigned int*>(packageListData) + 1);
	const unsigned int folderCount = ResourceUtility::CalculateArrayElementsCount(foldersStartAddress, foldersEndAddress, 0x14); //0x14 is size of SPackageListDataFolder

	this->packageListData->folders.Resize(folderCount);

	for (unsigned int i = 0; i < folderCount; i++)
	{
		const unsigned int folderAddress = foldersStartAddress + 0x14 * i;
		SPackageListDataFolder* packageListDataFolder = ResourceUtility::Convert4ByteAddressTo8BytePointer<SPackageListDataFolder>(packageListData, folderAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(packageListDataFolder) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(packageListData, charsAddress);

		this->packageListData->folders[i].name = ZString(chars);

		const unsigned int sectionsStartAddress = *(reinterpret_cast<unsigned int*>(packageListDataFolder) + 2);
		const unsigned int sectionsEndAddress = *(reinterpret_cast<unsigned int*>(packageListDataFolder) + 3);
		const unsigned int sectionIndexCount = ResourceUtility::CalculateArrayElementsCount(sectionsStartAddress, sectionsEndAddress, 0x18); //0x14 is size of SPackageListDataSection

		this->packageListData->folders[i].sections.Resize(sectionIndexCount);

		for (unsigned int j = 0; j < sectionIndexCount; j++)
		{
			const unsigned int sectionAddress = sectionsStartAddress + 0x18 * j;
			SPackageListDataSection* packageListDataSection = ResourceUtility::Convert4ByteAddressTo8BytePointer<SPackageListDataSection>(packageListData, sectionAddress);

			const unsigned int sceneResourceIDsStartAddress = *reinterpret_cast<unsigned int*>(packageListDataSection);
			const unsigned int sceneResourceIDsEndAddress = *(reinterpret_cast<unsigned int*>(packageListDataSection) + 1);
			const unsigned int sceneResourceIDIndexCount = ResourceUtility::CalculateArrayElementsCount(sceneResourceIDsStartAddress, sceneResourceIDsEndAddress, 0x8); //0x8 is size of ZString

			this->packageListData->folders[i].sections[j].sceneResourceIDs.Resize(sceneResourceIDIndexCount);

			for (unsigned int k = 0; k < sceneResourceIDIndexCount; k++)
			{
				const unsigned int sceneResourceIDAddress = sceneResourceIDsStartAddress + 0x8 * k;
				ZString* sceneResourceID = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZString>(packageListData, sceneResourceIDAddress);

				const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(sceneResourceID) + 1);
				const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(packageListData, charsAddress2);

				this->packageListData->folders[i].sections[j].sceneResourceIDs[k] = ZString(chars2);
			}

			const unsigned int resourcesStartAddress = *(reinterpret_cast<unsigned int*>(packageListDataSection) + 3);
			const unsigned int resourcesEndAddress = *(reinterpret_cast<unsigned int*>(packageListDataSection) + 4);
			const unsigned int resourceCount = ResourceUtility::CalculateArrayElementsCount(resourcesStartAddress, resourcesEndAddress, 0x8); //0x8 is size of ZString

			this->packageListData->folders[i].sections[j].resources.Resize(resourceCount);

			for (unsigned int k = 0; k < resourceCount; k++)
			{
				const unsigned int resourceAddress = resourcesStartAddress + 0x8 * k;
				ZString* resource = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZString>(packageListData, resourceAddress);

				const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(resource) + 1);
				const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(packageListData, charsAddress2);

				this->packageListData->folders[i].sections[j].resources[k] = ZString(chars2);
			}
		}
	}
}

std::shared_ptr<SPackageListData> PackageList::GetPackageListData()
{
	return packageListData;
}
