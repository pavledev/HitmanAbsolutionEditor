#pragma once

#include <string>

#include "Glacier/Resource/SResourceHeaderHeader.h"
#include "Glacier/Resource/HeaderLibraryChunkFlags.h"

#include "Resource.h"
#include "IO/BinaryReader.h"

class HeaderLibrary
{
public:
	struct HeaderLibraryChunk
	{
		std::string filePath;
		SResourceHeaderHeader resourceHeaderHeader;
		unsigned int offset;
		HeaderLibraryChunkFlags flags;
		unsigned int localizationCategory;
		std::vector<std::string> languages;
		std::vector<int> states;
		std::vector<unsigned int> ridMappingIndices;
		std::vector<unsigned long long> ridMappingIDs;
	};

	const std::string& GetFilePath() const;
	const SResourceHeaderHeader& GetResourceHeaderHeader() const;
	const std::vector<Resource>& GetReferences() const;
	const std::vector<std::string>& GetChunkResourceIDs() const;
	const std::vector<std::string>& GetExternalResourceIDs() const;
	bool ParseHeaderLibrary(std::string& resourceID);
	void ParseReferencesChunk(const BinaryReader& binaryReader);
	HeaderLibraryChunk ParseHeaderLibraryChunk(const int index);

private:
	std::string filePath;
	SResourceHeaderHeader resourceHeaderHeader;
	std::vector<Resource> references;
	std::vector<std::string> chunkResourceIDs;
	std::vector<unsigned int> chunkOffsets;
	std::vector<std::string> externalResourceIDs;
};
