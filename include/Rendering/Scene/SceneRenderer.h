#pragma once

#include <map>

#include "imgui.h"

#include "Camera.h"
#include "../RenderTexture.h"
#include "Resources/RenderPrimitive.h"
#include "../ConstantBuffer.h"
#include "../Shader.h"
#include "../ConstantBuffers.h"
#include "Entity.h"
#include "../CommandList.h"
#include "TransformGizmo.h"
//#include "../SkeletalDebugRendering.h"

class SceneRenderer
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

	SceneRenderer() = default;
	static std::shared_ptr<Camera> GetCamera();
	static std::shared_ptr<Entity> GetRootEntity();
	void Setup(const float width, const float height, std::shared_ptr<RenderPrimitive> renderPrimitive);
	void CreateRenderTextures(const float width, const float height);
	static void CreateConstantBuffers();
	void CreateShaders();
	static void CreateDepthStencilStates();
	static void CreateRasterizerStates();
	static void CreateBlendStates();
	static void CreateSamplers();
	static void SetGlobalShaderResources();
	void OnViewportSizeChanged(const float width, const float height);
	void Render();
	void PostProcessPass();
	void OutlinePass(RenderTexture* outputTexture);
	void GaussianBlurPass(RenderTexture* inputTexture);
	static void UpdateUberConstantBuffer();
	static void UpdateLightConstantBuffer();
	static UberConstantBuffer& GetUberConstantBufferCpu();
	static PointLightConstantBuffer& GetPointLightConstantBufferCpu();
	static std::shared_ptr<Shader> GetShader(const Shaders shader);

private:
	inline static std::shared_ptr<Camera> camera;
	ID3D11ShaderResourceView* shaderResourceView;
	std::shared_ptr<RenderPrimitive> renderPrimitive;
	inline static std::shared_ptr<Entity> rootEntity;
	TransformGizmo transformGizmo;

	inline static UberConstantBuffer uberConstantBufferCpu;
	inline static PointLightConstantBuffer pointLightConstantBufferCpu;
	inline static std::shared_ptr<ConstantBuffer> uberConstantBufferGpu;
	inline static std::shared_ptr<ConstantBuffer> pointLightConstantBufferGpu;
	std::array<std::shared_ptr<RenderTexture>, 5> renderTargets;
	inline static std::array<std::shared_ptr<Shader>, Shaders::ShaderCount> shaders;
	const float threadGroupCount = 8.0f;

public:
	//inline static SkeletalDebugRendering skeletalDebugRendering;
};
