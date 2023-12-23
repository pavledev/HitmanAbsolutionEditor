#pragma once

#include "Document.h"
#include "Resources/WaveBank.h"

class WaveBankDocument : public Document
{
public:
	WaveBankDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<WaveBank> GetWaveBank() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<WaveBank> waveBank;
};
