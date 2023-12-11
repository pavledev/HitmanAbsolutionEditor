#pragma once

#include "SPackageListDataFolder.h"

struct SPackageListData
{
	std::string SerializeToJson();

	TArray<SPackageListDataFolder> folders;
};
