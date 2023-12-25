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

	void Deserialize();
	std::vector<Entry>& GetEntries();

private:
	std::vector<Entry> entries;
};
