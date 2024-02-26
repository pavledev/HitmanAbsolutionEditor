#include "Utility/UI.h"
#include "Utility/Math.h"
#include "Rendering/Scene/Camera.h"
#include "Timer.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/DirectXRenderer.h"
#include "Math/Vector4.h"
#include "Math/RayHit.h"
#include "Rendering/Scene/Model.h"
#include "Editor.h"

Camera::Camera(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
}

void Camera::Initialize(const float fov, const float aspectRatio, const float nearPlane, const float farPlane, const Vector3& position, const Vector3& rotation, const float pitch, const float yaw)
{
	horizontalFov = DirectX::XMConvertToRadians(fov);
	this->aspectRatio = aspectRatio;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->pitch = pitch;
	this->yaw = yaw;
	speed = 1;
	sensitivity = 20;

	GetTransform()->SetWorldPosition(position);
	GetTransform()->SetWorldRotation(Quaternion::FromEulerAngles(rotation));

	GetCursorPos(&previousMousePosition);
	UpdateProjection();
	UpdateView();
}

const float Camera::GetAspectRatio() const
{
	return aspectRatio;
}

const float Camera::GetHorizontalFovDeg() const
{
	return DirectX::XMConvertToDegrees(horizontalFov);
}

const float Camera::GetVerticalFovRad() const
{
	const Viewport& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport();
	const float viewportWidth = viewport.GetSize().x;
	const float viewportHeight = viewport.GetSize().y;

	return 2 * std::atan(std::tan(horizontalFov / 2.0f) * (viewportHeight / viewportWidth));
}

const Matrix& Camera::GetProjection() const
{
	return projection;
}

std::shared_ptr<Entity> Camera::GetSelectedEntity() const
{
	return selectedEntity;
}

const Matrix& Camera::GetView() const
{
	return view;
}

Matrix& Camera::GetProjection()
{
	return projection;
}

Matrix& Camera::GetView()
{
	return view;
}

void Camera::SetPosition(const Vector3 position)
{
	GetTransform()->SetWorldPosition(position);
}

void Camera::SetAspectRatio(const float aspectRatio)
{
	this->aspectRatio = aspectRatio;
}

void Camera::Tick()
{
	POINT mousePosition;

	GetCursorPos(&mousePosition);

	if (!ImGui::IsWindowHovered() || !ImGui::IsMouseDown(ImGuiMouseButton_Right))
	{
		previousMousePosition = mousePosition;

		return;
	}

	ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	POINT direction{};

	direction.x = mousePosition.x - previousMousePosition.x;
	direction.y = previousMousePosition.y - mousePosition.y;
	previousMousePosition = mousePosition;

	pitch -= direction.y * sensitivity * 0.01f;
	yaw -= direction.x * sensitivity * 0.01f;

	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	std::shared_ptr<Transform> transform = GetTransform();
	Vector3 worldRotation = Math::ConvertDegressToRadians({ pitch, yaw, 0 });

	transform->SetWorldRotation(Quaternion::FromYawPitchRoll(worldRotation.y, worldRotation.x, worldRotation.z));

	float velocity = speed * static_cast<float>(Timer::GetDeltaTime()) * (ImGui::GetIO().KeyShift ? 8.0f : 2.0f);
	Vector3 position = transform->GetWorldPosition();

	if (ImGui::IsKeyDown(ImGuiKey_W))
	{
		position += transform->GetForward() * velocity;

		transform->SetWorldPosition(position);
	}

	if (ImGui::IsKeyDown(ImGuiKey_S))
	{
		position -= transform->GetForward() * velocity;

		transform->SetWorldPosition(position);
	}

	if (ImGui::IsKeyDown(ImGuiKey_Q))
	{
		position += transform->GetUp() * velocity;

		transform->SetWorldPosition(position);
	}

	if (ImGui::IsKeyDown(ImGuiKey_E))
	{
		position -= transform->GetUp() * velocity;

		transform->SetWorldPosition(position);
	}

	if (ImGui::IsKeyDown(ImGuiKey_A))
	{
		position += transform->GetRight() * velocity;

		transform->SetWorldPosition(position);
	}

	if (ImGui::IsKeyDown(ImGuiKey_D))
	{
		position -= transform->GetRight() * velocity;

		transform->SetWorldPosition(position);
	}

	UpdateView();
}

