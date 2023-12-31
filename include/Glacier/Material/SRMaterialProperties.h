#pragma once

#include <string>
#include <vector>
#include <memory>

#include "rapidjson/document.h"

class Resource;

struct SRMaterialProperties
{
	enum class MATERIAL_FLAGS
	{
		MF_REFLECTION2D = 1,
		MF_REFRACTION2D = 2,
		MF_LIGHTING = 4,
		MF_EMISSIVE = 8,
		MF_DISCARD = 16,
		MF_LM_SKIN = 32,
		MF_PRIMCLASS_STANDARD = 64,
		MF_PRIMCLASS_LINKED = 128,
		MF_PRIMCLASS_WEIGHTED = 256,
		MF_DOFOVERRIDE = 512,
		MF_USES_DEFAULT_VS = 1024,
		MF_USES_SPRITE_SA_VS = 2048,
		MF_USES_SPRITE_AO_VS = 4096,
		MF_ALPHA = 8192,
		MF_USES_SIMPLE_SHADER = 16384
	};

	void SerializeToJson(const std::string& materialClassType, std::vector<std::shared_ptr<Resource>>& references, rapidjson::Document& document);

	unsigned int lMaterialClassType;
	unsigned int lMaterialEffectIndex;
	unsigned int lMaterialClassFlags;
	unsigned int lTransparencyFlags;
	unsigned int lMaterialDescriptor;
	unsigned int lImpactMaterial;
	unsigned int lEffectResource;
};

inline SRMaterialProperties::MATERIAL_FLAGS operator&(SRMaterialProperties::MATERIAL_FLAGS lhs, SRMaterialProperties::MATERIAL_FLAGS rhs)
{
	return static_cast<SRMaterialProperties::MATERIAL_FLAGS>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
