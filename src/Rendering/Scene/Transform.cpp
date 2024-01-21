#include "Rendering/Scene/Transform.h"
#include "Utility/Math.h"
#include "Utility/UI.h"

Transform::Transform(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
	localMatrix = Matrix::Identity;
	localScale = { 1.0f, 1.0f, 1.0f };
	worldMatrix = Matrix::Identity;
	worldScale = { 1.0f, 1.0f, 1.0f };
	isDirty = true;
}

Transform::Transform(const Matrix& matrix)
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

const Matrix& Transform::GetLocalMatrix() const
{
	return localMatrix;
}

Matrix& Transform::GetLocalMatrix()
{
	return localMatrix;
}

const Matrix& Transform::GetWorldMatrix() const
{
	return worldMatrix;
}

Matrix& Transform::GetWorldMatrix()
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
	localMatrix = Matrix(localPosition, localRotation, localScale);

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
	static std::vector<UI::TableColumn> tableColumns;

	if (tableColumns.empty())
	{
		tableColumns.push_back({ "Name" , 0, 1.f });
		tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	}

	if (ImGui::TreeNodeEx("World Transform", treeNodeFlags))
	{
		UI::BeginProperties("WorldTransform", tableColumns, false);

		Vector3 worldPosition = GetWorldPosition();
		Vector3 worldRotation = GetWorldRotation().ToEulerAngles();
		Vector3 worldScale = GetWorldScale();

		if (UI::Vector3Property("Position", worldPosition))
		{
			SetWorldPosition(worldPosition);
		}

		if (UI::Vector3Property("Rotation", worldRotation))
		{
			SetWorldRotation(Quaternion::FromEulerAngles(worldRotation));
		}

		if (UI::Vector3Property("Scale", worldScale, nullptr, 1.0f))
		{
			SetWorldScale(worldScale);
		}

		UI::EndProperties();

		ImGui::TreePop();
	}

	if (ImGui::TreeNodeEx("Local Transform", treeNodeFlags))
	{
		UI::BeginProperties("LocalTransform", tableColumns, false);

		Vector3 localPosition = GetLocalPosition();
		Vector3 localRotation = GetLocalRotation().ToEulerAngles();
		Vector3 localScale = GetLocalScale();

		if (UI::Vector3Property("Position", localPosition))
		{
			SetLocalPosition(localPosition);
		}

		if (UI::Vector3Property("Rotation", localRotation))
		{
			SetLocalRotation(Quaternion::FromEulerAngles(localRotation));
		}

		if (UI::Vector3Property("Scale", localScale, nullptr, 1.0f))
		{
			SetLocalScale(localScale);
		}

		UI::EndProperties();

		ImGui::TreePop();
	}
}
