#pragma once

#include "SPackageListDataFolder.h"

struct SPackageListData
{
	void SerializeToJson(const std::string& outputFilePath);

	TArray<SPackageListDataFolder> folders;
};
