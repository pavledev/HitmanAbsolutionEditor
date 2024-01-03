#pragma once

#include "Resource.h"

class Localization : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	char GetLocalizationCategory();
	std::vector<int>& GetIndices();
	std::vector<std::string>& GetLocales();
	void SerializeToJson(const std::string& outputFilePath);

private:
	char localizationCategory;
	std::vector<int> indices;
	std::vector<std::string> locales;
};
