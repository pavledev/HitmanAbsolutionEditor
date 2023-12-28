#include "UI/Panels/BehaviorTreeEntityBlueprintPanel.h"

BehaviorTreeEntityBlueprintPanel::BehaviorTreeEntityBlueprintPanel(const char* name, const char* icon, std::shared_ptr<BehaviorTreeEntityBlueprint> behaviorTreeEntityBlueprintResource) : BasePanel(name, icon)
{
	this->behaviorTreeEntityBlueprintResource = behaviorTreeEntityBlueprintResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void BehaviorTreeEntityBlueprintPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!behaviorTreeEntityBlueprintResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading behavior treeEntity blueprint...");
		End();

		return;
	}

	if (!UI::BeginProperties("BehaviorTreeEntityBlueprintTable", tableColumns))
	{
		End();

		return;
	}

	std::shared_ptr<SBehaviorTreeInfo> behaviorTreeInfo = behaviorTreeEntityBlueprintResource->GetBehaviorTreeInfo();
	const bool isReferencesTreeNodeOpen = UI::BeginTreeNodeProperty("m_references", nullptr);

	if (isReferencesTreeNodeOpen)
	{
		for (size_t i = 0; i < behaviorTreeInfo->m_references.Size(); ++i)
		{
			const std::string treeNodeName = std::format("Behavior Tree Entity Reference {}", i);
			const bool isEntityReferenceTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

			if (isEntityReferenceTreeNodeOpen)
			{
				UI::Property("m_bList", behaviorTreeInfo->m_references[i].m_bList);

				std::string name = behaviorTreeInfo->m_references[i].m_sName.ToCString();

				UI::StringProperty("m_sName", name, nullptr);
			}

			UI::EndTreeNodeProperty(isEntityReferenceTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isReferencesTreeNodeOpen);

	const bool isInputPinConditionsTreeNodeOpen = UI::BeginTreeNodeProperty("m_inputPinConditions", nullptr);

	if (isInputPinConditionsTreeNodeOpen)
	{
		for (size_t i = 0; i < behaviorTreeInfo->m_inputPinConditions.Size(); ++i)
		{
			const std::string treeNodeName = std::format("Input Pin Condition {}", i);
			const bool isInputPinConditionTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

			if (isInputPinConditionTreeNodeOpen)
			{
				std::string name = behaviorTreeInfo->m_inputPinConditions[i].m_sName.ToCString();

				UI::StringProperty("m_sName", name, nullptr);
			}

			UI::EndTreeNodeProperty(isInputPinConditionTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isInputPinConditionsTreeNodeOpen);

	UI::EndProperties();

	End();
}

void BehaviorTreeEntityBlueprintPanel::OnResourceLoaded()
{
	behaviorTreeEntityBlueprintResource->Deserialize();
}
