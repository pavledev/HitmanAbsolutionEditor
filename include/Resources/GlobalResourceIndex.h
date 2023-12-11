#pragma once

#include <string>

#include "Glacier/GlobalResourceIndex/SResourceIndex.h"

#include "IO/BinaryReader.h"

class GlobalResourceIndex
{
public:
	~GlobalResourceIndex();
	SResourceIndex* GetResourceIndex();
	bool Deserialize(const std::string& filePath);
	bool Deserialize(void* data, const unsigned int dataSize);
	bool Deserialize(BinaryReader& binaryReader);
	void SerializeToJson(const std::string& outputFilePath);

private:
	SResourceIndex* resourceIndex;
	unsigned char alignment;
};
