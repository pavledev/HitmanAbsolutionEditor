#include <format>

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/LocalizationPanel.h"
#include "Utility/FileDialog.h"
#include "Logger.h"

LocalizationPanel::LocalizationPanel(const char* name, const char* icon, std::shared_ptr<Localization> localizationResource) : BasePanel(name, icon)
{
	this->localizationResource = localizationResource;

	tableColumns.push_back({ "Name/Hash" , 0, 1.f });
	tableColumns.push_back({ "Text" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void LocalizationPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!localizationResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading localization resource...");
		End();

		return;
	}

	if (ImGui::Button(ICON_MDI_IMPORT " Import JSON"))
	{
		std::string filePath = FileDialog::OpenFile("JSON Files (*.json)\0*.json\0All Files (*.*)\0*.*\0");

		if (!filePath.empty())
		{
			localizationResource->ImportFromJson(filePath);
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(ICON_MDI_CONTENT_SAVE " Patch Back To Game"))
	{
		if (localizationResource->PatchResourceLibrary())
		{
			ImGui::OpenPopup("Patch Success");
		}
		else
		{
			ImGui::OpenPopup("Patch Failed");
		}
	}

	ImGui::Separator();

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Patch Success", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Localization patched successfully!");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("Patch Failed", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Failed to patch. Check the log for details.");
		ImGui::Separator();

		if (ImGui::Button("OK", ImVec2(120, 0)))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (!UI::BeginProperties("Localization", tableColumns))
	{
		End();

		return;
	}

	char localizationCategory = localizationResource->GetLocalizationCategory();
	std::vector<std::string>& locales = localizationResource->GetLocales();
	std::vector<int>& indices = localizationResource->GetIndices();

	UI::Property("localizationCategory", localizationCategory);

	const bool isLocalesTreeNodeOpen = UI::BeginTreeNodeProperty("locales", nullptr);

	if (isLocalesTreeNodeOpen)
	{
		for (size_t i = 0; i < locales.size(); ++i)
		{
			std::string index = std::to_string(i);

			UI::StringProperty(index.c_str(), locales[i], nullptr);
		}
	}

	UI::EndTreeNodeProperty(isLocalesTreeNodeOpen);

	const bool isIndicesTreeNodeOpen = UI::BeginTreeNodeProperty("indices", nullptr);

	if (isIndicesTreeNodeOpen)
	{
		for (size_t i = 0; i < indices.size(); ++i)
		{
			std::string index = std::to_string(i);

			UI::Property(index.c_str(), indices[i]);
		}
	}

	UI::EndTreeNodeProperty(isIndicesTreeNodeOpen);

	UI::EndProperties();

	End();
}

void LocalizationPanel::OnResourceLoaded()
{
	localizationResource->Deserialize();
}
