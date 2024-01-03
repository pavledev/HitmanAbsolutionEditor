#pragma once

#include <string>

#include "Glacier/Resource/EResourceReferenceFlags.h"
#include "Glacier/Resource/HeaderLibraryChunkFlags.h"

#include "StringUtility.h"
#include "Hash.h"
#include "../Resources/Resource.h"

class ResourceUtility
{
public:
	static std::string ConvertResourceIDToFilePath(const std::string& resourceID);
	static std::string ConvertResourceReferenceFlagsToString(const EResourceReferenceFlags resourceReferenceFlags);
	static std::string ConvertHeaderLibraryChunkFlagsToString(const HeaderLibraryChunkFlags headerLibraryChunkFlags);
	static std::string GetResourceName(const std::string& resourceID);
	static std::string ConvertResourceTypeToString(const unsigned int type);

	static unsigned int CalculateArrayElementsCount(const unsigned int elementsStartOffset, const unsigned int elementsEndOffset, const unsigned int elementSize)
	{
		if (elementsEndOffset - elementsStartOffset == 0)
		{
			return 0;
		}

		return (elementsEndOffset - elementsStartOffset) / elementSize;
	}

	template <typename T>
	static T* Convert4ByteAddressTo8BytePointer(const void* buffer, const unsigned int address)
	{
		unsigned int high = reinterpret_cast<unsigned long long>(buffer) >> 32;

		return reinterpret_cast<T*>((static_cast<unsigned long long>(high) << 32) | address);
	}

	static std::shared_ptr<Resource> CreateResource(const std::string& type);
	static void LoadResource(std::shared_ptr<Resource> resource);

	static std::string GenerateFileName(std::shared_ptr<Resource> resource);
};
