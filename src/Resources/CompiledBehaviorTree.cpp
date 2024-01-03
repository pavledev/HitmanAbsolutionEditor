#include "Resources/CompiledBehaviorTree.h"

void CompiledBehaviorTree::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
}
