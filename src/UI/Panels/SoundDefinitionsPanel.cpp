#include "UI/Panels/SoundDefinitionsPanel.h"

SoundDefinitionsPanel::SoundDefinitionsPanel(const char* name, const char* icon, std::shared_ptr<SoundDefinitions> soundDefinitionsResource) : BasePanel(name, icon)
{
	this->soundDefinitionsResource = soundDefinitionsResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void SoundDefinitionsPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!soundDefinitionsResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading text list...");
		End();

		return;
	}

	if (!UI::BeginProperties("SoundDefinitions", tableColumns))
	{
		End();

		return;
	}

	std::vector<SoundDefinitions::Entry>& entries = soundDefinitionsResource->GetEntries();
	const std::vector<std::shared_ptr<Resource>>& sdefReferences = soundDefinitionsResource->GetReferences();
	const bool hasActorSoundDefinitions = entries.size() - 1 == static_cast<int>(SActorSoundDefs::EDefinition::_Last);

	for (size_t i = 0; i < entries.size(); ++i)
	{
		std::string treeNodeName = std::format("Entry {}", i);
		const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

		if (isTreeNodeOpen)
		{
			std::string waveBankResourceID;

			if (entries[i].referenceIndex != -1)
			{
				waveBankResourceID = sdefReferences[entries[i].referenceIndex]->GetResourceID();
			}

			if (hasActorSoundDefinitions)
			{
				EnumProperty("definition", "SActorSoundDefs.EDefinition", entries[i].definition);
			}
			else
			{
				EnumProperty("definition", "SDoorSoundDefs.EDefinition", entries[i].definition);
			}

			UI::StringProperty("waveBank", waveBankResourceID);
			UI::Property("attenuationOffset", entries[i].attenuationOffset);
			UI::Property("groupNumber", entries[i].groupNumber);
			EnumProperty("selectionMode", "ESoundPlayParameters", entries[i].selectionMode);
			UI::Property("noRepeatsCount", entries[i].noRepeatsCount);

			const bool isSubSoundRepeatCountsNodeOpen = UI::BeginTreeNodeProperty("subSoundRepeatCounts", nullptr);

			if (isSubSoundRepeatCountsNodeOpen)
			{
				for (size_t j = 0; j < entries[i].subSoundRepeatCounts.size(); ++j)
				{
					std::string name = std::to_string(j);

					UI::Property(name.c_str(), entries[i].subSoundRepeatCounts[j]);
				}
			}

			UI::EndTreeNodeProperty(isSubSoundRepeatCountsNodeOpen);
		}

		UI::EndTreeNodeProperty(isTreeNodeOpen);
	}

	UI::EndProperties();

	End();
}

void SoundDefinitionsPanel::OnResourceLoaded()
{
	soundDefinitionsResource->Deserialize();
}
