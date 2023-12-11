#include "Resources/AspectEntityType.h"

void AspectEntityType::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	while (binaryReader.GetPosition() < binaryReader.GetSize())
	{
		referenceIndices.push_back(binaryReader.Read<unsigned int>());
	}
}
