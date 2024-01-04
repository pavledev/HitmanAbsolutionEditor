#include "Resources/WaveBankFSBM.h"

FSB::Format WaveBankFSBM::GetFormat()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	unsigned int offset = offsetof(FSB::Header, mode);

	binaryReader.Seek(24 + offset);

	return static_cast<FSB::Format>(binaryReader.Read<unsigned int>());
}

void WaveBankFSBM::GetFSB5Data(void*& fsb5Data, unsigned int& fsb5Size)
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

	binaryReader.Seek(24);

	fsb5Data = binaryReader.GetBuffer(true);
	fsb5Size = static_cast<unsigned int>(binaryReader.GetSize()) - 24;
}
