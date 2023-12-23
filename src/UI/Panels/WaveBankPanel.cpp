#include "UI/Panels/WaveBankPanel.h"

WaveBankPanel::WaveBankPanel(const char* name, const char* icon, std::shared_ptr<WaveBank> waveBankResource) : BasePanel(name, icon)
{
	this->waveBankResource = waveBankResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void WaveBankPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!waveBankResource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading wave bank...");
		End();

		return;
	}

	if (!UI::BeginProperties("WaveBank", tableColumns))
	{
		End();

		return;
	}

	SWaveBankResourceData& waveBankResourceData = waveBankResource->GetWaveBankResourceData();

	UI::Property("m_nTracks", waveBankResourceData.m_nTracks);
	EnumProperty("m_eLoadType", "EWaveBankMemoryLoadRTType", waveBankResourceData.m_eLoadType);
	EnumProperty("m_eFormat", "EWaveBankRTFormat", waveBankResourceData.m_eFormat);
	UI::Property("m_nIsPlaceholder", waveBankResourceData.m_nIsPlaceholder);
	UI::Property("m_nIsNotDucking", waveBankResourceData.m_nIsNotDucking);
	UI::Property("m_fDuration", waveBankResourceData.m_fDuration);
	UI::Property("m_nChannels", waveBankResourceData.m_nChannels);
	UI::Property("m_nQuality", waveBankResourceData.m_nQuality);

	UI::EndProperties();

	End();
}

void WaveBankPanel::OnResourceLoaded()
{
	waveBankResource->Deserialize();
}
