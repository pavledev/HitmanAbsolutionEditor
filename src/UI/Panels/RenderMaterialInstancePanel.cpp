#include <IconsMaterialDesignIcons.h>

#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector4.h"
#include "Glacier/Material/RENDER_PRIMITIVE_INSTANCE_FLAGS.h"

#include "UI/Panels/RenderMaterialInstancePanel.h"

RenderMaterialInstancePanel::RenderMaterialInstancePanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialInstance> renderMaterialInstance) : BasePanel(name, icon)
{
	this->renderMaterialInstance = renderMaterialInstance;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	tableColumns.push_back({ "" , 0, 0.2f });
}

void RenderMaterialInstancePanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!renderMaterialInstance)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading material instance...");
		End();

		return;
	}

	if (!renderMaterialInstance->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading material instance...");
		End();

		return;
	}

	RenderProperties();

	End();
}

void RenderMaterialInstancePanel::OnResourceLoaded()
{
	renderMaterialInstance->Deserialize();
}

void RenderMaterialInstancePanel::SetResource(std::shared_ptr<RenderMaterialInstance> renderMaterialInstance)
{
	this->renderMaterialInstance = renderMaterialInstance;
}

void RenderMaterialInstancePanel::RenderMaterialInfo()
{
	const bool isMaterialInfoNodeOpen = UI::BeginTreeNodeProperty("materialInfo", nullptr);

	if (isMaterialInfoNodeOpen)
	{
		std::vector<std::shared_ptr<Resource>>& matiReferences = renderMaterialInstance->GetReferences();
		SRMaterialPropertyList& materialPropertyList = renderMaterialInstance->GetMaterialPropertyList();
		SRMaterialPropertyList::MATERIAL_FLAGS materialFlags = static_cast<SRMaterialPropertyList::MATERIAL_FLAGS>(materialPropertyList.lMaterialClassFlags);
		RENDER_PRIMITIVE_INSTANCE_FLAGS renderPrimitiveInstanceFlags = static_cast<RENDER_PRIMITIVE_INSTANCE_FLAGS>(materialPropertyList.lTransparencyFlags);
		std::string materialEffectResourceID = matiReferences[materialPropertyList.lMaterialEffectIndex]->GetResourceID();

		UI::StringProperty("materialClassType", renderMaterialInstance->GetMaterialClassType());
		UI::StringProperty("materialEffectResourceID", materialEffectResourceID);

		const bool isMaterialClassFlagsTreeNodeOpen = UI::BeginTreeNodeProperty("materialClassFlags", nullptr);

		if (isMaterialClassFlagsTreeNodeOpen)
		{
			static bool isReflection2DChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_REFLECTION2D) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_REFLECTION2D;
			static bool isRefraction2DChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_REFRACTION2D) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_REFRACTION2D;
			static bool isLightingChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_LIGHTING) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_LIGHTING;
			static bool isEmissiveChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_EMISSIVE) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_EMISSIVE;
			static bool isDiscardChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_DISCARD) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_DISCARD;
			static bool isSkinChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_LM_SKIN) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_LM_SKIN;
			static bool isPrimClassStandardChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD;
			static bool isPrimClassLinkedChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_PRIMCLASS_LINKED) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_PRIMCLASS_LINKED;
			static bool isPrimClassWeightedChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED;
			static bool isDofOverrideChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_DOFOVERRIDE) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_DOFOVERRIDE;
			static bool isUsesDefaltVSChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_DEFAULT_VS) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_DEFAULT_VS;
			static bool isUsesSpriteSAVSChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS;
			static bool isUsesSpriteAOVSChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS;
			static bool isAlphaChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_ALPHA) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_ALPHA;
			static bool isUsesSimpleShaderChecked = (materialFlags & SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER) == SRMaterialPropertyList::MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER;

			if (ImGui::Checkbox("MF_REFLECTION2D", &isReflection2DChecked))
			{
				if (isReflection2DChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_REFLECTION2D);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_REFLECTION2D);
				}
			}

			if (ImGui::Checkbox("MF_REFRACTION2D", &isRefraction2DChecked))
			{
				if (isRefraction2DChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_REFRACTION2D);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_REFRACTION2D);
				}
			}

			if (ImGui::Checkbox("MF_LIGHTING", &isLightingChecked))
			{
				if (isLightingChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_LIGHTING);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_LIGHTING);
				}
			}

			if (ImGui::Checkbox("MF_EMISSIVE", &isEmissiveChecked))
			{
				if (isEmissiveChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_EMISSIVE);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_EMISSIVE);
				}
			}

			if (ImGui::Checkbox("MF_DISCARD", &isDiscardChecked))
			{
				if (isDiscardChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_DISCARD);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_DISCARD);
				}
			}

			if (ImGui::Checkbox("MF_LM_SKIN", &isSkinChecked))
			{
				if (isSkinChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_LM_SKIN);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_LM_SKIN);
				}
			}

			if (ImGui::Checkbox("MF_PRIMCLASS_STANDARD", &isPrimClassStandardChecked))
			{
				if (isPrimClassStandardChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_PRIMCLASS_STANDARD);
				}
			}

			if (ImGui::Checkbox("MF_PRIMCLASS_LINKED", &isPrimClassLinkedChecked))
			{
				if (isPrimClassLinkedChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_PRIMCLASS_LINKED);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_PRIMCLASS_LINKED);
				}
			}

			if (ImGui::Checkbox("MF_PRIMCLASS_WEIGHTED", &isPrimClassWeightedChecked))
			{
				if (isPrimClassWeightedChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_PRIMCLASS_WEIGHTED);
				}
			}

			if (ImGui::Checkbox("MF_DOFOVERRIDE", &isDofOverrideChecked))
			{
				if (isDofOverrideChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_DOFOVERRIDE);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_DOFOVERRIDE);
				}
			}

			if (ImGui::Checkbox("MF_USES_DEFAULT_VS", &isUsesDefaltVSChecked))
			{
				if (isUsesDefaltVSChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_DEFAULT_VS);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_DEFAULT_VS);
				}
			}

			if (ImGui::Checkbox("MF_USES_SPRITE_SA_VS", &isUsesSpriteSAVSChecked))
			{
				if (isUsesSpriteSAVSChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_SPRITE_SA_VS);
				}
			}

			if (ImGui::Checkbox("MF_USES_SPRITE_AO_VS", &isUsesSpriteAOVSChecked))
			{
				if (isUsesSpriteAOVSChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_SPRITE_AO_VS);
				}
			}

			if (ImGui::Checkbox("MF_ALPHA", &isAlphaChecked))
			{
				if (isAlphaChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_ALPHA);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_ALPHA);
				}
			}

			if (ImGui::Checkbox("MF_USES_SIMPLE_SHADER", &isUsesSimpleShaderChecked))
			{
				if (isUsesSimpleShaderChecked)
				{
					materialPropertyList.lMaterialClassFlags |= static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER);
				}
				else
				{
					materialPropertyList.lMaterialClassFlags &= ~static_cast<unsigned int>(SRMaterialProperties::MATERIAL_FLAGS::MF_USES_SIMPLE_SHADER);
				}
			}
		}

		UI::EndTreeNodeProperty(isMaterialClassFlagsTreeNodeOpen);

		const bool isTransparencyFlagsTreeNodeOpen = UI::BeginTreeNodeProperty("transparencyFlags", nullptr);

		if (isTransparencyFlagsTreeNodeOpen)
		{
			static bool isOpaqueEmissiveChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE;
			static bool isTransEmissiveChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE;
			static bool isTransAddEmissiveChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE;
			static bool isOpaqueLitChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT;
			static bool isTransLitChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT;
			static bool isDecalChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL;
			static bool isRefractiveChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE;
			static bool isLMSkinChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN;
			static bool isForceEmissiveChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE;
			static bool isDisableShaderLODChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD;
			static bool isDiscardChecked = (renderPrimitiveInstanceFlags & RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD) == RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD;

			if (ImGui::Checkbox("TF_OPAQUE_EMISSIVE", &isOpaqueEmissiveChecked))
			{
				if (isOpaqueEmissiveChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_EMISSIVE);
				}
			}

			if (ImGui::Checkbox("TF_TRANS_EMISSIVE", &isTransEmissiveChecked))
			{
				if (isTransEmissiveChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_EMISSIVE);
				}
			}

			if (ImGui::Checkbox("TF_TRANSADD_EMISSIVE", &isTransAddEmissiveChecked))
			{
				if (isTransAddEmissiveChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANSADD_EMISSIVE);
				}
			}

			if (ImGui::Checkbox("TF_OPAQUE_LIT", &isOpaqueLitChecked))
			{
				if (isOpaqueLitChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_OPAQUE_LIT);
				}
			}

			if (ImGui::Checkbox("TF_TRANS_LIT", &isTransLitChecked))
			{
				if (isTransLitChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_TRANS_LIT);
				}
			}

			if (ImGui::Checkbox("TF_DECAL", &isDecalChecked))
			{
				if (isDecalChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DECAL);
				}
			}

			if (ImGui::Checkbox("TF_REFRACTIVE", &isRefractiveChecked))
			{
				if (isRefractiveChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_REFRACTIVE);
				}
			}

			if (ImGui::Checkbox("TF_LM_SKIN", &isLMSkinChecked))
			{
				if (isLMSkinChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_LM_SKIN);
				}
			}

			if (ImGui::Checkbox("TF_FORCE_EMISSIVE", &isForceEmissiveChecked))
			{
				if (isForceEmissiveChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_FORCE_EMISSIVE);
				}
			}

			if (ImGui::Checkbox("TF_DISABLE_SHADER_LOD", &isDisableShaderLODChecked))
			{
				if (isDisableShaderLODChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISABLE_SHADER_LOD);
				}
			}

			if (ImGui::Checkbox("TF_DISCARD", &isDiscardChecked))
			{
				if (isDiscardChecked)
				{
					materialPropertyList.lTransparencyFlags |= static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD);
				}
				else
				{
					materialPropertyList.lTransparencyFlags &= ~static_cast<unsigned int>(RENDER_PRIMITIVE_INSTANCE_FLAGS::TF_DISCARD);
				}
			}
		}

		UI::EndTreeNodeProperty(isTransparencyFlagsTreeNodeOpen);
	}

	UI::EndTreeNodeProperty(isMaterialInfoNodeOpen);
}

