#pragma once

#include "Document.h"
#include "Resources/GFXMovie.h"

class GFXMovieDocument : public Document
{
public:
	GFXMovieDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<GFXMovie> GetGFXMovie() const;
	void RenderMenuBar() override;
	void RenderToolBar() override;

private:
	void RenderZoomPercentageText() const;
	void RenderRGBAChannelButtons() const;

	std::shared_ptr<GFXMovie> gfxMovie;
	float sliderValue;
};
