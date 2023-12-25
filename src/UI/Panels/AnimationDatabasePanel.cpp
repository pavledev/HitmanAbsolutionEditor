#include "UI/Panels/AnimationDatabasePanel.h"

AnimationDatabasePanel::AnimationDatabasePanel(const char* name, const char* icon, std::shared_ptr<AnimationDatabase> animationDatabaseResource) : BasePanel(name, icon)
{
	this->animationDatabaseResource = animationDatabaseResource;

	tableColumns.push_back({ "Animation Name" , 0, 1.f });
	tableColumns.push_back({ "Resource ID" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void AnimationDatabasePanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!animationDatabaseResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading animation database...");
		End();

		return;
	}

	if (!UI::BeginProperties("AnimationDatabase", tableColumns))
	{
		End();

		return;
	}

	std::vector<AnimationDatabase::Entry>& entries = animationDatabaseResource->GetEntries();

	for (size_t i = 0; i < entries.size(); ++i)
	{
		UI::StringProperty(entries[i].animationName, entries[i].resourceID);
	}

	UI::EndProperties();

	End();
}

void AnimationDatabasePanel::OnResourceLoaded()
{
	animationDatabaseResource->Deserialize();
}