void RenderMaterialInstancePanel::RenderProperties()
{
	if (!UI::BeginProperties("MaterialInstanceProperties", tableColumns))
	{
		return;
	}

	RenderMaterialInfo();

	RenderMaterialInstance::Property& instanceProperty = renderMaterialInstance->GetInstanceProperty();

	RenderProperties(instanceProperty);

	UI::EndProperties();
}

void RenderMaterialInstancePanel::RenderProperties(RenderMaterialInstance::Property& property)
{
	std::unordered_map<unsigned int, std::string>& materialPropertyNames = renderMaterialInstance->GetMaterialPropertyNames();
	const std::string& propertyName = materialPropertyNames[property.propertyInfo.lName];
	const PROPERTY_TYPE propertyType = static_cast<PROPERTY_TYPE>(property.propertyInfo.lType);

	switch (propertyType)
	{
		case PROPERTY_TYPE::PT_FLOAT:
		{
			Property(propertyName.c_str(), property.floatValue);
			break;
		}
		case PROPERTY_TYPE::PT_CHAR:
		{
			UI::StringProperty(propertyName.c_str(), property.stringValue);
			break;
		}
		case PROPERTY_TYPE::PT_UINT32:
		{
			if (property.propertyInfo.lName == 'TXID')
			{
				std::vector<std::shared_ptr<Resource>>& matiReferences = renderMaterialInstance->GetReferences();
				std::string textureResourceID;

				if (property.uint32Value != -1)
				{
					textureResourceID = matiReferences[property.uint32Value]->GetResourceID();
				}

				UI::StringProperty(propertyName.c_str(), textureResourceID);
			}
			else
			{
				Property(propertyName.c_str(), property.uint32Value);
			}

			break;
		}
		case PROPERTY_TYPE::PT_LIST:
		{
			const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName.c_str(), nullptr);

			if (isTreeNodeOpen)
			{
				for (size_t i = 0; i < property.childProperties.size(); ++i)
				{
					RenderProperties(property.childProperties[i]);
				}
			}

			UI::EndTreeNodeProperty(isTreeNodeOpen);

			break;
		}
	}
}

void RenderMaterialInstancePanel::RenderRemoveModifierButton(const unsigned int modifierIndex)
{
	ImGui::TableNextColumn();

	const float checkboxWidth = ImGui::GetFrameHeight();
	const ImVec2 buttonSize = { checkboxWidth , 0.f };
	const float columnWidth = ImGui::GetColumnWidth();
	const float paddingSide = (columnWidth - buttonSize.x) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingSide);

	const std::string buttonID = UI::GetPropertyID("Button");

	ImGui::PushID(buttonID.c_str());

	if (ImGui::Button(ICON_MDI_CLOSE, buttonSize))
	{
	}

	ImGui::PopID();
}
