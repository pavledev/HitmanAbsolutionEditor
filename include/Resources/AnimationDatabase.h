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
	std::vector<Entry>& GetEntries();

private:
	std::vector<Entry> entries;
};
