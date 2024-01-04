#include "Resources/Localization.h"

void Localization::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	localizationCategory = binaryReader.Read<char>();
	const unsigned int count = (GetResourceDataSize() - 1) >> 3;
	const char* data = static_cast<const char*>(GetResourceData());

	for (unsigned int i = 0; i < count; ++i)
	{
		const int index = *reinterpret_cast<const int*>(&data[8 * i + 5]);
		const std::string locale = &data[8 * i + 1];

		indices.push_back(index);
		locales.push_back(locale);
	}

	isResourceDeserialized = true;
}

void Localization::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		SerializeToJson(outputPath);
	}
}

char Localization::GetLocalizationCategory()
{
	return localizationCategory;
}

std::vector<int>& Localization::GetIndices()
{
	return indices;
}

std::vector<std::string>& Localization::GetLocales()
{
	return locales;
}

void Localization::SerializeToJson(const std::string& outputFilePath)
{
	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	writer.StartObject();

	writer.String("localizationCategory");
	writer.Uint(localizationCategory);

	writer.String("locales");
	writer.StartArray();

	for (size_t i = 0; i < locales.size(); ++i)
	{
		writer.String(locales[i].c_str());
	}

	writer.EndArray();

	writer.String("indices");
	writer.StartArray();

	for (size_t i = 0; i < indices.size(); ++i)
	{
		writer.Int(indices[i]);
	}

	writer.EndArray();

	writer.EndObject();

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}
