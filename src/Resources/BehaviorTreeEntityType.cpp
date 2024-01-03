#include "Resources/BehaviorTreeEntityType.h"

void BehaviorTreeEntityType::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}
