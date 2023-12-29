#include <IconsMaterialDesignIcons.h>

#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector4.h"

#include "UI/Panels/RenderMaterialInstancePanel.h"

RenderMaterialInstancePanel::RenderMaterialInstancePanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialInstance> renderMaterialInstance) : BasePanel(name, icon)
{
	this->renderMaterialInstance = renderMaterialInstance;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	tableColumns.push_back({ "" , 0, 0.2f });

	if (materialPropertyNames.empty())
	{
		materialPropertyNames.insert(std::make_pair('AREF', "Alpha Reference"));
		materialPropertyNames.insert(std::make_pair('ATST', "Alpha Test Enabled"));
		materialPropertyNames.insert(std::make_pair('BENA', "Blend Enabled"));
		materialPropertyNames.insert(std::make_pair('BIND', "Binder"));
		materialPropertyNames.insert(std::make_pair('BMOD', "Blend Mode"));
		materialPropertyNames.insert(std::make_pair('COLO', "Color"));
		materialPropertyNames.insert(std::make_pair('CULL', "Culling Mode"));
		materialPropertyNames.insert(std::make_pair('DBDE', "Decal Blend Diffuse"));
		materialPropertyNames.insert(std::make_pair('DBEE', "Decal Blend Emission"));
		materialPropertyNames.insert(std::make_pair('DBNE', "Decal Blend Normal"));
		materialPropertyNames.insert(std::make_pair('DBRE', "Decal Blend Roughness"));
		materialPropertyNames.insert(std::make_pair('DBSE', "Decal Blend Specular"));
		materialPropertyNames.insert(std::make_pair('ENAB', "Enabled"));
		materialPropertyNames.insert(std::make_pair('FENA', "Fog Enabled"));
		materialPropertyNames.insert(std::make_pair('FLTV', "Float Value"));
		materialPropertyNames.insert(std::make_pair('INST', "Instance"));
		materialPropertyNames.insert(std::make_pair('NAME', "Name"));
		materialPropertyNames.insert(std::make_pair('OPAC', "Opacity"));
		materialPropertyNames.insert(std::make_pair('RSTA', "Render State"));
		materialPropertyNames.insert(std::make_pair('SSBW', "Subsurface Value"));
		materialPropertyNames.insert(std::make_pair('SSVB', "Subsurface Blue"));
		materialPropertyNames.insert(std::make_pair('SSVG', "Subsurface Green"));
		materialPropertyNames.insert(std::make_pair('SSVR', "Subsurface Red"));
		materialPropertyNames.insert(std::make_pair('TAGS', "Tags"));
		materialPropertyNames.insert(std::make_pair('TEXT', "Texture"));
		materialPropertyNames.insert(std::make_pair('TILU', "Tiling U"));
		materialPropertyNames.insert(std::make_pair('TILV', "Tiling V"));
		materialPropertyNames.insert(std::make_pair('TXID', "Texture Id"));
		materialPropertyNames.insert(std::make_pair('TYPE', "Type"));
		materialPropertyNames.insert(std::make_pair('VALU', "Value"));
		materialPropertyNames.insert(std::make_pair('ZBIA', "Z Bias"));
		materialPropertyNames.insert(std::make_pair('ZOFF', "Z Offset"));
	}
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

void RenderMaterialInstancePanel::RenderProperties()
{
	if (!UI::BeginProperties("MaterialInstanceProperties", tableColumns))
	{
		return;
	}

	RenderMaterialInstance::Property& instanceProperty = renderMaterialInstance->GetInstanceProperty();

	RenderProperties(instanceProperty);

	UI::EndProperties();
}

void RenderMaterialInstancePanel::RenderProperties(RenderMaterialInstance::Property& property)
{
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
