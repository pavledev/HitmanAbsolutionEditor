#pragma once

#include "Utility/UI.h"

#include "TextEditor.h"

#include "BasePanel.h"
#include "Resources/RenderMaterialEffect.h"

class ShadersPanel : public BasePanel
{
public:
	ShadersPanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialEffect> renderMaterialEffectResource);
	void Render() override;
	void OnResourceLoaded();

private:
	std::shared_ptr<RenderMaterialEffect> renderMaterialEffectResource;
	unsigned int selectedShaderIndex;
	TextEditor textEditor;
};
