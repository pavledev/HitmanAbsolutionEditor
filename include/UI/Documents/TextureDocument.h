#pragma once

#include "Document.h"
#include "Resources/Texture.h"

class TextureDocument : public Document
{
public:
	TextureDocument(const char* name, const char* icon, const Type type, const unsigned long long runtimeResourceID, const ImGuiID dockID);
	void CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize) override;
	std::shared_ptr<Texture> GetTexture() const;
	void RenderMenuBar() override;

private:
	std::shared_ptr<Texture> texture;
};
