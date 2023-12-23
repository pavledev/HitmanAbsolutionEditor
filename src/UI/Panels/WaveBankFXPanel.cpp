#include "UI/Panels/WaveBankFXPanel.h"

WaveBankFXPanel::WaveBankFXPanel(const char* name, const char* icon, std::shared_ptr<WaveBankFX> waveBankFXResource) : BasePanel(name, icon)
{
	this->waveBankFXResource = waveBankFXResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void WaveBankFXPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!waveBankFXResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading wave bank fx...");
		End();

		return;
	}

	if (!UI::BeginProperties("WaveBankFX", tableColumns))
	{
		End();

		return;
	}

	std::vector<WaveBankFX::FaceFXAnimationReference>& faceFXAnimationReferences = waveBankFXResource->GetFaceFXAnimationReferences();
	std::vector<WaveBankFX::SubtitleReference>& subtitleReferences = waveBankFXResource->GetSubtitleReferences();

	const bool isfaceFXAnimationReferencesTreeNodeOpen = UI::BeginTreeNodeProperty("faceFXAnimationReferences", nullptr);

	if (isfaceFXAnimationReferencesTreeNodeOpen)
	{
		for (size_t i = 0; i < faceFXAnimationReferences.size(); ++i)
		{
			std::string treeNodeName = std::format("FaceFXAnimationReference {}", i);
			const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

			if (isTreeNodeOpen)
			{
				const unsigned int animationSetResourceIndex = faceFXAnimationReferences[i].animationSetResourceIndex;
				std::string animationSetResourceID;

				if (animationSetResourceIndex != -1)
				{
					std::vector<std::shared_ptr<Resource>>& wbfxReferences = waveBankFXResource->GetReferences();

					animationSetResourceID = wbfxReferences[animationSetResourceIndex]->GetResourceID();
				}

				UI::StringProperty("animationSetResourceID", animationSetResourceID, nullptr);
				UI::StringProperty("animationName", faceFXAnimationReferences[i].animationName, nullptr);
			}

			UI::EndTreeNodeProperty(isTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isfaceFXAnimationReferencesTreeNodeOpen);

	const bool issubtitleReferencesTreeNodeOpen = UI::BeginTreeNodeProperty("subtitleReferences", nullptr);

	if (issubtitleReferencesTreeNodeOpen)
	{
		for (size_t i = 0; i < subtitleReferences.size(); ++i)
		{
			std::string treeNodeName = std::format("SubtitleReference {}", i);
			const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(treeNodeName.c_str(), nullptr);

			if (isTreeNodeOpen)
			{
				const unsigned int textlistResourceIndex = subtitleReferences[i].textlistResourceIndex;
				std::string textlistResourceID;

				if (textlistResourceIndex != -1)
				{
					std::vector<std::shared_ptr<Resource>>& wbfxReferences = waveBankFXResource->GetReferences();

					textlistResourceID = wbfxReferences[textlistResourceIndex]->GetResourceID();
				}

				UI::StringProperty("textlistResourceID", textlistResourceID, nullptr);
				UI::StringProperty("textListEntryName", subtitleReferences[i].textListEntryName);
			}

			UI::EndTreeNodeProperty(isTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(issubtitleReferencesTreeNodeOpen);

	UI::EndProperties();

	End();
}

void WaveBankFXPanel::OnResourceLoaded()
{
	waveBankFXResource->Deserialize();
}
