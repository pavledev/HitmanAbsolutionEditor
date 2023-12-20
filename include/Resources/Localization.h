#pragma once

#include "Resource.h"

class Localization : public Resource
{
public:
	void Parse();
	char GetLocalizationCategory();
	std::vector<int>& GetIndices();
	std::vector<std::string>& GetLocales();

private:
	char localizationCategory;
	std::vector<int> indices;
	std::vector<std::string> locales;
};
