#include "Glacier/GlobalResourceIndex/SResourceIndexFolder.h"

void SResourceIndexFolder::SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("name");
	name.SerializeToJson(writer);

	writer.String("resourceIndices");
	resourceIndices.SerializeToJson(writer);

	writer.EndObject();
}

void SResourceIndexFolder::SerializeToMemory(ZBinarySerializer& binarySerializer, const unsigned int offset)
{
	unsigned int nameOffset = offset + offsetof(SResourceIndexFolder, name);
	unsigned int resourceIndicesOffset = offset + offsetof(SResourceIndexFolder, resourceIndices);

	name.SerializeToMemory(binarySerializer, nameOffset);
	resourceIndices.SerializeToMemory(binarySerializer, resourceIndicesOffset);
}

SResourceIndexFolder* SResourceIndexFolder::DeserializeFromJson(const rapidjson::Value& object)
{
	SResourceIndexFolder* resourceIndexFolder = new SResourceIndexFolder();

	resourceIndexFolder->name = *ZString::DeserializeFromJson(object["name"].GetString());
	resourceIndexFolder->resourceIndices = *TArray<int>::DeserializeFromJson(object["resourceIndices"].GetArray());

	return resourceIndexFolder;
}
