#include "Resources/WaveBankFSBF.h"

FSB::Format WaveBankFSBF::GetFormat()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	unsigned int offset = offsetof(FSB::Header, mode);

	binaryReader.Seek(16 + offset);

	return static_cast<FSB::Format>(binaryReader.Read<unsigned int>());
}

void WaveBankFSBF::GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size)
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	binaryReader.Seek(16);

	fsb5Data = binaryReader.GetBuffer(true);
	fsb5Size = static_cast<unsigned int>(binaryReader.GetSize()) - 16;
}
