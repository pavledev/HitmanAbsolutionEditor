#include "Resources/ScatterData.h"

void ScatterData::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());

	scatterDataHeader = binaryReader.Read<SScatterDataHeader>();

	materialHeaders.reserve(scatterDataHeader.m_nMaterials);

	const std::vector<std::shared_ptr<Resource>> references = GetReferences();

	for (unsigned int i = 0; i < scatterDataHeader.m_nMaterials; ++i)
	{
		SScatterPackedMaterial materialHeader;

		materialHeader.m_idResource = references[i]->GetRuntimeResourceID();
		materialHeader.m_Base = binaryReader.Read<SScatterPackedMaterialBase>();

		materialHeaders.push_back(materialHeader);
	}

	instances.resize(scatterDataHeader.m_nMaterials);

	for (unsigned int i = 0; i < scatterDataHeader.m_nMaterials; ++i)
	{
		const unsigned int instanceCount = materialHeaders[i].m_Base.m_nInstances;

		instances[i].reserve(instanceCount);

		for (unsigned int j = 0; j < instanceCount; ++j)
		{
			instances[i].push_back(binaryReader.Read<SScatterPackedInstance>());
		}
	}

	isResourceDeserialized = true;
}

SScatterDataHeader& ScatterData::GetScatterDataHeader()
{
	return scatterDataHeader;
}

std::vector<SScatterPackedMaterial>& ScatterData::GetMaterialHeaders()
{
	return materialHeaders;
}

std::vector<std::vector<SScatterPackedInstance>>& ScatterData::GetInstances()
{
	return instances;
}
