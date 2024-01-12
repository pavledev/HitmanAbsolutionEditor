#include "Utility/UI.h"
#include "Rendering/Scene/DirectionalLight.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/RHIStaticStates.h"
#include "Rendering/Scene/Geometry.h"
#include "Editor.h"
#include "Rendering/Renderer3D.h"
#include "Rendering/Scene/LightType.h"

DirectionalLight::DirectionalLight(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
	lightColor = { 0.839216f, 0.839216f, 0.839216f, 1.f };
}

void DirectionalLight::Render()
{
	LightConstantBuffer& lightConstantBufferCpu = renderer3D->GetLightConstantBufferCpu();
	Matrix worldView = GetTransform()->GetWorldMatrix() * renderer3D->GetCamera()->GetView();
	const Vector3 lookDirection = renderer3D->GetCamera()->GetTransform()->GetForward();

	lightDirection = lookDirection;
	lightConstantBufferCpu.lights[0].lightType = static_cast<int>(LightType::Directional);
	lightConstantBufferCpu.lights[0].lightDirection = -Math::TransformNormal(lightDirection, GetTransform()->GetWorldMatrix()).Normalized();
	lightConstantBufferCpu.lights[0].lightColor = lightColor;

	renderer3D->UpdateLightConstantBuffer();
}

void DirectionalLight::RenderProperties()
{
	static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	static std::vector<UI::TableColumn> tableColumns;

	if (tableColumns.empty())
	{
		tableColumns.push_back({ "Name" , 0, 1.f });
		tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	}

	if (ImGui::TreeNodeEx("Directional Light", treeNodeFlags))
	{
		UI::BeginProperties("DirectionalLightProperties", tableColumns, false);

		UI::ColorRGBAProperty("Light Color", lightColor);

		UI::EndProperties();

		ImGui::TreePop();
	}
}

void DirectionalLight::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
	this->renderer3D = renderer3D;
}
