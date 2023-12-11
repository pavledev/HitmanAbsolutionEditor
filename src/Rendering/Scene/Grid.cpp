#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "backends/imgui_impl_dx11.h"
#include "imgui_internal.h"

#include "ImGuizmo.h"

#include "Rendering/Scene/Grid.h"
#include "Utility/UI.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/Scene/SceneRenderer.h"
#include "Rendering/Scene/Transform.h"
#include "Editor.h"

Grid::Grid(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
    size = 100.f;
    drawList = nullptr;
}

Grid::~Grid()
{
    delete drawList;
}

void Grid::Initialize()
{
    drawList = new ImDrawList(ImGui::GetDrawListSharedData());
}

void Grid::Render()
{
    const Matrix44& viewMatrix = SceneRenderer::GetCamera()->GetView().Transposed();
    const Matrix44& projectionMatrix = SceneRenderer::GetCamera()->GetProjection().Transposed();
    const Matrix44& worldMatrix = GetTransform()->GetWorldMatrix();

    drawList->_ResetForNewFrame();

    /*if (drawList->CmdBuffer.size() == 0)
    {
        drawList->AddDrawCmd();
    }*/

    const Viewport viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();
    const Vector2 viewportPosition = viewport.GetPosition();
    const Vector2 viewportSize = viewport.GetSize();

    drawList->PushClipRect(viewportPosition, viewportPosition + viewportSize);
    drawList->PushTextureID(Editor::GetInstance().GetImGuiRenderer()->GetDefaultFont()->ContainerAtlas->TexID);

    ImGuizmo::SetDrawlist(drawList);
    ImGuizmo::DrawGrid(&viewMatrix.m00, &projectionMatrix.m00, &worldMatrix.m00, size);

    /*Matrix viewProjection = viewMatrix * projectionMatrix;
    static float height = 1.0f;
    static float radius = 0.5f;

    ImVec2 canvasSize(200, 200);
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 startPoint(canvasPos.x + canvasSize.x / 2.0f, canvasPos.y + radius);
    float halfHeight = height / 2.0f;
    float halfRadius = radius / 2.0f;
    float centerX = startPoint.x;
    float centerY = startPoint.y + halfHeight - radius;

    const int numSegments = 16;
    const float segmentAngle = 2.0f * 3.14159f / numSegments;

    drawList->AddCircleFilled(ImVec2(centerX, centerY), radius, ImGui::GetColorU32(ImGuiCol_Button));
    drawList->AddRectFilled(ImVec2(centerX - halfRadius, centerY), ImVec2(centerX + halfRadius, centerY + height), ImGui::GetColorU32(ImGuiCol_Button));

    for (int i = 0; i < numSegments; ++i)
    {
        float angle = segmentAngle * i;
        float x = centerX + radius * sin(angle);
        float y = centerY + radius * cos(angle);
        drawList->AddLine(ImVec2(x, y), ImVec2(x, y + height), ImGui::GetColorU32(ImGuiCol_Button));
    }*/

    drawList->PopTextureID();
    drawList->PopClipRect();

    drawData.DisplayPos = viewportPosition;
    drawData.DisplaySize = viewportSize;
    //drawData.CmdLists = &drawList;
    drawData.CmdListsCount = 1;
    drawData.TotalIdxCount = drawList->IdxBuffer.size();
    drawData.TotalVtxCount = drawList->VtxBuffer.size();
    drawData.Valid = true;

    ImGui_ImplDX11_RenderDrawData(&drawData);
}

void Grid::RenderProperties()
{
    /*UI::BeginProperties();

    UI::Property("Size", size);

    UI::EndProperties();*/
}
