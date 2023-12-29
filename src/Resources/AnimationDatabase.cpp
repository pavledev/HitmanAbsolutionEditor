#include "Resources/AnimationDatabase.h"

void AnimationDatabase::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	entries.reserve(entryCount);

	const std::vector<std::shared_ptr<Resource>>& references = GetReferences();

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		Entry entry;

		const unsigned int referenceIndex = binaryReader.Read<unsigned int>();
		entry.resourceID = references[referenceIndex]->GetResourceID();

		entries.push_back(entry);
	}

	const unsigned int animationNamesOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(animationNamesOffset - 4, SeekOrigin::Current);

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		entries[i].animationName = binaryReader.ReadString();
	}

	isResourceDeserialized = true;
}

std::vector<AnimationDatabase::Entry>& AnimationDatabase::GetEntries()
{
	return entries;
}
