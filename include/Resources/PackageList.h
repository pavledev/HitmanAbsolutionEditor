#pragma once

#include "Glacier/PackageList/SPackageListData.h"

#include "Resource.h"

class PackageList : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void Parse(void* packageListData);
	std::shared_ptr<SPackageListData> GetPackageListData();

private:
	std::shared_ptr<SPackageListData> packageListData;
};
