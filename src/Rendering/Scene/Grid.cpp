#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "backends/imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "ImGuizmo.h"

#include "Rendering/Scene/Grid.h"
#include "Utility/UI.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/Renderer3D.h"
#include "Editor.h"

Grid::Grid(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
    size = 100.f;
    drawList = nullptr;
}

void Grid::Initialize()
{
    drawList = std::make_unique<ImDrawList>(ImGui::GetDrawListSharedData());
}

void Grid::Render()
{
    const Matrix& viewMatrix = renderer3D->GetCamera()->GetView();
    const Matrix& projectionMatrix = renderer3D->GetCamera()->GetProjection();
    const Matrix& worldMatrix = GetTransform()->GetWorldMatrix();

    drawList->_ResetForNewFrame();

    const Viewport viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();
    const Vector2 viewportPosition = viewport.GetPosition();
    const Vector2 viewportSize = viewport.GetSize();

    drawList->PushClipRect(viewportPosition, viewportPosition + viewportSize);
    drawList->PushTextureID(Editor::GetInstance().GetImGuiRenderer()->GetDefaultFont()->ContainerAtlas->TexID);

    ImGuizmo::SetDrawlist(drawList.get());
    ImGuizmo::DrawGrid(&viewMatrix.m00, &projectionMatrix.m00, &worldMatrix.m00, size);

    drawList->PopTextureID();
    drawList->PopClipRect();

    drawData.Clear();

    drawData.DisplayPos = viewportPosition;
    drawData.DisplaySize = viewportSize;
    drawData.TotalIdxCount = drawList->IdxBuffer.size();
    drawData.TotalVtxCount = drawList->VtxBuffer.size();
    drawData.FramebufferScale = ImVec2(1.0f, 1.0f);
    drawData.Valid = true;

    drawData.AddDrawList(drawList.get());

    ImGui_ImplDX11_RenderDrawData(&drawData);
}

void Grid::RenderProperties()
{
    /*UI::BeginProperties();

    UI::Property("Size", size);

    UI::EndProperties();*/
}

void Grid::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
    this->renderer3D = renderer3D;
}
