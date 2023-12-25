#include "UI/Panels/TextListPanel.h"

TextListPanel::TextListPanel(const char* name, const char* icon, std::shared_ptr<TextList> textListResource) : BasePanel(name, icon)
{
    this->textListResource = textListResource;

    tableColumns.push_back({ "Name/Hash" , 0, 1.f });
    tableColumns.push_back({ "Text" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void TextListPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!textListResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading text list...");
		End();

		return;
	}

	if (!UI::BeginProperties("TextListEntries", tableColumns))
	{
		End();

		return;
	}

	std::vector<TextList::Entry>& entries = textListResource->GetEntries();

	for (size_t i = 0; i < entries.size(); ++i)
	{
		UI::StringProperty(entries[i].name, entries[i].text);
	}

	UI::EndProperties();

	End();
}

void TextListPanel::OnResourceLoaded()
{
	textListResource->Deserialize();
}
