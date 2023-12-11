#include "Rendering/Scene/Transform.h"
#include "Utility/Math.h"
#include "Utility/UI.h"

Transform::Transform(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
	localMatrix = Matrix44::Identity;
	localScale = { 1.0f, 1.0f, 1.0f };
	worldMatrix = Matrix44::Identity;
	worldScale = { 1.0f, 1.0f, 1.0f };
	isDirty = true;
}

Transform::Transform(const Matrix44& matrix)
{
	localPosition = matrix.GetTranslation();
	localRotation = matrix.GetRotation();
	localScale = matrix.GetScale();
	isDirty = false;

	UpdateTransform();
}

const Vector3& Transform::GetLocalPosition() const
{
	return localPosition;
}

const Quaternion& Transform::GetLocalRotation() const
{
	return localRotation;
}

const Vector3& Transform::GetLocalScale() const
{
	return localScale;
}

Vector3& Transform::GetLocalPosition()
{
	return localPosition;
}

Quaternion& Transform::GetLocalRotation()
{
	return localRotation;
}

Vector3& Transform::GetLocalScale()
{
	return localScale;
}

const Vector3& Transform::GetWorldPosition() const
{
	return worldPosition;
}

const Quaternion& Transform::GetWorldRotation() const
{
	return worldRotation;
}

const Vector3& Transform::GetWorldScale() const
{
	return worldScale;
}

Vector3& Transform::GetWorldPosition()
{
	return worldPosition;
}

Quaternion& Transform::GetWorldRotation()
{
	return worldRotation;
}

Vector3& Transform::GetWorldScale()
{
	return worldScale;
}

const Matrix44& Transform::GetLocalMatrix() const
{
	return localMatrix;
}

Matrix44& Transform::GetLocalMatrix()
{
	return localMatrix;
}

const Matrix44& Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

Matrix44& Transform::GetWorldMatrix()
{
	return worldMatrix;
}

const Vector3 Transform::GetRight() const
{
	return localRotation * Vector3::Right;
}

const Vector3 Transform::GetUp() const
{
	return localRotation * Vector3::Up;
}

const Vector3 Transform::GetForward() const
{
	return localRotation * Vector3::Forward;
}

void Transform::SetLocalPosition(const Vector3& localPosition)
{
	if (this->localPosition == localPosition)
	{
		return;
	}

	this->localPosition = localPosition;

	UpdateTransform();
}

void Transform::SetLocalRotation(const Quaternion& localRotation)
{
	if (this->localRotation == localRotation)
	{
		return;
	}

	this->localRotation = localRotation;

	UpdateTransform();
}

void Transform::SetLocalScale(const Vector3& localScale)
{
	if (this->localScale == localScale)
	{
		return;
	}

	this->localScale = localScale;

	UpdateTransform();
}

void Transform::SetWorldPosition(const Vector3& worldPosition)
{
	if (this->worldPosition == worldPosition)
	{
		return;
	}

	Transform* parent = this->parent.lock().get();

	if (parent)
	{
		SetLocalPosition(worldPosition * parent->worldMatrix.Inverted());
	}
	else
	{
		SetLocalPosition(worldPosition);
	}
}

void Transform::SetWorldRotation(const Quaternion& worldRotation)
{
	if (this->worldRotation == worldRotation)
	{
		return;
	}

	Transform* parent = this->parent.lock().get();

	if (parent)
	{
		SetLocalRotation(worldRotation * parent->worldRotation.Inverse());
	}
	else
	{
		SetLocalRotation(worldRotation);
	}
}

void Transform::SetWorldScale(const Vector3& worldScale)
{
	if (this->worldScale == worldScale)
	{
		return;
	}

	Transform* parent = this->parent.lock().get();

	if (parent)
	{
		SetLocalScale(worldScale / parent->worldScale);
	}
	else
	{
		SetLocalScale(worldScale);
	}
}

void Transform::Tick()
{
	/*if (!isDirty)
	{
		return;
	}

	UpdateTransform();

	isDirty = false;*/
}

void Transform::UpdateTransform()
{
	localMatrix = Matrix44(localPosition, localRotation, localScale);

	Transform* parent = this->parent.lock().get();

	if (parent)
	{
		worldMatrix = localMatrix * parent->worldMatrix;
	}
	else
	{
		worldMatrix = localMatrix;
	}

	worldPosition = worldMatrix.GetTranslation();
	worldRotation = worldMatrix.GetRotation();
	worldScale = worldMatrix.GetScale();

	for (std::shared_ptr<Transform> child : children)
	{
		child->UpdateTransform();
	}
}

void Transform::SetParent(std::weak_ptr<Transform> parent)
{
	this->parent = parent;
	isDirty = true;
}

void Transform::AddChild(std::shared_ptr<Transform> child)
{
	auto iterator = find(children.begin(), children.end(), child);

	if (iterator == children.end())
	{
		children.push_back(child);
	}
}

void Transform::Render()
{
}

void Transform::RenderProperties()
{
	static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	/*if (ImGui::TreeNodeEx("World Transform", treeNodeFlags))
	{
		UI::BeginProperties();

		Vector3 worldPosition = GetWorldPosition();
		Vector3 worldRotation = GetWorldRotation().ToEulerAngles();
		Vector3 worldScale = GetWorldScale();

		UI::RenderVector3Control("Position", worldPosition);
		UI::RenderVector3Control("Rotation", worldRotation);
		UI::RenderVector3Control("Scale", worldScale, nullptr, 1.0f);

		SetWorldPosition(worldPosition);
		SetWorldRotation(Quaternion::FromEulerAngles(worldRotation));
		SetWorldScale(worldScale);

		UI::EndProperties();

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Local Transform", treeNodeFlags))
	{
		UI::BeginProperties();

		Vector3 localPosition = GetLocalPosition();
		Vector3 localRotation = GetLocalRotation().ToEulerAngles();
		Vector3 localScale = GetLocalScale();

		UI::RenderVector3Control("Position", localPosition);
		UI::RenderVector3Control("Rotation", localRotation);
		UI::RenderVector3Control("Scale", localScale, nullptr, 1.0f);

		Vector3 localRotation2 = Math::ConvertDegressToRadians(localRotation);

		SetLocalPosition(localPosition);
		SetLocalRotation(Quaternion::FromEulerAngles(localRotation));
		SetLocalScale(localScale);

		UI::EndProperties();

		ImGui::TreePop();
	}*/
}
