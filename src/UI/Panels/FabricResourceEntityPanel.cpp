#include <IconsMaterialDesignIcons.h>

#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/Math/SVector4.h"

#include "UI/Panels/FabricResourceEntityPanel.h"
#include "Registry/ResourceInfoRegistry.h"

FabricResourceEntityPanel::FabricResourceEntityPanel(const char* name, const char* icon, std::shared_ptr<FabricResourceEntity> fabricResourceEntity) : BasePanel(name, icon)
{
	this->fabricResourceEntity = fabricResourceEntity;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	tableColumns.push_back({ "" , 0, 0.2f });
}

void FabricResourceEntityPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!fabricResourceEntity->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading fabric resource entity...");
		End();

		return;
	}

	if (!fabricResourceEntityBlueprint || fabricResourceEntityBlueprint && !fabricResourceEntityBlueprint->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading fabric resource entity blueprint...");
		End();

		return;
	}

	RenderProperties();

	End();
}

void FabricResourceEntityPanel::OnResourceLoaded()
{
	fabricResourceEntity->Deserialize();

	std::vector<std::shared_ptr<Resource>>& clotReferences = fabricResourceEntity->GetReferences();

	for (size_t i = 0; i < clotReferences.size(); ++i)
	{
		const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(clotReferences[i]->GetHash());

		if (referenceInfo.type == "CLOB")
		{
			fabricResourceEntityBlueprint = std::static_pointer_cast<FabricResourceEntityBlueprint>(clotReferences[i]);

			break;
		}
	}

	const ResourceInfoRegistry::ResourceInfo& matbResourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(fabricResourceEntityBlueprint->GetHash());

	fabricResourceEntityBlueprint->LoadResource(0, matbResourceInfo.headerLibraries[0].chunkIndex, matbResourceInfo.headerLibraries[0].indexInLibrary, false, false, true);
	fabricResourceEntityBlueprint->Deserialize();
	fabricResourceEntityBlueprint->DeleteResourceData();
}

void FabricResourceEntityPanel::RenderProperties()
{
	if (!UI::BeginProperties("FabricResourceEntityProperties", tableColumns))
	{
		return;
	}

	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet>& clothPiecePropertySets = fabricResourceEntity->GetClothPiecePropertySets();
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet>& clothPieceExtendedPropertySets = fabricResourceEntity->GetClothPieceExtendedPropertySets();
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet>& transformPropertySets = fabricResourceEntity->GetTransformPropertySets();
	std::vector<ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet>& strandsPropertySets = fabricResourceEntity->GetStrandsPropertySets();

	std::vector<std::string>& clothPieceNames = fabricResourceEntityBlueprint->GetClothPieceNames();
	std::vector<std::string>& clothPieceExtendedNames = fabricResourceEntityBlueprint->GetClothPieceExtendedNames();
	std::vector<std::string>& transformNames = fabricResourceEntityBlueprint->GetTransformNames();
	std::vector<std::string>& strandsNames = fabricResourceEntityBlueprint->GetStrandsNames();

	for (size_t i = 0; i < clothPieceNames.size(); ++i)
	{
		SClothPiecePropertySetProperty(clothPieceNames[i].c_str(), clothPiecePropertySets[i]);
	}

	for (size_t i = 0; i < clothPieceExtendedNames.size(); ++i)
	{
		SClothPieceExtendedPropertySetProperty(clothPieceExtendedNames[i].c_str(), clothPieceExtendedPropertySets[i]);
	}

	for (size_t i = 0; i < transformNames.size(); ++i)
	{
		STransformPropertySetProperty(transformNames[i].c_str(), transformPropertySets[i]);
	}

	for (size_t i = 0; i < strandsNames.size(); ++i)
	{
		SStrandsPropertySetProperty(strandsNames[i].c_str(), strandsPropertySets[i]);
	}

	UI::EndProperties();
}

void FabricResourceEntityPanel::SClothPiecePropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet& clothPiecePropertySet)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		SVector3Property("m_vGravity", clothPiecePropertySet.m_vGravity);
		UI::Property("m_nNumIterations", clothPiecePropertySet.m_nNumIterations);
		UI::Property("m_fDamping", clothPiecePropertySet.m_fDamping);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void FabricResourceEntityPanel::SClothPieceExtendedPropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet& clothPieceExtendedPropertySet)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		ZRuntimeResourceIDProperty("m_MaterialInstance", clothPieceExtendedPropertySet.m_MaterialInstance);
		SVector3Property("m_vGravity", clothPieceExtendedPropertySet.m_vGravity);
		UI::Property("m_nNumIterations", clothPieceExtendedPropertySet.m_nNumIterations);
		UI::Property("m_fDamping", clothPieceExtendedPropertySet.m_fDamping);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void FabricResourceEntityPanel::STransformPropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet& transformPropertySet)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void FabricResourceEntityPanel::SStrandsPropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet& strandsPropertySet)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		UI::Property("m_fStrandWidth", strandsPropertySet.m_fStrandWidth);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void FabricResourceEntityPanel::SVector3Property(const char* name, SVector3& vector)
{
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
		vector.x = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f"))
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
		vector.y = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f"))
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
		vector.z = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Z", &vector.z, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	UI::EndProperty();
}

void FabricResourceEntityPanel::ZRuntimeResourceIDProperty(const char* propertyName, ZRuntimeResourceID& runtimeResourceID)
{
	static char stringBuffer[2048] = {};

	if (runtimeResourceID.GetID() != -1)
	{
		const std::string& resourceID = ZRuntimeResourceID::QueryResourceID(runtimeResourceID);

		memcpy(stringBuffer, resourceID.c_str(), resourceID.length() + 1);
	}
	else
	{
		stringBuffer[0] = '\0';
	}

	UI::Property(propertyName, stringBuffer, sizeof(stringBuffer), nullptr, false, [&](char* newString)
	{
		runtimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(stringBuffer);
	});
}
