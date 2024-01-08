#include "UI/Panels/ModelViewerPanel.h"

ModelViewerPanel::ModelViewerPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
    renderer3D = std::make_shared<Renderer3D>();
    viewportWidth = 0;
    viewportHeight = 0;
}

void ModelViewerPanel::Render()
{
    if (!Begin())
    {
        return;
    }

    if (!renderer3D->GetCamera())
    {
        End();

        return;
    }

    const float width = ImGui::GetContentRegionAvail().x;
    const float height = ImGui::GetContentRegionAvail().y;

    if (width > 0 && height > 0 && (viewportWidth != width || viewportHeight != height))
    {
        viewportWidth = width;
        viewportHeight = height;

        renderer3D->OnViewportSizeChanged(viewportWidth, viewportHeight);
    }

    renderer3D->Render();

    End();
}

void ModelViewerPanel::OnResourceLoaded()
{
    renderer3D->Initialize();
}

std::shared_ptr<Renderer3D> ModelViewerPanel::GetRenderer3D() const
{
    return renderer3D;
}
