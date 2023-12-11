#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Component.h"
#include "Math/Matrix44.h"

class Transform : public Component
{
public:
	Transform() = default;
	Transform(const char* name, const char* icon, std::weak_ptr<Entity> entity);
	explicit Transform(const Matrix44& matrix);
	const Vector3& GetLocalPosition() const;
	const Quaternion& GetLocalRotation() const;
	const Vector3& GetLocalScale() const;
	Vector3& GetLocalPosition();
	Quaternion& GetLocalRotation();
	Vector3& GetLocalScale();
	const Vector3& GetWorldPosition() const;
	const Quaternion& GetWorldRotation() const;
	const Vector3& GetWorldScale() const;
	Vector3& GetWorldPosition();
	Quaternion& GetWorldRotation();
	Vector3& GetWorldScale();
	const Matrix44& GetLocalMatrix() const;
	Matrix44& GetLocalMatrix();
	const Matrix44& GetWorldMatrix() const;
	Matrix44& GetWorldMatrix();
	const Vector3 GetRight() const;
	const Vector3 GetUp() const;
	const Vector3 GetForward() const;
	void SetLocalPosition(const Vector3& localPosition);
	void SetLocalRotation(const Quaternion& localRotation);
	void SetLocalScale(const Vector3& localScale);
	void SetWorldPosition(const Vector3& worldPosition);
	void SetWorldRotation(const Quaternion& worldRotation);
	void SetWorldScale(const Vector3& worldScale);
	void Tick() override;
	void Render() override;
	void UpdateTransform();
	void SetParent(std::weak_ptr<Transform> parent);
	void AddChild(std::shared_ptr<Transform> child);
	void RenderProperties() override;

private:
	Matrix44 localMatrix;
	Vector3 localPosition;
	Quaternion localRotation;
	Vector3 localScale;

	Matrix44 worldMatrix;
	Vector3 worldPosition;
	Quaternion worldRotation;
	Vector3 worldScale;

	std::weak_ptr<Transform> parent;
	std::vector<std::shared_ptr<Transform>> children;
	bool isDirty = false;
};
