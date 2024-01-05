#pragma once

#include "BasePanel.h"
#include "Rendering/Scene/SceneRenderer.h"

class SceneViewportPanel2 : public BasePanel
{
public:
	SceneViewportPanel2(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive);
	void Render() override;

private:
	std::shared_ptr<RenderPrimitive> renderPrimitive;
	SceneRenderer sceneRenderer;
	float viewportWidth;
	float viewportHeight;
};
