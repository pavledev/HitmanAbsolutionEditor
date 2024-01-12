#include "Rendering/Scene/TransformGizmo.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/Scene/Camera.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/Math.h"
#include "Editor.h"

TransformGizmo::TransformGizmo()
{
    transformOperation = ImGuizmo::OPERATION::TRANSLATE;
    transformSpace = ImGuizmo::MODE::WORLD;

    ImGuizmo::SetOrthographic(false);
}

void TransformGizmo::Tick()
{
    if (!camera)
    {
        return;
    }

    const std::shared_ptr<Entity> selectedEntity = camera->GetSelectedEntity();

    ImGuizmo::Enable(selectedEntity != nullptr);

    if (!selectedEntity)
    {
        return;
    }

    if (ImGui::IsWindowHovered())
    {
        if (ImGui::IsKeyDown(ImGuiKey_T))
        {
            transformOperation = ImGuizmo::OPERATION::TRANSLATE;
        }

        if (ImGui::IsKeyDown(ImGuiKey_R))
        {
            transformOperation = ImGuizmo::OPERATION::ROTATE;
        }

        if (ImGui::IsKeyDown(ImGuiKey_E))
        {
            transformOperation = ImGuizmo::OPERATION::SCALE;
        }
    }

    const Matrix& viewMatrix = camera->GetView();
    const Matrix& projectionMatrix = camera->GetProjection();
    std::shared_ptr<Transform> transform = selectedEntity->GetComponent<Transform>();

    Vector3 worldPosition = transform->GetWorldPosition();
    Quaternion worldRotation = transform->GetWorldRotation();
    Vector3 worldScale = transform->GetWorldScale();
    Matrix transformMatrix = Matrix::GenerateRowFirst(worldPosition, worldRotation, worldScale);

    ImGuizmo::SetDrawlist();
    ImGuizmo::Manipulate(&viewMatrix.m00, &projectionMatrix.m00, transformOperation, transformSpace, &transformMatrix.m00, nullptr, nullptr);

    if (ImGuizmo::IsUsing())
    {
        transformMatrix.Transposed().Decompose(worldPosition, worldRotation, worldScale);

        transform->SetWorldPosition(worldPosition);
        transform->SetWorldRotation(worldRotation);
        transform->SetWorldScale(worldScale);
    }
}

void TransformGizmo::Update()
{
    const Viewport viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();
    const Vector2 viewportPosition = viewport.GetPosition();
    const Vector2 viewportSize = viewport.GetSize();

    ImGuizmo::SetRect(viewportPosition.x, viewportPosition.y, viewportSize.x, viewportSize.y);
}

bool TransformGizmo::IsPickingAllowed()
{
    return !ImGuizmo::IsOver() && !ImGuizmo::IsUsing();
}

void TransformGizmo::SetCamera(std::shared_ptr<Camera> camera)
{
    this->camera = camera;
}
