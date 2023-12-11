#pragma once

/**
 * A vertex shader for rendering a texture on a simple element.
 */
class FSimpleElementVS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSimpleElementVS, Global);
public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return true; }

	FSimpleElementVS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	FSimpleElementVS() {}

	void SetParameters(FRHICommandList& RHICmdList, const FMatrix& WorldToClipMatrix);
	void SetParameters(FRHICommandList& RHICmdList, const FRelativeViewMatrices& Matrices);

	//virtual bool Serialize(FArchive& Ar) override;

	static void ModifyCompilationEnvironment(const FGlobalShaderPermutationParameters& Parameters, FShaderCompilerEnvironment& OutEnvironment);

private:
	LAYOUT_FIELD(FShaderParameter, RelativeTransform);
	LAYOUT_FIELD(FShaderParameter, TransformTilePosition);
};

/**
 * Simple pixel shader that just reads from a texture
 * This is used for resolves and needs to be as efficient as possible
 */
class FSimpleElementPS : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FSimpleElementPS, Global);
public:

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters) { return true; }

	FSimpleElementPS(const ShaderMetaType::CompiledShaderInitializerType& Initializer);
	FSimpleElementPS() {}

	/**
	 * Sets parameters for compositing editor primitives
	 *
	 * @param View			SceneView for view constants when compositing
	 * @param DepthTexture	Depth texture to read from when depth testing for compositing.  If not set no compositing will occur
	 */
	void SetEditorCompositingParameters(FRHICommandList& RHICmdList, const FSceneView* View);

	void SetParameters(FRHICommandList& RHICmdList, const FTexture* TextureValue);

	//virtual bool Serialize(FArchive& Ar) override;

private:
	LAYOUT_FIELD(FShaderResourceParameter, InTexture)
		LAYOUT_FIELD(FShaderResourceParameter, InTextureSampler)
		LAYOUT_FIELD(FShaderParameter, TextureComponentReplicate)
		LAYOUT_FIELD(FShaderParameter, TextureComponentReplicateAlpha)
		LAYOUT_FIELD(FShaderParameter, EditorCompositeDepthTestParameter)
		LAYOUT_FIELD(FShaderParameter, ScreenToPixel)
};
