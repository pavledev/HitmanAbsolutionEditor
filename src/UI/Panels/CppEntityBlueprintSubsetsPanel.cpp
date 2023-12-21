#include "UI/Panels/CppEntityBlueprintSubsetsPanel.h"
#include "Editor.h"

CppEntityBlueprintSubsetsPanel::CppEntityBlueprintSubsetsPanel(const char* name, const char* icon, std::shared_ptr<CppEntityBlueprint> cbluResource) : BasePanel(name, icon)
{
	this->cbluResource = cbluResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void CppEntityBlueprintSubsetsPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!cbluResource)
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading cpp entity blueprint...");
		End();

		return;
	}

	if (!cbluResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading cpp entity blueprint...");
		End();

		return;
	}

	ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

	RenderSubsets();

	ImGui::PopFont();

	End();
}

void CppEntityBlueprintSubsetsPanel::OnResourceLoaded()
{
	cbluResource->Deserialize();
}

void CppEntityBlueprintSubsetsPanel::SetResource(std::shared_ptr<CppEntityBlueprint> cbluResource)
{
	this->cbluResource = cbluResource;
}

void CppEntityBlueprintSubsetsPanel::RenderSubsets()
{
	if (!UI::BeginProperties("Subsets", tableColumns))
	{
		return;
	}

	TArray<SCppEntitySubsetInfo>& subsets = cbluResource->GetCppEntityBlueprint()->subsets;

	for (size_t i = 0; i < subsets.Size(); ++i)
	{
		std::string treeNodeName = std::format("Cpp Entity Subset Info {}", i);
		const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

		if (isTreeNodeOpen)
		{
			static char nameBuffer[2048] = {};
			static char typeNameBuffer[2048] = {};
			const char* typeName = subsets[i].type->pTypeInfo->GetTypeName();

			memcpy(nameBuffer, subsets[i].name.ToCString(), subsets[i].name.Length() + 1);
			memcpy(typeNameBuffer, typeName, strlen(typeName) + 1);

			UI::Property("name", nameBuffer, sizeof(nameBuffer));
			UI::Property("type", typeNameBuffer, sizeof(typeNameBuffer));
			UI::Property("flags", subsets[i].flags);
		}

		UI::EndTreeNodeProperty(isTreeNodeOpen);
	}

	UI::EndProperties();
}
