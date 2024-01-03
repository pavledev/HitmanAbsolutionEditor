#pragma once

#include "Resource.h"

class TextList : public Resource
{
public:
	struct Entry
	{
		unsigned int key;
		std::string name;
		std::string text;
	};

	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	std::vector<Entry>& GetEntries();
	void SerializeToJson(const std::string& outputFilePath);

private:
	std::vector<Entry> entries;
};
