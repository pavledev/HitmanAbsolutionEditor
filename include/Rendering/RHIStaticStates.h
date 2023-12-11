#pragma once

#include <memory>

#include "Sampler.h"
#include "RasterizerState.h"
#include "DepthStencilState.h"
#include "BlendState.h"

template <typename InitializerType, typename RHIRefType, typename RHIParamRefType>
class TStaticStateRHI
{
public:
	static void InitRHI()
	{
		stateRHI = InitializerType::CreateRHI();
	}

	static RHIParamRefType GetRHI()
	{
		return stateRHI.get();
	}

private:
	inline static RHIRefType stateRHI;
};

template <
	Sampler::Filter filter = Sampler::Filter::Point,
	Sampler::AddressMode addressU = Sampler::AddressMode::Clamp,
	Sampler::AddressMode addressV = Sampler::AddressMode::Clamp,
	Sampler::AddressMode addressW = Sampler::AddressMode::Clamp,
	float mipLodBias = 0.f,
	unsigned int maxAnisotropy = D3D11_MAX_MAXANISOTROPY,
	unsigned int borderColor = 0,
	Sampler::CompareFunction compareFunction = Sampler::CompareFunction::Never>
class TStaticSamplerState : public TStaticStateRHI<TStaticSamplerState<filter, addressU, addressV, addressW, mipLodBias, maxAnisotropy, borderColor, compareFunction>, std::shared_ptr<Sampler>, Sampler*>
{
public:
	static std::shared_ptr<Sampler> CreateRHI()
	{
		return std::make_shared<Sampler>(filter, addressU, addressV, addressW, mipLodBias, maxAnisotropy, borderColor, compareFunction);
	}
};

template <
	RasterizerState::FillMode fillMode = RasterizerState::FillMode::Solid,
	RasterizerState::CullMode cullMode = RasterizerState::CullMode::None,
	bool enableLineAA = false,
	bool enableMSAA = false,
	RasterizerState::DepthClipMode depthClipMode = RasterizerState::DepthClipMode::DepthClip>
class TStaticRasterizerState : public TStaticStateRHI<TStaticRasterizerState<fillMode, cullMode, enableLineAA>, std::shared_ptr<RasterizerState>, RasterizerState*>
{
public:
	static std::shared_ptr<RasterizerState> CreateRHI()
	{
		return std::make_shared<RasterizerState>(fillMode, cullMode, 0.0f, 0.0f, depthClipMode, enableMSAA, enableLineAA);
	}
};

template <
	bool enableDepthWrite = true,
	DepthStencilState::CompareFunction depthTest = DepthStencilState::CompareFunction::DepthNearOrEqual,
	bool enableFrontFaceStencil = false,
	DepthStencilState::CompareFunction frontFaceStencilTest = DepthStencilState::CompareFunction::Always,
	DepthStencilState::StencilOp frontFaceStencilFailStencilOp = DepthStencilState::StencilOp::Keep,
	DepthStencilState::StencilOp frontFaceDepthFailStencilOp = DepthStencilState::StencilOp::Keep,
	DepthStencilState::StencilOp frontFacePassStencilOp = DepthStencilState::StencilOp::Keep,
	bool enableBackFaceStencil = false,
	DepthStencilState::CompareFunction backFaceStencilTest = DepthStencilState::CompareFunction::Always,
	DepthStencilState::StencilOp backFaceStencilFailStencilOp = DepthStencilState::StencilOp::Keep,
	DepthStencilState::StencilOp backFaceDepthFailStencilOp = DepthStencilState::StencilOp::Keep,
	DepthStencilState::StencilOp backFacePassStencilOp = DepthStencilState::StencilOp::Keep,
	unsigned char stencilReadMask = 0xFF,
	unsigned char stencilWriteMask = 0xFF
