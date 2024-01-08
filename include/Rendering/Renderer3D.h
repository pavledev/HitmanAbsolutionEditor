#pragma once

#include <map>

#include "imgui.h"

#include "Rendering/Scene/Camera.h"
#include "Rendering/RenderTexture.h"
#include "Rendering/ConstantBuffer.h"
#include "Rendering/Shader.h"
#include "Rendering/ConstantBuffers.h"
#include "Rendering/Scene/Entity.h"
#include "Rendering/CommandList.h"
#include "Rendering/Scene/TransformGizmo.h"

class Renderer3D
{
public:
	enum Shaders
	{
		OutlineVertex,
		OutlinePixel,
		OutlineCompute,
		BlurGaussianCompute,
		SimpleElementVertex,
		SimpleElementPixel,
		SolidVertex,
		SolidPixel,
		PhongVertex,
		PhongPixel,
		PhongDiffuseVertex,
		PhongDiffusePixel,
		PhongDiffuseMaskSpecularVertex,
		PhongDiffuseMaskSpecularPixel,
		PhongDiffuseMaskSpecularNormalVertex,
		PhongDiffuseMaskSpecularNormalPixel,
		PhongDiffuseNormalVertex,
		PhongDiffuseNormalPixel,
		PhongDiffuseSpecularVertex,
		PhongDiffuseSpecularPixel,
		PhongDiffuseSpecularNormalVertex,
		PhongDiffuseSpecularNormalPixel,
		ShaderCount
	};

	enum RendererTextures
	{
		FrameOutput,
		Depth,
		Blur,
		Outline,
		WhiteTexture
	};

	using SelectedEntityCallback = std::function<void(std::shared_ptr<Entity> selectedEntity)>;

	Renderer3D() = default;
	void Initialize();
	std::shared_ptr<Camera> GetCamera();
	std::shared_ptr<Entity> GetRootEntity();
	void SetCamera(std::shared_ptr<Camera> camera);
	void SetRootEntity(std::shared_ptr<Entity> rootEntity);
	void CreateRenderTextures(const float width, const float height);
	void CreateConstantBuffers();
	static void CreateShaders();
	static void CreateDepthStencilStates();
	static void CreateRasterizerStates();
	static void CreateBlendStates();
	static void CreateSamplers();
	void SetGlobalShaderResources();
	void OnViewportSizeChanged(const float width, const float height);
	void Render();
	void PostProcessPass();
	void OutlinePass(RenderTexture* outputTexture);
	void GaussianBlurPass(RenderTexture* inputTexture);
	void UpdateUberConstantBuffer();
	void UpdateLightConstantBuffer();
	UberConstantBuffer& GetUberConstantBufferCpu();
	PointLightConstantBuffer& GetPointLightConstantBufferCpu();
	static std::shared_ptr<Shader> GetShader(const Shaders shader);
	void SetSelectedEntityCallback(SelectedEntityCallback selectedEntityCallback);

private:
	bool isRendererSetup;
	std::shared_ptr<Camera> camera;
	ID3D11ShaderResourceView* shaderResourceView;
	std::shared_ptr<Entity> rootEntity;
	TransformGizmo transformGizmo;

	UberConstantBuffer uberConstantBufferCpu;
	PointLightConstantBuffer pointLightConstantBufferCpu;
	std::shared_ptr<ConstantBuffer> uberConstantBufferGpu;
	std::shared_ptr<ConstantBuffer> pointLightConstantBufferGpu;
	std::array<std::shared_ptr<RenderTexture>, 5> renderTargets;
	inline static std::array<std::shared_ptr<Shader>, Shaders::ShaderCount> shaders;
	const float threadGroupCount = 8.0f;
	SelectedEntityCallback selectedEntityCallback;
};
