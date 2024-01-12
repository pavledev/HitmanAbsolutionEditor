#include "Utility/UI.h"
#include "Rendering/Scene/AmbientLight.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/RHIStaticStates.h"
#include "Rendering/Scene/Geometry.h"
#include "Editor.h"
#include "Rendering/Renderer3D.h"
#include "Rendering/Scene/LightType.h"

AmbientLight::AmbientLight(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
	lightAmbient = { 0.211765f, 0.211765f, 0.211765f, 1.f };
}

void AmbientLight::Render()
{
	LightConstantBuffer& lightConstantBufferCpu = renderer3D->GetLightConstantBufferCpu();
	Matrix worldView = GetTransform()->GetWorldMatrix() * renderer3D->GetCamera()->GetView();

	lightConstantBufferCpu.lightAmbient = lightAmbient;
	lightConstantBufferCpu.lightCount = 1;

	renderer3D->UpdateLightConstantBuffer();
}

void AmbientLight::RenderProperties()
{
	static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;
	static std::vector<UI::TableColumn> tableColumns;

	if (tableColumns.empty())
	{
		tableColumns.push_back({ "Name" , 0, 1.f });
		tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	}

	if (ImGui::TreeNodeEx("Ambient Light", treeNodeFlags))
	{
		UI::BeginProperties("AmbientLightProperties", tableColumns, false);

		UI::ColorRGBAProperty("Light Ambient", lightAmbient);

		UI::EndProperties();

		ImGui::TreePop();
	}
}

void AmbientLight::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
	this->renderer3D = renderer3D;
}

