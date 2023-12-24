#pragma once

#include "Document.h"
#include "Resources/FlashMovie.h"

class FlashMovieDocument : public Document
{
public:
	FlashMovieDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<FlashMovie> GetFlashMovie() const;
	void RenderMenuBar() override;
	void RenderToolBar() override;

private:
	void OnResourceLoaded();
	void RenderZoomPercentageText() const;
	void RenderRGBAChannelButtons() const;

	std::shared_ptr<FlashMovie> flashMovie;
	float sliderValue;
};
