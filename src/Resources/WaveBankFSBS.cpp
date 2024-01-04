#include "Resources/WaveBankFSBS.h"
#include "Utility/StringUtility.h"

FSB::Format WaveBankFSBS::GetFormat()
{
	void* fsb5Data = nullptr;
	unsigned int fsb5Size = 0;

	GetFSB5Data(fsb5Data, fsb5Size);

	BinaryReader binaryReader = BinaryReader(fsb5Data, fsb5Size);
	unsigned int offset = offsetof(FSB::Header, mode);

	binaryReader.Seek(offset);

	return static_cast<FSB::Format>(binaryReader.Read<unsigned int>());
}

void WaveBankFSBS::GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size)
{
	const unsigned int fsb5DataOffset = FindFSB5DataOffsetInFSBS();
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	binaryReader.Seek(fsb5DataOffset);

	fsb5Data = binaryReader.GetBuffer(true);
	fsb5Size = static_cast<unsigned int>(binaryReader.GetSize()) - fsb5DataOffset;
}

void WaveBankFSBS::GetFilePaths(std::vector<std::string>& filePaths)
{
	char* resourceData = static_cast<char*>(GetResourceData());
	const unsigned int resourceDataSize = GetResourceDataSize();
	size_t position = 0;

	while (true)
	{
		size_t index = StringUtility::FindString(static_cast<char*>(resourceData), R"(Assembly\Sound\Localization\Dialogue)", resourceDataSize, position);

		if (index == std::string::npos)
		{
			break;
		}

		std::string releaseFilePath;

		while (resourceData[index] != '\0')
		{
			releaseFilePath += resourceData[index];

			++index;
		}

		position = index;

		filePaths.push_back(releaseFilePath);
	}
}

unsigned int WaveBankFSBS::FindFSB5DataOffsetInFSBS()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	binaryReader.Seek(24 + 16);

	unsigned int magic = binaryReader.Read<unsigned int>();
	unsigned int offset = -1;

	if (magic == '5BSF')
	{
		offset = static_cast<unsigned int>(binaryReader.GetPosition()) - 4;
	}
	else
	{
		binaryReader.Seek(36);

		magic = binaryReader.Read<unsigned int>();

		if (magic == 'ECAF')
		{
			binaryReader.Seek(24);

			const unsigned int size = binaryReader.Read<unsigned int>();

			binaryReader.Seek(binaryReader.GetPosition() - 4 + size);

			while (true)
			{
				if (binaryReader.Read<unsigned char>() == 'F')
				{
					binaryReader.Seek(binaryReader.GetPosition() - 1);

					magic = binaryReader.Read<unsigned int>();

					if (magic == '5BSF')
					{
						break;
					}
				}
			}

			offset = static_cast<unsigned int>(binaryReader.GetPosition()) - 4;
		}
	}

	return offset;
}
