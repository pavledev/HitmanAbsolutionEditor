#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Component.h"
#include "Entity.h"
#include "Math/Ray.h"
#include "Math/Matrix.h"

class Renderer3D;

class Camera : public Component
{
public:
	Camera() = default;
	Camera(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(const float fov, const float aspectRatio, const float nearPlane, const float farPlane, const Vector3& position = Vector3(0.f, 0.f, 0.f), const Vector3& rotation = Vector3(0.f, 0.f, 0.f), const float pitch = 0, const float yaw = 0);
	const float GetAspectRatio() const;
	const float GetHorizontalFovDeg() const;
	const float GetVerticalFovRad() const;
	std::shared_ptr<Entity> GetSelectedEntity() const;
	const Matrix& GetProjection() const;
	const Matrix& GetView() const;
	Matrix& GetProjection();
	Matrix& GetView();
	void SetPosition(const Vector3 position);
	void SetSelectedEntity(std::shared_ptr<Entity> selectedEntity);
	void SetAspectRatio(const float aspectRatio);
	void Tick() override;
	void UpdateView();
	void UpdateProjection();
	const Ray ComputePickingRay();
	void Pick();
	Vector3 ScreenToWorldCoordinates(const Vector2& screenPosition, const float z) const;
	Vector2 WorldToScreenCoordinates(const Vector3& worldPosition) const;
	void Render() override;
	void RenderProperties() override;
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	Matrix view;
	Matrix projection;
	POINT previousMousePosition;
	float aspectRatio;
	float horizontalFov;
	float nearPlane;
	float farPlane;
	float pitch;
	float yaw;
	float speed;
	float sensitivity;
	std::shared_ptr<Entity> selectedEntity;
	std::shared_ptr<Renderer3D> renderer3D;
};
