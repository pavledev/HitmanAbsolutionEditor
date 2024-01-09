#include "Utility/UI.h"
#include "Rendering/Scene/PointLight.h"
#include "Rendering/DirectXRenderer.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/RHIStaticStates.h"
#include "Rendering/Scene/Geometry.h"
#include "Editor.h"
#include "Rendering/Renderer3D.h"

PointLight::PointLight(const char* name, const char* icon, std::weak_ptr<Entity> entity) : Component(name, icon, entity)
{
	//solidSphere = new SolidSphere(0.1f);
	diffuseIntensity = 1.0f;
	diffuseColor = { 1.0f, 1.0f, 1.0f };
	ambientColor = { 0.05f, 0.05f, 0.05f };
	constantAttenuation = 1.0f;
	linearAttenuation = 0.025f;
	quadraticAttenuation = 0.0030f;

	GetTransform()->SetWorldScale(0.1f);
}

PointLight::~PointLight()
{
	//delete solidSphere;
}

void PointLight::Initialize()
{
	std::vector<VertexPosition> sphereVertices;
	std::vector<unsigned short> sphereIndices;

	Geometry::CreateSphere(sphereVertices, sphereIndices);

	mesh->Initialize(sphereVertices, sphereIndices, Renderer3D::Shaders::SolidVertex, Renderer3D::Shaders::SolidPixel, Vector3(0.721f, 0.709f, 0.709f));
	mesh->GetTransform()->SetLocalScale(Vector3(0.1f, 0.1f, 0.1f));
}

void PointLight::SetMesh(std::shared_ptr<Mesh> mesh)
{
	this->mesh = mesh;
}

void PointLight::Render()
{
	PointLightConstantBuffer& pointLightConstantBufferCpu = renderer3D->GetPointLightConstantBufferCpu();
	Matrix worldView = GetTransform()->GetWorldMatrix() * renderer3D->GetCamera()->GetView();

	pointLightConstantBufferCpu.transform = worldView * renderer3D->GetCamera()->GetProjection();
	pointLightConstantBufferCpu.viewLightPos = GetTransform()->GetWorldPosition();
	pointLightConstantBufferCpu.ambientColor = ambientColor;
	pointLightConstantBufferCpu.diffuseColor = diffuseColor;
	pointLightConstantBufferCpu.diffuseIntensity = diffuseIntensity;
	pointLightConstantBufferCpu.attConst = constantAttenuation;
	pointLightConstantBufferCpu.attLin = linearAttenuation;
	pointLightConstantBufferCpu.attQuad = quadraticAttenuation;

	renderer3D->UpdateLightConstantBuffer();

	CommandList& commandList = Editor::GetInstance().GetDirectXRenderer()->GetCommandList();
	static PipelineState pipelineState;

	pipelineState.vertexShader = mesh->GetVertexShader();
	pipelineState.pixelShader = mesh->GetPixelShader();
	pipelineState.rasterizerState = TStaticRasterizerState<RasterizerState::FillMode::Solid, RasterizerState::CullMode::Clockwise>::GetRHI();
	pipelineState.blendState = TStaticBlendState<>::GetRHI();
	pipelineState.depthStencilState = TStaticDepthStencilState<true, DepthStencilState::CompareFunction::DepthNearOrEqual, false, DepthStencilState::CompareFunction::Always, DepthStencilState::StencilOp::Keep, DepthStencilState::StencilOp::Keep, DepthStencilState::StencilOp::Keep, true>::GetRHI();
	pipelineState.primitiveType = PrimitiveType::TriangleList;

	commandList.SetPipelineState(pipelineState, renderer3D.get(), false);
	commandList.SetVertexBuffer(mesh->GetVertexBuffer());
	commandList.SetIndexBuffer(mesh->GetIndexBuffer());

	commandList.DrawIndexedPrimitive(mesh->GetIndexBuffer(), 0, 0, 0, 1);
}

void PointLight::RenderProperties()
{
	static constexpr ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed
		| ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

	//if (ImGui::TreeNodeEx("Intensity/Color", treeNodeFlags))
	//{
	//	UI::BeginProperties();

	//	UI::Property("Diffuse Intensity", diffuseIntensity, 0.01f, 2.0f, nullptr, 0.1f, "%.2f");
	//	//UI::PropertyVector("Diffuse Color", diffuseColor, true, false);
	//	//UI::PropertyVector("Ambient Color", ambientColor, true, false);

	//	UI::EndProperties();

	//	ImGui::TreePop();
	//}

	//if (ImGui::TreeNodeEx("Attenuation", treeNodeFlags))
	//{
	//	UI::BeginProperties();

	//	UI::Property("Constant", constantAttenuation, 0.05f, 10.0f, nullptr, 0.1f, "%.2f");
	//	UI::Property("Linear", linearAttenuation, 0.0001f, 4.0f, nullptr, 0.1f, "%.4f");
	//	UI::Property("Quadratic", quadraticAttenuation, 0.0000001f, 10.0f, nullptr, 0.1f, "%.7f");

	//	UI::EndProperties();

	//	ImGui::TreePop();
	//}
}

void PointLight::SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D)
{
	this->renderer3D = renderer3D;
}
