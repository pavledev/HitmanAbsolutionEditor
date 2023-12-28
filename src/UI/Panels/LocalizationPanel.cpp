#include "UI/Panels/LocalizationPanel.h"

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

	if (!localizationResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading localizaiton resource...");
		End();

		return;
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
