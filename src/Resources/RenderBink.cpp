#include "Resources/RenderBink.h"
#include "IO/BinaryWriter.h"

void RenderBink::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		BinaryWriter binaryWriter = BinaryWriter(outputPath);

		binaryWriter.Write(resourceData, resourceDataSize);
	}
}
