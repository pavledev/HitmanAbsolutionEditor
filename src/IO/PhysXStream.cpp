#include "IO/PhysXStream.h"

PhysXStream::PhysXStream(BinaryReader* binaryReader)
{
	this->binaryReader = binaryReader;
}

unsigned char PhysXStream::readByte() const
{
	return binaryReader->Read<unsigned char>();
}

unsigned short PhysXStream::readWord() const
{
	return binaryReader->Read<unsigned short>();
}

unsigned int PhysXStream::readDword() const
{
	return binaryReader->Read<unsigned int>();
}

float PhysXStream::readFloat() const
{
	return binaryReader->Read<float>();
}

double PhysXStream::readDouble() const
{
	return binaryReader->Read<double>();
}

void PhysXStream::readBuffer(void* buffer, NxU32 size) const
{
	return binaryReader->Read<void>(buffer, size);
}

NxStream& PhysXStream::storeByte(NxU8 b)
{
	binaryWriter.Write(b);

	return *this;
}

NxStream& PhysXStream::storeWord(NxU16 w)
{
	binaryWriter.Write(w);

	return *this;
}

NxStream& PhysXStream::storeDword(NxU32 d)
{
	binaryWriter.Write(d);

	return *this;
}

NxStream& PhysXStream::storeFloat(NxF32 f)
{
	binaryWriter.Write(f);

	return *this;
}

NxStream& PhysXStream::storeDouble(NxF64 f)
{
	binaryWriter.Write(f);

	return *this;
}

NxStream& PhysXStream::storeBuffer(const void* buffer, NxU32 size)
{
	binaryWriter.Write(buffer, size);

	return *this;
}
