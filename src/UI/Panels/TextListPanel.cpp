#include <format>

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/TextListPanel.h"
#include "Utility/FileDialog.h"
#include "Logger.h"

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

	if (!textListResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading text list...");
		End();

		return;
	}

	if (ImGui::Button(ICON_MDI_IMPORT " Import JSON"))
	{
		ImportJson();
	}

	ImGui::SameLine();

	if (ImGui::Button(ICON_MDI_CONTENT_SAVE " Patch Back To Game"))
	{
		PatchBackToGame();
	}

	ImGui::Separator();

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

void TextListPanel::ImportJson()
{
	std::string filePath;
	std::string extension;

	FileDialog::OpenSaveFileDialog("", ".json", "JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0", filePath, extension);

	if (filePath.empty())
	{
		return;
	}

	textListResource->ImportFromJson(filePath);
}

void TextListPanel::PatchBackToGame()
{
	std::vector<unsigned char> newResourceData;

	if (!textListResource->SerializeToBinary(newResourceData))
	{
		ImGui::OpenPopup("Patch Failed");

		patchErrorMessage = "Failed to serialize text list to binary.";

		return;
	}

	if (newResourceData.size() != textListResource->GetResourceDataSize())
	{
		ImGui::OpenPopup("Patch Failed");

		patchErrorMessage = std::format("New data size ({}) does not match original ({}). You can only change text to the same length or shorter.", newResourceData.size(), textListResource->GetResourceDataSize());

		return;
	}

	if (textListResource->PatchResourceLibrary())
	{
		ImGui::OpenPopup("Patch Success");
	}
	else
	{
		ImGui::OpenPopup("Patch Failed");

		patchErrorMessage = "Failed to patch. Check the log for details.";
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Patch Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Text list patched successfully!");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Patch Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::TextWrapped("%s", patchErrorMessage.c_str());
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