>
class TStaticDepthStencilState : public TStaticStateRHI<
	TStaticDepthStencilState<
	enableDepthWrite,
	depthTest,
	enableFrontFaceStencil,
	frontFaceStencilTest,
	frontFaceStencilFailStencilOp,
	frontFaceDepthFailStencilOp,
	frontFacePassStencilOp,
	enableBackFaceStencil,
	backFaceStencilTest,
	backFaceStencilFailStencilOp,
	backFaceDepthFailStencilOp,
	backFacePassStencilOp,
	stencilReadMask,
	stencilWriteMask
	>,
	std::shared_ptr<DepthStencilState>,
	DepthStencilState*
>
{
public:
	static std::shared_ptr<DepthStencilState> CreateRHI()
	{
		return std::make_shared<DepthStencilState>(enableDepthWrite,
			depthTest,
			enableFrontFaceStencil,
			frontFaceStencilTest,
			frontFaceStencilFailStencilOp,
			frontFaceDepthFailStencilOp,
			frontFacePassStencilOp,
			enableBackFaceStencil,
			backFaceStencilTest,
			backFaceStencilFailStencilOp,
			backFaceDepthFailStencilOp,
			backFacePassStencilOp,
			stencilReadMask,
			stencilWriteMask);
	}
};

/**
 * A static RHI blend state resource.
 * TStaticBlendStateRHI<...>::GetStaticState() will return a FBlendStateRHIRef to a blend state with the desired settings.
 * Should only be used from the rendering thread.
 *
 * Alpha blending happens on GPU's as:
 * FinalColor.rgb = SourceColor * ColorSrcBlend (ColorBlendOp) DestColor * ColorDestBlend;
 * if (BlendState->bSeparateAlphaBlendEnable)
 *		FinalColor.a = SourceAlpha * AlphaSrcBlend (AlphaBlendOp) DestAlpha * AlphaDestBlend;
 * else
 *		Alpha blended the same way as rgb
 *
 * Where source is the color coming from the pixel shader, and target is the color in the render target.
 *
 * So for example, TStaticBlendState<BlendState::BlendOperation::Add,BF_SourceAlpha,BF_InverseSourceAlpha,BlendState::BlendOperation::Add,BlendState::BlendFactor::Zero,BlendState::BlendFactor::One> produces:
 * FinalColor.rgb = SourceColor * SourceAlpha + DestColor * (1 - SourceAlpha);
 * FinalColor.a = SourceAlpha * 0 + DestAlpha * 1;
 */
template <
	BlendState::ColorWriteMask rt0ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt0ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt0ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt0ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt0AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt0AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt0AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt1ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt1ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt1ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt1ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt1AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt1AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt1AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt2ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt2ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt2ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt2ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt2AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt2AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt2AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt3ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt3ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt3ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt3ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt3AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt3AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt3AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt4ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt4ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt4ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt4ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt4AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt4AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt4AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt5ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt5ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt5ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt5ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt5AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt5AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt5AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt6ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt6ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt6ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt6ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt6AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt6AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt6AlphaDestBlend = BlendState::BlendFactor::Zero,
	BlendState::ColorWriteMask rt7ColorWriteMask = BlendState::ColorWriteMask::RGBA,
	BlendState::BlendOperation rt7ColorBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt7ColorSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt7ColorDestBlend = BlendState::BlendFactor::Zero,
	BlendState::BlendOperation rt7AlphaBlendOp = BlendState::BlendOperation::Add,
	BlendState::BlendFactor    rt7AlphaSrcBlend = BlendState::BlendFactor::One,
	BlendState::BlendFactor    rt7AlphaDestBlend = BlendState::BlendFactor::Zero,
	bool					   useAlphaToCoverage = false
