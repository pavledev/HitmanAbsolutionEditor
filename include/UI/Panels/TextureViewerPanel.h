#pragma once

#include "BasePanel.h"
#include "Resources/Texture.h"
#include "Rendering/Shader.h"
#include "Rendering/ConstantBuffer.h"

class TextureViewerPanel : public BasePanel
{
public:
	enum class ZoomMode : unsigned char
	{
		Custom,
		Fit,
		Fill
	};

	TextureViewerPanel(const char* name, const char* icon, std::shared_ptr<Texture> textureResource, float* sliderValue);
	~TextureViewerPanel() override;
	void Render() override;
	const ZoomMode GetZoomMode() const;
	void SetZoomMode(const ZoomMode zoomMode);
	void SetTextureWidth(const unsigned int textureWidth);
	void SetTextureHeight(const unsigned int textureWidth);
	void SetTexture(ID3D11Resource* texture);
	void SetTextureView(ID3D11ShaderResourceView* textureView);
	void OnResourceLoaded();
	void OnZoomSliderValueChange();
	void SetIsRedChannelEnabled(const bool isRedChannelEnabled);
	void SetIsGreenChannelEnabled(const bool isGreenChannelEnabled);
	void SetIsBlueChannelEnabled(const bool isBlueChannelEnabled);
	void SetIsAlphaChannelEnabled(const bool isAlphaChannelEnabled);
	double CalculateDisplayedZoomLevel() const;

private:
	struct ChannelControlConstantBuffer
	{
		BOOL isRedChannelEnabled;
		BOOL isGreenChannelEnabled;
		BOOL isBlueChannelEnabled;
		BOOL isAlphaChannelEnabled;
	};

	void RenderTexture();
	void CalculateTextureDimensions(unsigned int& width, unsigned int& height) const;
	void SetCustomZoomLevel(double zoomValue);
	void ZoomIn();
	void ZoomOut();
	void UpdateZoomSliderValue();

	static void SetRenderState(const ImDrawList* drawList, const ImDrawCmd* cmd);

	std::shared_ptr<Texture> textureResource;
	ID3D11Resource* texture;
	ID3D11ShaderResourceView* textureView;
	unsigned int textureWidth;
	unsigned int textureHeight;

	ImVec2 viewportSize;
	float* sliderValue;
	//The texture's zoom factor.
	double zoom;
	ZoomMode zoomMode;
	//maxZoom and minZoom should both be powers of two
	const double maxZoom = 16.0;
	const double minZoom = 1.0 / 64;
	//zoomFactor is multiplicative such that an integer number of steps will give a power of two zoom (50% or 200%)
	const int zoomFactorLogSteps = 8;
	const double zoomFactor = pow(2.0, 1.0 / zoomFactorLogSteps);

	bool isRedChannelEnabled;
	bool isGreenChannelEnabled;
	bool isBlueChannelEnabled;
	bool isAlphaChannelEnabled;

	std::shared_ptr<Shader> pixelShader;
	ChannelControlConstantBuffer channelControlConstantBufferCpu;
	std::shared_ptr<ConstantBuffer> channelControlConstantBufferGpu;
};
