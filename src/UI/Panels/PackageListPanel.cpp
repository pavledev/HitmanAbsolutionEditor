#include "UI/Panels/PackageListPanel.h"

PackageListPanel::PackageListPanel(const char* name, const char* icon, std::shared_ptr<PackageList> packageListResource) : BasePanel(name, icon)
{
	this->packageListResource = packageListResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void PackageListPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!packageListResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading package list...");
		End();

		return;
	}

	if (!UI::BeginProperties("PackageListTable", tableColumns))
	{
		End();

		return;
	}

	std::shared_ptr<SPackageListData> packageListData = packageListResource->GetPackageListData();
	const bool isFoldersTreeNodeOpen = UI::BeginTreeNodeProperty("folders", nullptr);

	if (isFoldersTreeNodeOpen)
	{
		for (size_t i = 0; i < packageListData->folders.Size(); ++i)
		{
			std::string folderName = packageListData->folders[i].name.ToCString();

			UI::StringProperty("name", folderName, nullptr);

			const bool isSectionsTreeNodeOpen = UI::BeginTreeNodeProperty("sections", nullptr);

			if (isSectionsTreeNodeOpen)
			{
				for (size_t j = 0; j < packageListData->folders[i].sections.Size(); ++j)
				{
					std::string sectionName = std::to_string(j);
					const bool isSectionTreeNodeOpen = UI::BeginTreeNodeProperty(sectionName.c_str(), nullptr);

					if (isSectionTreeNodeOpen)
					{
						const bool isSceneResourceIDsTreeNodeOpen = UI::BeginTreeNodeProperty("sceneResourceIDs", nullptr);

						if (isSceneResourceIDsTreeNodeOpen)
						{
							for (size_t k = 0; k < packageListData->folders[i].sections[j].sceneResourceIDs.Size(); ++k)
							{
								std::string name = std::format("sceneResourceID {}", k);
								std::string sceneResourceID = packageListData->folders[i].sections[j].sceneResourceIDs[k].ToCString();

								UI::StringProperty(name.c_str(), sceneResourceID, nullptr);
							}
						}

						UI::EndTreeNodeProperty(isSceneResourceIDsTreeNodeOpen);

						const bool isResourcesTreeNodeOpen = UI::BeginTreeNodeProperty("resources", nullptr);

						if (isResourcesTreeNodeOpen)
						{
							for (size_t k = 0; k < packageListData->folders[i].sections[j].resources.Size(); ++k)
							{
								std::string name = std::format("resource {}", k);
								std::string resource = packageListData->folders[i].sections[j].resources[k].ToCString();

								UI::StringProperty(name.c_str(), resource, nullptr);
							}
						}

						UI::EndTreeNodeProperty(isResourcesTreeNodeOpen);
					}

					UI::EndTreeNodeProperty(isSectionTreeNodeOpen);
				}
			}

			UI::EndTreeNodeProperty(isSectionsTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isFoldersTreeNodeOpen);

	UI::EndProperties();

	End();
}

void PackageListPanel::OnResourceLoaded()
{
	packageListResource->Deserialize();
}
