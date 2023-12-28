#include "UI/Panels/GlobalResourceIndexPanel.h"

GlobalResourceIndexPanel::GlobalResourceIndexPanel(const char* name, const char* icon, std::shared_ptr<GlobalResourceIndex> globalResourceIndexResource) : BasePanel(name, icon)
{
	this->globalResourceIndexResource = globalResourceIndexResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void GlobalResourceIndexPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!globalResourceIndexResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading global resource index...");
		End();

		return;
	}

	if (!UI::BeginProperties("GlobalResourceIndexTable", tableColumns))
	{
		End();

		return;
	}

	std::shared_ptr<SResourceIndex> resourceIndex = globalResourceIndexResource->GetResourceIndex();
	const bool isFoldersTreeNodeOpen = UI::BeginTreeNodeProperty("folders", nullptr);

	if (isFoldersTreeNodeOpen)
	{
		for (size_t i = 0; i < resourceIndex->folders.Size(); ++i)
		{
			std::string folderName = resourceIndex->folders[i].name.ToCString();

			UI::StringProperty("name", folderName, nullptr);

			const bool isResourceIndicesTreeNodeOpen = UI::BeginTreeNodeProperty("resourceIndices", nullptr);

			if (isResourceIndicesTreeNodeOpen)
			{
				for (size_t j = 0; j < resourceIndex->folders[i].resourceIndices.Size(); ++j)
				{
					std::string index = std::to_string(j);

					UI::Property(index.c_str(), resourceIndex->folders[i].resourceIndices[j]);
				}
			}

			UI::EndTreeNodeProperty(isResourceIndicesTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isFoldersTreeNodeOpen);

	UI::EndProperties();

	End();
}

void GlobalResourceIndexPanel::OnResourceLoaded()
{
	globalResourceIndexResource->Deserialize();
}
