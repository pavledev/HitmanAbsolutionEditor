#include "Resources/TextList.h"
#include "Utility/StringUtility.h"
#include "Registry/TextListHashRegistry.h"

void TextList::Parse()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int entryCount = binaryReader.Read<unsigned int>();

	for (unsigned int i = 0; i < entryCount; ++i)
	{
		Entry entry;

		const unsigned int key = binaryReader.Read<unsigned int>();
		const unsigned int textLength = binaryReader.Read<unsigned int>();
		const std::string text = binaryReader.ReadString(static_cast<size_t>(textLength));
		const std::string name = TextListHashRegistry::GetInstance().GetName(key);

		entry.key = key;
		entry.text = text;

		if (name.empty())
		{
			entry.name = StringUtility::ConvertValueToHexString(key);
		}
		else
		{
			entry.name = name;
		}

		entries.push_back(entry);
	}
}

std::vector<TextList::Entry>& TextList::GetEntries()
{
	return entries;
}
