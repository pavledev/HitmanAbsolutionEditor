#include "Glacier/Material/SRMaterialProperties.h"
#include "Glacier/Material/RENDER_PRIMITIVE_INSTANCE_FLAGS.h"
#include "Resources/Resource.h"

void SRMaterialProperties::SerializeToJson(const std::string& materialClassType, std::vector<std::shared_ptr<Resource>>& references, rapidjson::Document& document)
{
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();
	rapidjson::Value materialInfoObject;

	materialInfoObject.SetObject();

	materialInfoObject.AddMember("materialClassType", rapidjson::Value(materialClassType.c_str(), allocator).Move(), allocator);
	materialInfoObject.AddMember("materialEffectResourceID", rapidjson::Value(references[lMaterialEffectIndex]->GetResourceID().c_str(), allocator).Move(), allocator);

	const MATERIAL_FLAGS materialFlags = static_cast<MATERIAL_FLAGS>(lMaterialClassFlags);
	rapidjson::Value materialClassFlagsObject;

	materialClassFlagsObject.SetObject();

	materialClassFlagsObject.AddMember("MF_REFLECTION2D", (materialFlags & MATERIAL_FLAGS::MF_REFLECTION2D) == MATERIAL_FLAGS::MF_REFLECTION2D, allocator);
	materialClassFlagsObject.AddMember("MF_REFRACTION2D", (materialFlags & MATERIAL_FLAGS::MF_REFRACTION2D) == MATERIAL_FLAGS::MF_REFRACTION2D, allocator);
	materialClassFlagsObject.AddMember("MF_LIGHTING", (materialFlags & MATERIAL_FLAGS::MF_LIGHTING) == MATERIAL_FLAGS::MF_LIGHTING, allocator);
	materialClassFlagsObject.AddMember("MF_EMISSIVE", (materialFlags & MATERIAL_FLAGS::MF_EMISSIVE) == MATERIAL_FLAGS::MF_EMISSIVE, allocator);
	materialClassFlagsObject.AddMember("MF_DISCARD", (materialFlags & MATERIAL_FLAGS::MF_DISCARD) == MATERIAL_FLAGS::MF_DISCARD, allocator);
	materialClassFlagsObject.AddMember("MF_LM_SKIN", (materialFlags & MATERIAL_FLAGS::MF_LM_SKIN) == MATERIAL_FLAGS::MF_LM_SKIN, allocator);
	materialClassFlagsObject.AddMember("MF_PRIMCLASS_STANDARD", (materialFlags & MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD) == MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD, allocator);
	materialClassFlagsObject.AddMember("MF_PRIMCLASS_LINKED", (materialFlags & MATERIAL_FLAGS::MF_PRIMCLASS_LINKED) == MATERIAL_FLAGS::MF_PRIMCLASS_LINKED, allocator);
	materialClassFlagsObject.AddMember("MF_PRIMCLASS_WEIGHTED", (materialFlags & MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED) == MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED, allocator);
	materialClassFlagsObject.AddMember("MF_DOFOVERRIDE", (materialFlags & MATERIAL_FLAGS::MF_DOFOVERRIDE) == MATERIAL_FLAGS::MF_DOFOVERRIDE, allocator);
	materialClassFlagsObject.AddMember("MF_USES_DEFAULT_VS", (materialFlags & MATERIAL_FLAGS::MF_USES_DEFAULT_VS) == MATERIAL_FLAGS::MF_USES_DEFAULT_VS, allocator);
	materialClassFlagsObject.AddMember("MF_USES_SPRITE_SA_VS", (materialFlags & MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS) == MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS, allocator);
	materialClassFlagsObject.AddMember("MF_USES_SPRITE_AO_VS", (materialFlags & MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS) == MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS, allocator);
	materialClassFlagsObject.AddMember("MF_ALPHA", (materialFlags & MATERIAL_FLAGS::MF_ALPHA) == MATERIAL_FLAGS::MF_ALPHA, allocator);
	materialClassFlagsObject.AddMember("MF_USES_SIMPLE_SHADER", (materialFlags & MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER) == MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER, allocator);
	materialInfoObject.AddMember("materialClassFlags", materialClassFlagsObject, allocator);

	const RENDER_PRIMITIVE_INSTANCE_FLAGS renderPrimitiveInstanceFlags = static_cast<RENDER_PRIMITIVE_INSTANCE_FLAGS>(lTransparencyFlags);
	rapidjson::Value transparencyFlagsObject;

	transparencyFlagsObject.SetObject();

	transparencyFlagsObject.AddMember("TF_OPAQUE_EMISSIVE", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE, allocator);
	transparencyFlagsObject.AddMember("TF_TRANS_EMISSIVE", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE, allocator);
	transparencyFlagsObject.AddMember("TF_TRANSADD_EMISSIVE", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE, allocator);
	transparencyFlagsObject.AddMember("TF_OPAQUE_LIT", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT, allocator);
	transparencyFlagsObject.AddMember("TF_TRANS_LIT", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT, allocator);
	transparencyFlagsObject.AddMember("TF_DECAL", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL, allocator);
	transparencyFlagsObject.AddMember("TF_REFRACTIVE", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE, allocator);
	transparencyFlagsObject.AddMember("TF_LM_SKIN", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN, allocator);
	transparencyFlagsObject.AddMember("TF_FORCE_EMISSIVE", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE, allocator);
	transparencyFlagsObject.AddMember("TF_DISABLE_SHADER_LOD", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD, allocator);
	transparencyFlagsObject.AddMember("TF_DISCARD", (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD, allocator);
	materialInfoObject.AddMember("transparencyFlags", transparencyFlagsObject, allocator);

	materialInfoObject.AddMember("materialDescriptor", lMaterialDescriptor, allocator);
	materialInfoObject.AddMember("impactMaterial", lImpactMaterial, allocator);
	materialInfoObject.AddMember("effectResource", lEffectResource, allocator);

	document.AddMember("materialInfo", materialInfoObject, allocator);
}