void Camera::UpdateView()
{
	const Vector3& position = GetTransform()->GetWorldPosition();
	const Vector3& lookAt = GetTransform()->GetWorldPosition() + GetTransform()->GetForward();
	const Vector3& up = GetTransform()->GetUp();

	view = Matrix::CreateLookAtRH(position, lookAt, up);
}

void Camera::UpdateProjection()
{
	projection = Matrix::CreatePerspectiveFieldOfViewRH(horizontalFov, aspectRatio, nearPlane, farPlane);
}

void Camera::SetSelectedEntity(std::shared_ptr<Entity> selectedEntity)
{
	this->selectedEntity = selectedEntity;
}

const Ray Camera::ComputePickingRay()
{
	std::shared_ptr<Transform> transform = GetTransform();
	Vector3 rayStart = transform->GetWorldPosition();
	Vector3 rayDirection = ScreenToWorldCoordinates(UI::GetMousePositionRelativeToWindow(), 1.0f);

	return Ray(rayStart, rayDirection);
}

void Camera::Pick()
{
	const Ray ray = ComputePickingRay();

	// Traces ray against all AABBs in the world
	std::vector<RayHit> hits;
	std::vector<std::shared_ptr<Entity>> meshEntities = GetMeshEntities();

	for (size_t i = 0; i < meshEntities.size(); ++i)
	{
		// Get object oriented bounding box
		const BoundingBox& aabb = meshEntities[i]->GetComponent<Mesh>()->GetBoundingBox(Mesh::BoundingBoxType::Transformed);

		// Compute hit distance
		const float distance = ray.HitDistance(aabb);

		// Don't store hit data if there was no hit
		if (distance == std::numeric_limits<float>::infinity())
		{
			continue;
		}

		hits.emplace_back(
			meshEntities[i],                                        // Entity
			ray.GetStart() + ray.GetDirection() * distance, // Position
			distance,                                           // Distance
			distance == 0.0f                                    // Inside
		);
	}

	// Sort by distance (ascending)
	std::sort(hits.begin(), hits.end(), [](const RayHit& a, const RayHit& b) { return a.distance < b.distance; });

	if (hits.empty())
	{
		selectedEntity = nullptr;

		return;
	}

	if (hits.size() == 1)
	{
		selectedEntity = hits.front().entity;

		return;
	}

	// If there are more hits, perform triangle intersection
	float minDistance = std::numeric_limits<float>::max();

	for (RayHit& hit : hits)
	{
		// Get entity geometry
		std::shared_ptr<Mesh> mesh = hit.entity->GetComponent<Mesh>();
		const std::vector<VertexPosition>& vertexPositions = mesh->GetVertexPositions();
		const std::vector<unsigned int>& indices = mesh->GetIndices();

		if (indices.empty() || vertexPositions.empty())
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Failed to get geometry of entity {}, skipping intersection test.", hit.entity->GetName());

			continue;
		}

		// Compute matrix which can transform vertices to view space
		Matrix vertexTransform = hit.entity->GetComponent<Transform>()->GetWorldMatrix();

		// Go through each face
		for (unsigned short i = 0; i < indices.size(); i += 3)
		{
			Vector3 p1World = Vector3(vertexPositions[indices[i]].position) * vertexTransform;
			Vector3 p2World = Vector3(vertexPositions[indices[i + 1]].position) * vertexTransform;
			Vector3 p3World = Vector3(vertexPositions[indices[i + 2]].position) * vertexTransform;

			float distance = ray.HitDistance(p1World, p2World, p3World);

			if (distance < minDistance)
			{
				selectedEntity = hit.entity;
				minDistance = distance;
			}
		}
	}
}

