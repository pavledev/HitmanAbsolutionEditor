#pragma once

#include "../Containers/TArray.h"

struct SPackageListDataSection
{
	void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

	TArray<ZString> sceneResourceIDs;
	TArray<ZString> resources;
};