>
class TStaticBlendState : public TStaticStateRHI<
	TStaticBlendState<
	rt0ColorWriteMask, rt0ColorBlendOp, rt0ColorSrcBlend, rt0ColorDestBlend, rt0AlphaBlendOp, rt0AlphaSrcBlend, rt0AlphaDestBlend,
	rt1ColorWriteMask, rt1ColorBlendOp, rt1ColorSrcBlend, rt1ColorDestBlend, rt1AlphaBlendOp, rt1AlphaSrcBlend, rt1AlphaDestBlend,
	rt2ColorWriteMask, rt2ColorBlendOp, rt2ColorSrcBlend, rt2ColorDestBlend, rt2AlphaBlendOp, rt2AlphaSrcBlend, rt2AlphaDestBlend,
	rt3ColorWriteMask, rt3ColorBlendOp, rt3ColorSrcBlend, rt3ColorDestBlend, rt3AlphaBlendOp, rt3AlphaSrcBlend, rt3AlphaDestBlend,
	rt4ColorWriteMask, rt4ColorBlendOp, rt4ColorSrcBlend, rt4ColorDestBlend, rt4AlphaBlendOp, rt4AlphaSrcBlend, rt4AlphaDestBlend,
	rt5ColorWriteMask, rt5ColorBlendOp, rt5ColorSrcBlend, rt5ColorDestBlend, rt5AlphaBlendOp, rt5AlphaSrcBlend, rt5AlphaDestBlend,
	rt6ColorWriteMask, rt6ColorBlendOp, rt6ColorSrcBlend, rt6ColorDestBlend, rt6AlphaBlendOp, rt6AlphaSrcBlend, rt6AlphaDestBlend,
	rt7ColorWriteMask, rt7ColorBlendOp, rt7ColorSrcBlend, rt7ColorDestBlend, rt7AlphaBlendOp, rt7AlphaSrcBlend, rt7AlphaDestBlend,
	useAlphaToCoverage
	>,
	std::shared_ptr<BlendState>,
	BlendState*
>
{
public:
	static std::shared_ptr<BlendState> CreateRHI()
	{
		std::array<BlendState::RenderTarget, 8> renderTargetBlendStates;

		renderTargetBlendStates[0] = BlendState::RenderTarget(rt0ColorWriteMask, rt0ColorBlendOp, rt0ColorSrcBlend, rt0ColorDestBlend, rt0AlphaBlendOp, rt0AlphaSrcBlend, rt0AlphaDestBlend);
		renderTargetBlendStates[1] = BlendState::RenderTarget(rt1ColorWriteMask, rt1ColorBlendOp, rt1ColorSrcBlend, rt1ColorDestBlend, rt1AlphaBlendOp, rt1AlphaSrcBlend, rt1AlphaDestBlend);
		renderTargetBlendStates[2] = BlendState::RenderTarget(rt2ColorWriteMask, rt2ColorBlendOp, rt2ColorSrcBlend, rt2ColorDestBlend, rt2AlphaBlendOp, rt2AlphaSrcBlend, rt2AlphaDestBlend);
		renderTargetBlendStates[3] = BlendState::RenderTarget(rt3ColorWriteMask, rt3ColorBlendOp, rt3ColorSrcBlend, rt3ColorDestBlend, rt3AlphaBlendOp, rt3AlphaSrcBlend, rt3AlphaDestBlend);
		renderTargetBlendStates[4] = BlendState::RenderTarget(rt4ColorWriteMask, rt4ColorBlendOp, rt4ColorSrcBlend, rt4ColorDestBlend, rt4AlphaBlendOp, rt4AlphaSrcBlend, rt4AlphaDestBlend);
		renderTargetBlendStates[5] = BlendState::RenderTarget(rt5ColorWriteMask, rt5ColorBlendOp, rt5ColorSrcBlend, rt5ColorDestBlend, rt5AlphaBlendOp, rt5AlphaSrcBlend, rt5AlphaDestBlend);
		renderTargetBlendStates[6] = BlendState::RenderTarget(rt6ColorWriteMask, rt6ColorBlendOp, rt6ColorSrcBlend, rt6ColorDestBlend, rt6AlphaBlendOp, rt6AlphaSrcBlend, rt6AlphaDestBlend);
		renderTargetBlendStates[7] = BlendState::RenderTarget(rt7ColorWriteMask, rt7ColorBlendOp, rt7ColorSrcBlend, rt7ColorDestBlend, rt7AlphaBlendOp, rt7AlphaSrcBlend, rt7AlphaDestBlend);

		return std::make_shared<BlendState>(renderTargetBlendStates);
	}
};
