#pragma once

#include "Glacier/PackageList/SPackageListData.h"

#include "Resource.h"

class PackageList : public Resource
{
public:
	void Deserialize() override;
	void Parse(void* packageListData);
	std::shared_ptr<SPackageListData> GetPackageListData();
	void SerializeToJson();

private:
	std::shared_ptr<SPackageListData> packageListData;
};
