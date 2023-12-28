#pragma once

#include "Resource.h"

class MultiLanguage : public Resource
{
public:
	void Deserialize() override;
	char GetLocalizationCategory();
	std::vector<int>& GetIndices();
	std::vector<std::string>& GetLocales();

private:
	char localizationCategory;
	std::vector<int> indices;
	std::vector<std::string> locales;
};
