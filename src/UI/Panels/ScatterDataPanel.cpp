#include "UI/Panels/ScatterDataPanel.h"

ScatterDataPanel::ScatterDataPanel(const char* name, const char* icon, std::shared_ptr<ScatterData> scatterDataResource) : BasePanel(name, icon)
{
	this->scatterDataResource = scatterDataResource;

	tableColumns.push_back({ "Name/Hash" , 0, 1.f });
	tableColumns.push_back({ "Text" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void ScatterDataPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!scatterDataResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading scatter data...");
		End();

		return;
	}

	if (!UI::BeginProperties("ScatterDataTable", tableColumns))
	{
		End();

		return;
	}

	SScatterDataHeader& scatterDataHeader = scatterDataResource->GetScatterDataHeader();
	std::vector<SScatterPackedMaterial>& materialHeaders = scatterDataResource->GetMaterialHeaders();
	std::vector<std::vector<SScatterPackedInstance>>& instances = scatterDataResource->GetInstances();
	const std::vector<std::shared_ptr<Resource>> references = scatterDataResource->GetReferences();

	const bool isScatterDataHeaderTreeNodeOpen = UI::BeginTreeNodeProperty("scatterDataHeader", nullptr);

	if (isScatterDataHeaderTreeNodeOpen)
	{
		UI::Property("m_nType", scatterDataHeader.m_nType);
		UI::Property("m_nMaterials", scatterDataHeader.m_nMaterials);
	}

	UI::EndTreeNodeProperty(isScatterDataHeaderTreeNodeOpen);

	const bool isMaterialHeadersTreeNodeOpen = UI::BeginTreeNodeProperty("materialHeaders", nullptr);

	if (isMaterialHeadersTreeNodeOpen)
	{
		for (size_t i = 0; i < materialHeaders.size(); ++i)
		{
			std::string materialHeaderIndex = std::to_string(i);
			const bool isMaterialHeaderTreeNodeOpen = UI::BeginTreeNodeProperty(materialHeaderIndex.c_str(), nullptr);

			if (isMaterialHeaderTreeNodeOpen)
			{
				std::string resourceID = references[i]->GetResourceID();

				UI::StringProperty("m_idResource", resourceID);

				const bool isBaseTreeNodeOpen = UI::BeginTreeNodeProperty("m_Base", nullptr);

				if (isBaseTreeNodeOpen)
				{
					UI::Property("m_nInstances", materialHeaders[i].m_Base.m_nInstances);
					UI::Property("m_bBend", materialHeaders[i].m_Base.m_bBend);
					UI::Property("m_fBendConstraint", materialHeaders[i].m_Base.m_fBendConstraint);
					UI::Property("m_fCutoffDistance", materialHeaders[i].m_Base.m_fCutoffDistance);
					UI::Property("m_fScaleBeginDistance", materialHeaders[i].m_Base.m_fScaleBeginDistance);
				}

				UI::EndTreeNodeProperty(isBaseTreeNodeOpen);

				const bool isInstancesTreeNodeOpen = UI::BeginTreeNodeProperty("instances", nullptr);

				if (isInstancesTreeNodeOpen)
				{
					for (size_t j = 0; j < instances[i].size(); ++j)
					{
						std::string instanceIndex = std::to_string(j);
						const bool isInstanceTreeNodeOpen = UI::BeginTreeNodeProperty(instanceIndex.c_str(), nullptr);

						if (isInstanceTreeNodeOpen)
						{
							const bool isDataTreeNodeOpen = UI::BeginTreeNodeProperty("m_nData", nullptr);

							if (isDataTreeNodeOpen)
							{
								for (size_t k = 0; k < 11; ++k)
								{
									std::string index = std::to_string(k);

									UI::Property(index.c_str(), instances[i][j].m_nData[k]);
								}
							}

							UI::EndTreeNodeProperty(isDataTreeNodeOpen);
						}

						UI::EndTreeNodeProperty(isInstanceTreeNodeOpen);
					}
				}

				UI::EndTreeNodeProperty(isInstancesTreeNodeOpen);
			}

			UI::EndTreeNodeProperty(isMaterialHeaderTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isMaterialHeadersTreeNodeOpen);

	UI::EndProperties();

	End();
}

void ScatterDataPanel::OnResourceLoaded()
{
	scatterDataResource->Deserialize();
}
