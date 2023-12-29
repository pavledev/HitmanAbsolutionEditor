#include "Resources/WaveBankFX.h"
#include "Registry/TextListHashRegistry.h"
#include "Utility/StringUtility.h"

void WaveBankFX::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	faceFXAnimationReferences.reserve(entryCount);
	subtitleReferences.reserve(entryCount);

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		FaceFXAnimationReference faceFXAnimationReference;

		faceFXAnimationReference.animationSetResourceIndex = binaryReader.Read<unsigned int>();

		if (faceFXAnimationReference.animationSetResourceIndex != -1)
		{
			const unsigned int animationNameLength = binaryReader.Read<unsigned int>();
			faceFXAnimationReference.animationName = binaryReader.ReadString(static_cast<size_t>(animationNameLength) - 1);
		}

		faceFXAnimationReferences.push_back(faceFXAnimationReference);
	}

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		SubtitleReference subtitleReference;

		subtitleReference.textlistResourceIndex = binaryReader.Read<unsigned int>();

		if (subtitleReference.textlistResourceIndex != -1)
		{
			subtitleReference.hashedLoganID = binaryReader.Read<unsigned int>();

			const std::string name = TextListHashRegistry::GetInstance().GetName(subtitleReference.hashedLoganID);

			if (name.empty())
			{
				subtitleReference.textListEntryName = StringUtility::ConvertValueToHexString(subtitleReference.hashedLoganID);
			}
			else
			{
				subtitleReference.textListEntryName = name;
			}
		}

		subtitleReferences.push_back(subtitleReference);
	}

	isResourceDeserialized = true;
}

std::vector<WaveBankFX::FaceFXAnimationReference>& WaveBankFX::GetFaceFXAnimationReferences()
{
	return faceFXAnimationReferences;
}

std::vector<WaveBankFX::SubtitleReference>& WaveBankFX::GetSubtitleReferences()
{
	return subtitleReferences;
}
