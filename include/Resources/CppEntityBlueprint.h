#pragma once

#include <string>

#include "Glacier/Entity/SCppEntityBlueprint.h"

#include "IO/BinaryReader.h"

class CppEntityBlueprint
{
public:
	~CppEntityBlueprint();
	SCppEntityBlueprint* GetCppEntityBlueprint();
	bool Deserialize(const std::string& filePath);
	bool Deserialize(void* data, const unsigned int dataSize);
	bool Deserialize(BinaryReader& binaryReader);
	void SerializeToJson(const std::string& outputFilePath);

private:
	SCppEntityBlueprint* cppEntityBlueprint;
	unsigned char alignment;
};