std::vector<std::shared_ptr<Entity>> Camera::GetMeshEntities()
{
	std::vector<std::shared_ptr<Entity>> meshEntities;
	const std::vector<std::shared_ptr<Entity>>& entities = renderer3D->GetRootEntity()->GetChildren();

	for (size_t i = 0; i < entities.size(); ++i)
	{
		if (!entities[i]->GetComponent<Model>())
		{
			continue;
		}

		const std::vector<std::shared_ptr<Entity>>& modelEntityChildren = entities[i]->GetChildren();

		for (size_t j = 0; j < modelEntityChildren.size(); ++j)
		{
			if (modelEntityChildren[j]->GetComponent<Mesh>())
			{
				meshEntities.push_back(modelEntityChildren[j]);
			}
			else if (modelEntityChildren[j]->GetComponent<Skeleton>())
			{
				const std::vector<std::shared_ptr<Entity>>& skeletonEntityChildren = modelEntityChildren[j]->GetChildren();

				for (size_t k = 0; k < skeletonEntityChildren.size(); ++k)
				{
					const std::vector<std::shared_ptr<Entity>>& modelBoneEntityChildren = skeletonEntityChildren[k]->GetChildren();

					for (size_t l = 0; l < modelBoneEntityChildren.size(); ++l)
					{
						meshEntities.push_back(modelBoneEntityChildren[l]);
					}
				}
			}
		}
	}

	return meshEntities;
}

Vector3 Camera::ScreenToWorldCoordinates(const Vector2& screenPosition, const float z) const
{
	const D3D11_VIEWPORT& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport().GetD3DViewport();

	// Convert screen space position to clip space position
	Vector3 clipPosition;
	clipPosition.x = (screenPosition.x / viewport.Width) * 2.0f - 1.0f;
	clipPosition.y = (screenPosition.y / viewport.Height) * -2.0f + 1.0f;
	clipPosition.z = std::clamp(z, 0.0f, 1.0f);

	// Compute world space position
	Matrix invertedViewProjection = (view * projection).Inverted();
	Vector4 worldPosition = Vector4(clipPosition, 1.0f) * invertedViewProjection;

	return Vector3(worldPosition) / worldPosition.w;
}

Vector2 Camera::WorldToScreenCoordinates(const Vector3& worldPosition) const
{
	const D3D11_VIEWPORT& viewport = Editor::GetInstance().GetDirectXRenderer()->GetViewport().GetD3DViewport();

	// Convert world space position to clip space position
	const Vector3 clipPosition = worldPosition * view * projection;

	// Convert clip space position to screen space position
	Vector2 screenPosition;
	screenPosition.x = (clipPosition.x / clipPosition.z) * (0.5f * viewport.Width) + (0.5f * viewport.Width);
	screenPosition.y = (clipPosition.y / clipPosition.z) * -(0.5f * viewport.Height) + (0.5f * viewport.Height);

	return screenPosition;
}

void Camera::Render()
{
	CameraConstantBuffer& cameraConstantBufferCpu = renderer3D->GetCameraConstantBufferCpu();
	Matrix worldView = GetTransform()->GetWorldMatrix() * renderer3D->GetCamera()->GetView();

	cameraConstantBufferCpu.viewProjection = view * projection;
	cameraConstantBufferCpu.eyePosition = GetTransform()->GetWorldPosition();

	renderer3D->UpdateCameraConstantBuffer();
}

void Camera::RenderProperties()
{
	static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	static std::vector<UI::TableColumn> tableColumns;

	if (tableColumns.empty())
	{
		tableColumns.push_back({ "Name" , 0, 1.f });
		tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	}

	if (ImGui::TreeNodeEx("Camera", treeNodeFlags))
	{
		UI::BeginProperties("CameraProperties", tableColumns, false);

		if (UI::Property("Horizontal FOV", horizontalFov))
		{
			UpdateProjection();
		}

		UI::Property("Speed", speed);
		UI::Property("Sensitivity", sensitivity);

		UI::EndProperties();

		ImGui::TreePop();
	}
}

void Camera::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
	this->renderer3D = renderer3D;
}
