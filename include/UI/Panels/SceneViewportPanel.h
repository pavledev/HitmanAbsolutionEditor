#pragma once

#include <d3d11.h>
#include "BasePanel.h"
#include <fstream>

class SceneViewportPanel : public BasePanel
{
public:
	SceneViewportPanel(const char* name, const char* icon);
	void Render() override;
	void OnReceiveMessage(const std::string& type, const std::string& content);
	void CreateShaderResourceView();
	static void SetBlendState(const ImDrawList* drawList, const ImDrawCmd* cmd);
	static void RestoreBlendState(const ImDrawList* drawList, const ImDrawCmd* cmd);
	static bool IsWindowHovered();
	static void OnMouseMove();

private:
	HWND engineHWND;
	HANDLE sharedHandle;
	ID3D11Texture2D* sharedTexture;
	ID3D11ShaderResourceView* shaderResourceView;
	float width;
	float height;

	inline static ID3D11BlendState* originalBlendState;
	inline static float originalBlendFactor[4];
	inline static unsigned int originalSampleMask;
};
