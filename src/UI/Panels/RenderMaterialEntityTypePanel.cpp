#include <IconsMaterialDesignIcons.h>

#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector4.h"

#include "UI/Panels/RenderMaterialEntityTypePanel.h"
#include "Registry/ResourceInfoRegistry.h"

RenderMaterialEntityTypePanel::RenderMaterialEntityTypePanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialEntityType> renderMaterialEntityTypeResource) : BasePanel(name, icon)
{
	this->renderMaterialEntityTypeResource = renderMaterialEntityTypeResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	tableColumns.push_back({ "" , 0, 0.2f });
}

void RenderMaterialEntityTypePanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!renderMaterialEntityTypeResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading material entity type...");
		End();

		return;
	}

	if (!renderMaterialEntityBlueprint || renderMaterialEntityBlueprint && !renderMaterialEntityBlueprint->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading material entity blueprint...");
		End();

		return;
	}

	RenderModifiers();

	End();
}

void RenderMaterialEntityTypePanel::OnResourceLoaded()
{
	renderMaterialEntityTypeResource->Deserialize();

	std::vector<std::shared_ptr<Resource>>& mattReferences = renderMaterialEntityTypeResource->GetReferences();

	for (size_t i = 0; i < mattReferences.size(); ++i)
	{
		const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(mattReferences[i]->GetHash());

		if (referenceInfo.type == "MATB")
		{
			renderMaterialEntityBlueprint = std::static_pointer_cast<RenderMaterialEntityBlueprint>(mattReferences[i]);

			break;
		}
	}

	const ResourceInfoRegistry::ResourceInfo& matbResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(renderMaterialEntityBlueprint->GetHash());

	renderMaterialEntityBlueprint->LoadResource(0, matbResourceInfo.headerLibraries[0].chunkIndex, matbResourceInfo.headerLibraries[0].indexInLibrary, false, false, true);
	renderMaterialEntityBlueprint->Deserialize();
	renderMaterialEntityBlueprint->DeleteResourceData();
}

void RenderMaterialEntityTypePanel::RenderModifiers()
{
	if (!UI::BeginProperties("MaterialEntityTypeModifiers", tableColumns))
	{
		return;
	}

	std::vector<std::shared_ptr<RenderMaterialEntityType::Modifier>>& mattModifiers = renderMaterialEntityTypeResource->GetModifiers();
	std::vector<std::shared_ptr<RenderMaterialEntityBlueprint::Modifier>>& matbModifiers = renderMaterialEntityBlueprint->GetModifiers();

	for (size_t i = 0; i < mattModifiers.size(); ++i)
	{
		switch (mattModifiers[i]->type)
		{
			case RenderMaterialEntityType::ModifierType::UInt:
				Property(matbModifiers[i]->name, *static_cast<unsigned int*>(mattModifiers[i]->value), i);
				break;
			case RenderMaterialEntityType::ModifierType::SColorRGB:
				SColorRGBProperty(matbModifiers[i]->name, mattModifiers[i]->value, i);
				break;
			case RenderMaterialEntityType::ModifierType::SColorRGBA:
				SColorRGBAProperty(matbModifiers[i]->name, mattModifiers[i]->value, i);
				break;
			case RenderMaterialEntityType::ModifierType::Float:
				Property(matbModifiers[i]->name, *static_cast<float*>(mattModifiers[i]->value), i);
				break;
			case RenderMaterialEntityType::ModifierType::SVector2:
				SVector2Property(matbModifiers[i]->name, mattModifiers[i]->value, i);
				break;
			case RenderMaterialEntityType::ModifierType::SVector3:
				SVector3Property(matbModifiers[i]->name, mattModifiers[i]->value, i);
				break;
			case RenderMaterialEntityType::ModifierType::SVector4:
				SVector4Property(matbModifiers[i]->name, mattModifiers[i]->value, i);
				break;
		}
	}

	UI::EndProperties();
}

void RenderMaterialEntityTypePanel::SColorRGBProperty(std::string& name, void* value, const unsigned int index)
{
	SColorRGB* colorRGB = static_cast<SColorRGB*>(value);

	UI::BeginProperty(name, nullptr);

	ImGui::ColorEdit3(UI::GetPropertyID(name.c_str()).c_str(), &colorRGB->r);

	UI::EndProperty();

	RenderRemoveModifierButton(index);
}

void RenderMaterialEntityTypePanel::SColorRGBAProperty(std::string& name, void* value, const unsigned int index)
{
	SColorRGBA* colorRGBA = static_cast<SColorRGBA*>(value);

	UI::BeginProperty(name, nullptr);

	ImGui::ColorEdit4(UI::GetPropertyID(name.c_str()).c_str(), &colorRGBA->r);

	UI::EndProperty();

	RenderRemoveModifierButton(index);
}

void RenderMaterialEntityTypePanel::SVector2Property(std::string& name, void* value, const unsigned int index)
{
	SVector2* vector = static_cast<SVector2*>(value);

	UI::BeginProperty(name, nullptr, false);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 2.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("X", buttonSize))
	{
		vector->x = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &vector->x, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector->y = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vector->y, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	UI::EndProperty();

	RenderRemoveModifierButton(index);
}

void RenderMaterialEntityTypePanel::SVector3Property(std::string& name, void* value, const unsigned int index)
{
	SVector3* vector = static_cast<SVector3*>(value);

	UI::BeginProperty(name, nullptr, false);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 3.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	bool isValueChanged = false;

	if (ImGui::Button("X", buttonSize))
	{
		vector->x = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##X", &vector->x, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector->y = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Y", &vector->y, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Z
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Z", buttonSize))
	{
		vector->z = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Z", &vector->z, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	UI::EndProperty();

	RenderRemoveModifierButton(index);
}

void RenderMaterialEntityTypePanel::SVector4Property(std::string& name, void* value, const unsigned int index)
{
	SVector4* vector = static_cast<SVector4*>(value);

	UI::BeginProperty(name, nullptr, false);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 3.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	bool isValueChanged = false;

	if (ImGui::Button("X", buttonSize))
	{
		vector->x = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##X", &vector->x, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector->y = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Y", &vector->y, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Z
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Z", buttonSize))
	{
		vector->z = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Z", &vector->z, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// W
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("W", buttonSize))
	{
		vector->z = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##W", &vector->w, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	UI::EndProperty();

	RenderRemoveModifierButton(index);
}

void RenderMaterialEntityTypePanel::RenderRemoveModifierButton(const unsigned int modifierIndex)
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
		std::vector<std::shared_ptr<RenderMaterialEntityType::Modifier>>& mattModifiers = renderMaterialEntityTypeResource->GetModifiers();
		std::vector<std::shared_ptr<RenderMaterialEntityBlueprint::Modifier>>& matbModifiers = renderMaterialEntityBlueprint->GetModifiers();

		mattModifiers.erase(mattModifiers.begin() + modifierIndex);
		matbModifiers.erase(matbModifiers.begin() + modifierIndex);
	}

	ImGui::PopID();
}
