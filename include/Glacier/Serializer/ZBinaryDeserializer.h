#pragma once

#include "BinaryBulkDataHeader.h"
#include "../TypeInfo/STypeID.h"

#include "IO/BinaryReader.h"
#include "IO/BinaryWriter.h"
#include "Resources/Resource.h"

class ZBinaryDeserializer
{
public:
	ZBinaryDeserializer();
	void* Deserialize(const std::string& filePath, const std::vector<std::shared_ptr<Resource>>* references = nullptr);
	void* Deserialize(void* buffer, const unsigned int size, const std::vector<std::shared_ptr<Resource>>* references = nullptr);
	void* Deserialize(BinaryReader& binaryReader, const std::vector<std::shared_ptr<Resource>>* references = nullptr);
	const unsigned char GetAlignment() const;

private:
	void ParseRebaseSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter);
	void ParseTypeReindexingSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter);
	void ParseRuntimeResourceIDReindexingSection(BinaryReader& binaryReader, BinaryReader& dataSectionbinaryReader, BinaryWriter& dataSectionBinaryWriter, const std::vector<std::shared_ptr<Resource>>* references = nullptr);
	static void Align(BinaryReader& binaryReader, const unsigned int currentPosition, const unsigned char alignment);
	STypeID* GetTypeIDFromTypeName(const std::string& typeName);

	unsigned char alignment;
};
