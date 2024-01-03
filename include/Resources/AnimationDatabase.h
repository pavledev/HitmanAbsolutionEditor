#pragma once

#include "Resource.h"

class AnimationDatabase : public Resource
{
public:
	struct Entry
	{
		std::string animationName;
		std::string resourceID;
	};

	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	std::vector<Entry>& GetEntries();
	void SerializeToJson(const std::string& outputFilePath);

private:
	std::vector<Entry> entries;
};
