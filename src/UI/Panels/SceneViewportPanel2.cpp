#include "UI/Panels/SceneViewportPanel2.h"

SceneViewportPanel2::SceneViewportPanel2(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive) : BasePanel(name, icon)
{
	this->renderPrimitive = renderPrimitive;
}

void SceneViewportPanel2::Render()
{
    float width = ImGui::GetContentRegionAvail().x;
    float height = ImGui::GetContentRegionAvail().y;

    if (viewportWidth == 0 || viewportHeight == 0)
    {
        viewportWidth = width;
        viewportHeight = height;

        sceneRenderer.Setup(viewportWidth, viewportHeight, renderPrimitive);
    }

    if (viewportWidth != width || viewportHeight != height)
    {
        viewportWidth = width;
        viewportHeight = height;

        sceneRenderer.OnViewportSizeChanged(viewportWidth, viewportHeight);
    }

    sceneRenderer.Render();
}
