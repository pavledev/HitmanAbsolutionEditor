#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "Component.h"
#include "Entity.h"
#include "Math/Ray.h"
#include "Math/Matrix44.h"

class Camera : public Component
{
public:
	Camera() = default;
	Camera(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	void Initialize(const float fov, const float aspectRatio, const float nearPlane, const float farPlane);
	const float GetAspectRatio() const;
	const float GetHorizontalFovDeg() const;
	const float GetVerticalFovRad() const;
	std::shared_ptr<Entity> GetSelectedEntity() const;
	const Matrix44& GetProjection() const;
	const Matrix44& GetView() const;
	Matrix44& GetProjection();
	Matrix44& GetView();
	void SetPosition(const Vector3 position);
	void SetSelectedEntity(std::shared_ptr<Entity> selectedEntity);
	void SetAspectRatio(const float aspectRatio);
	void Tick() override;
	void UpdateView();
	void UpdateProjection();
	const Ray ComputePickingRay();
	void Pick();
	Vector3 ScreenToWorldCoordinates(const Vector2& screenPosition, const float z) const;
	void Render() override;
	void RenderProperties() override;

private:
	Matrix44 view;
	Matrix44 projection;
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
};
