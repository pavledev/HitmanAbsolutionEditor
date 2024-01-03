#include "Glacier/Material/SRMaterialProperties.h"
#include "Glacier/Material/RENDER_PRIMITIVE_INSTANCE_FLAGS.h"
#include "Resources/Resource.h"

void SRMaterialProperties::SerializeToJson(const std::string& materialClassType, std::vector<std::shared_ptr<Resource>>& references, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer)
{
	writer.StartObject();

	writer.String("materialClassType");
	writer.String(materialClassType.c_str());

	writer.String("materialEffectResourceID");
	writer.String(references[lMaterialEffectIndex]->GetResourceID().c_str());

	MATERIAL_FLAGS materialFlags = static_cast<MATERIAL_FLAGS>(lMaterialClassFlags);

	writer.String("materialClassFlags");
	writer.StartObject();

	writer.String("MF_REFLECTION2D");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_REFLECTION2D) == MATERIAL_FLAGS::MF_REFLECTION2D);

	writer.String("MF_REFRACTION2D");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_REFRACTION2D) == MATERIAL_FLAGS::MF_REFRACTION2D);

	writer.String("MF_LIGHTING");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_LIGHTING) == MATERIAL_FLAGS::MF_LIGHTING);

	writer.String("MF_EMISSIVE");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_EMISSIVE) == MATERIAL_FLAGS::MF_EMISSIVE);

	writer.String("MF_DISCARD");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_DISCARD) == MATERIAL_FLAGS::MF_DISCARD);

	writer.String("MF_LM_SKIN");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_LM_SKIN) == MATERIAL_FLAGS::MF_LM_SKIN);

	writer.String("MF_PRIMCLASS_STANDARD");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD) == MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD);

	writer.String("MF_PRIMCLASS_LINKED");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_PRIMCLASS_LINKED) == MATERIAL_FLAGS::MF_PRIMCLASS_LINKED);

	writer.String("MF_PRIMCLASS_WEIGHTED");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED) == MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED);

	writer.String("MF_DOFOVERRIDE");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_DOFOVERRIDE) == MATERIAL_FLAGS::MF_DOFOVERRIDE);

	writer.String("MF_USES_DEFAULT_VS");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_USES_DEFAULT_VS) == MATERIAL_FLAGS::MF_USES_DEFAULT_VS);

	writer.String("MF_USES_SPRITE_SA_VS");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS) == MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS);

	writer.String("MF_USES_SPRITE_AO_VS");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS) == MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS);

	writer.String("MF_ALPHA");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_ALPHA) == MATERIAL_FLAGS::MF_ALPHA);

	writer.String("MF_USES_SIMPLE_SHADER");
	writer.Bool((materialFlags & MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER) == MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER);

	writer.EndObject();

	RENDER_PRIMITIVE_INSTANCE_FLAGS renderPrimitiveInstanceFlags = static_cast<RENDER_PRIMITIVE_INSTANCE_FLAGS>(lTransparencyFlags);

	writer.String("transparencyFlags");
	writer.StartObject();

	writer.String("TF_OPAQUE_EMISSIVE");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE);

	writer.String("TF_TRANS_EMISSIVE");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE);

	writer.String("TF_TRANSADD_EMISSIVE");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE);

	writer.String("TF_OPAQUE_LIT");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT);

	writer.String("TF_TRANS_LIT");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT);

	writer.String("TF_DECAL");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL);

	writer.String("TF_REFRACTIVE");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE);

	writer.String("TF_LM_SKIN");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN);

	writer.String("TF_FORCE_EMISSIVE");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE);

	writer.String("TF_DISABLE_SHADER_LOD");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD);

	writer.String("TF_DISCARD");
	writer.Bool((renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD);

	writer.EndObject();

	writer.String("materialDescriptor");
	writer.Uint(lMaterialDescriptor);

	writer.String("impactMaterial");
	writer.Uint(lImpactMaterial);

	writer.String("effectResource");
	writer.Uint(lEffectResource);

	writer.EndObject();
}
