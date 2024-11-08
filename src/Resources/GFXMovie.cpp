#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/GFXMovie.h"
#include "Utility/ResourceUtility.h"

GFXMovie::~GFXMovie()
{
	if (gfxResource)
	{
		delete[] gfxResource->gfxData;
	}
}

void GFXMovie::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* gfxResource = binaryDeserializer.Deserialize(resourceData, resourceDataSize);

	Parse(gfxResource);

	operator delete(gfxResource, std::align_val_t(binaryDeserializer.GetAlignment()));

	isResourceDeserialized = true;
}

void GFXMovie::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		const std::string fileName = outputPath.substr(outputPath.find_last_of("\\") + 1);
		const std::string gfxFilePath = std::format("{}\\{}.gfx", outputPath, fileName);
		BinaryWriter binaryWriter = BinaryWriter(gfxFilePath);

		binaryWriter.Write(gfxResource->gfxData, gfxResource->gfxDataLength);

		for (size_t i = 0; i < gfxResource->filenames.Size(); ++i)
		{
			const std::string ddsFilePath = std::format("{}\\{}", outputPath, gfxResource->filenames[i].ToCString());
			BinaryWriter binaryWriter = BinaryWriter(ddsFilePath);

			binaryWriter.Write(gfxResource->fileContents[i].GetStart(), gfxResource->fileContents[i].Size());
		}
	}
}

void GFXMovie::Parse(void* gfxResource)
{
	this->gfxResource = std::make_shared<SGfxResource>();

	const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(gfxResource) + 1);
	const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(gfxResource, charsAddress);

	this->gfxResource->movieName = ZString(chars);

	const unsigned int gfxDataAddress = *(reinterpret_cast<unsigned int*>(gfxResource) + 2);
	const unsigned char* gfxData = ResourceUtility::Convert4ByteAddressTo8BytePointer<unsigned char>(gfxResource, gfxDataAddress);
	const unsigned int gfxDataLength = *(reinterpret_cast<unsigned int*>(gfxResource) + 3);

	this->gfxResource->gfxData = new unsigned char[gfxDataLength];
	this->gfxResource->gfxDataLength = gfxDataLength;

	memcpy(this->gfxResource->gfxData, gfxData, gfxDataLength);

	const unsigned int fileNamesStartAddress = *(reinterpret_cast<unsigned int*>(gfxResource) + 4);
	const unsigned int fileNamesEndAddress = *(reinterpret_cast<unsigned int*>(gfxResource) + 5);
	const unsigned int fileNameCount = ResourceUtility::CalculateArrayElementsCount(fileNamesStartAddress, fileNamesEndAddress, 0x8); //0x8 is size of ZString

	this->gfxResource->filenames.Resize(fileNameCount);

	for (unsigned int i = 0; i < fileNameCount; i++)
	{
		const unsigned int fileNameAddress = fileNamesStartAddress + 0x8 * i;
		ZString* string = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZString>(gfxResource, fileNameAddress);

		const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(string) + 1);
		const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(string, charsAddress2);

		this->gfxResource->filenames[i] = ZString(chars2);
	}

	const unsigned int fileContentsStartAddress = *(reinterpret_cast<unsigned int*>(gfxResource) + 7);
	const unsigned int fileContentsEndAddress = *(reinterpret_cast<unsigned int*>(gfxResource) + 8);
	const unsigned int fileContentCount = ResourceUtility::CalculateArrayElementsCount(fileContentsStartAddress, fileContentsEndAddress, 0xC); //0xC is size of TArray<unsigned char>

	this->gfxResource->fileContents.Resize(fileContentCount);

	for (unsigned int i = 0; i < fileContentCount; i++)
	{
		const unsigned int fileContentAddress = fileContentsStartAddress + 0xC * i;
		TArray<unsigned char>* array = ResourceUtility::Convert4ByteAddressTo8BytePointer<TArray<unsigned char>>(gfxResource, fileContentAddress);

		const unsigned int dataStartAddress = *reinterpret_cast<unsigned int*>(array);
		const unsigned int dataEndAddress = *(reinterpret_cast<unsigned int*>(array) + 1);
		const unsigned int byteCount = ResourceUtility::CalculateArrayElementsCount(dataStartAddress, dataEndAddress, 0x1); //0xC is size of TArray<unsigned char>

		this->gfxResource->fileContents[i].Resize(byteCount);

		for (unsigned int j = 0; j < byteCount; j++)
		{
			const unsigned int dataAddress = dataStartAddress + j;
			unsigned char* byte = ResourceUtility::Convert4ByteAddressTo8BytePointer<unsigned char>(array, dataAddress);

			this->gfxResource->fileContents[i][j] = *byte;
		}
	}
}

std::shared_ptr<SGfxResource> GFXMovie::GetGfxResource()
{
	return gfxResource;
}
