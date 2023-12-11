#pragma once

#include "SPackageListDataSection.h"

struct SPackageListDataFolder
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	ZString name;
	TArray<SPackageListDataSection> sections;
};
