#pragma once

#include "Foundation/NxSimpleTypes.h"
#include "Foundation/NxStream.h"

#include "IO/BinaryReader.h"
#include "IO/BinaryWriter.h"

class PhysXStream : public NxStream
{
public:
	PhysXStream(BinaryReader* binaryReader);
	unsigned char readByte() const override;
	unsigned short readWord() const override;
	unsigned int readDword() const override;
	float readFloat() const override;
	double readDouble() const override;
	void readBuffer(void* buffer, NxU32 size) const override;
	NxStream& storeByte(NxU8 b) override;
	NxStream& storeWord(NxU16 w) override;
	NxStream& storeDword(NxU32 d) override;
	NxStream& storeFloat(NxF32 f) override;
	NxStream& storeDouble(NxF64 f) override;
	NxStream& storeBuffer(const void* buffer, NxU32 size) override;

private:
	BinaryReader* binaryReader;
	BinaryWriter binaryWriter;
};
