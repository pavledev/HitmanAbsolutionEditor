#include "UI/Panels/SoundBlendContainerExternalParametersTypePanel.h"

SoundBlendContainerExternalParametersTypePanel::SoundBlendContainerExternalParametersTypePanel(const char* name, const char* icon, std::shared_ptr<SoundBlendContainerExternalParametersType> soundBlendContainerExternalParametersTypeResource) : BasePanel(name, icon)
{
	this->soundBlendContainerExternalParametersTypeResource = soundBlendContainerExternalParametersTypeResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void SoundBlendContainerExternalParametersTypePanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!soundBlendContainerExternalParametersTypeResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading resource...");
		End();

		return;
	}

	if (!UI::BeginProperties("SoundBlendContainerExternalParametersTypeTable", tableColumns))
	{
		End();

		return;
	}

	unsigned int nextID = soundBlendContainerExternalParametersTypeResource->GetNextID();
	std::vector<SoundBlendContainerExternalParametersType::SoundBlendContainerExternalParameter>& soundBlendContainerExternalParameters = soundBlendContainerExternalParametersTypeResource->GetSoundBlendContainerExternalParameters();

	UI::Property("NextID", nextID);

	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty("Parameters", nullptr);

	if (isTreeNodeOpen)
	{
		for (size_t i = 0; i < soundBlendContainerExternalParameters.size(); ++i)
		{
			std::string treeNodeName = std::format("Parameter {}", i);
			const bool isParameterTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

			if (isParameterTreeNodeOpen)
			{
				UI::StringProperty("name", soundBlendContainerExternalParameters[i].name, nullptr);
				UI::Property("id", soundBlendContainerExternalParameters[i].id);

				const bool isPropertiesTreeNodeOpen = UI::BeginTreeNodeProperty("properties", nullptr);

				if (isPropertiesTreeNodeOpen)
				{
					UI::Property("m_bIsDistanceControlled", soundBlendContainerExternalParameters[i].properties.m_bIsDistanceControlled);
					UI::Property("m_fMin", soundBlendContainerExternalParameters[i].properties.m_fMin);
					UI::Property("m_fMax", soundBlendContainerExternalParameters[i].properties.m_fMax);
					UI::Property("m_fDefault", soundBlendContainerExternalParameters[i].properties.m_fDefault);
					UI::Property("m_fSustainPointValue", soundBlendContainerExternalParameters[i].properties.m_fSustainPointValue);
					UI::Property("m_fSustainPointVelocity", soundBlendContainerExternalParameters[i].properties.m_fSustainPointVelocity);
				}

				UI::EndTreeNodeProperty(isPropertiesTreeNodeOpen);
			}

			UI::EndTreeNodeProperty(isParameterTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);

	UI::EndProperties();

	End();
}

void SoundBlendContainerExternalParametersTypePanel::OnResourceLoaded()
{
	soundBlendContainerExternalParametersTypeResource->Deserialize();
}
