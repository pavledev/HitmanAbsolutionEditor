#pragma once

#include "Document.h"
#include "Resources/Texture.h"

class TextureDocument : public Document
{
public:
	TextureDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const bool hasToolBar, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<Texture> GetTexture() const;
	void RenderMenuBar() override;
	void RenderToolBar() override;

private:
	void RenderZoomPercentageText() const;
	void RenderRGBAChannelButtons() const;

	std::shared_ptr<Texture> texture;
	float sliderValue;
};
