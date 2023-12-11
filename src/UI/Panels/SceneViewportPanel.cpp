#include "imgui_internal.h"

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/SceneViewportPanel.h"
#include "Utility/StringUtility.h"
#include "Utility/UI.h"
#include "Rendering/DirectXRenderer.h"
#include "Connection/PipeClient.h"
#include "Logger.h"
#include "Connection/SharedMemoryClient.h"
#include "Editor.h"

SceneViewportPanel::SceneViewportPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
	engineHWND = nullptr;
	sharedHandle = nullptr;
	sharedTexture = nullptr;
	shaderResourceView = nullptr;
	width = 0;
	height = 0;
	originalBlendState = nullptr;
}

void SceneViewportPanel::Render()
{
	Begin();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	if (shaderResourceView)
	{
		ImDrawList* drawList = ImGui::GetWindowDrawList();

		drawList->AddCallback(SetBlendState, nullptr);

		ImGui::Image(shaderResourceView, ImVec2(width, height));

		drawList->AddCallback(RestoreBlendState, nullptr);
	}

	SharedMemoryClient::Data* data = SharedMemoryClient::GetInstance().GetData();

	if (data)
	{
		data->sceneViewPortHasFocus = ImGui::IsWindowHovered();
	}

	/*ImVec2 vMin = ImGui::GetWindowContentRegionMin();
	ImVec2 vMax = ImGui::GetWindowContentRegionMax();

	vMin.x += ImGui::GetWindowPos().x;
	vMin.y += ImGui::GetWindowPos().y;
	vMax.x += ImGui::GetWindowPos().x;
	vMax.y += ImGui::GetWindowPos().y;

	ImGui::GetForegroundDrawList()->AddRect(vMin, vMax, IM_COL32(255, 255, 0, 255));

	ImGui::Text(std::format("X:{}, Y:{}", ImGui::GetWindowSize().x, ImGui::GetWindowSize().y).c_str());
	ImGui::Text(std::format("vMin X:{}, vMin Y:{}", vMin.x, vMin.y).c_str());
	ImGui::Text(std::format("vMax X:{}, vMax Y:{}", vMax.x, vMax.y).c_str());*/

	//ImGui::PopStyleVar();

	ImGui::PopStyleVar(3);
	End();
}

void SceneViewportPanel::OnReceiveMessage(const std::string& type, const std::string& content)
{
	if (type == "handle")
	{
		engineHWND = FindWindowA(nullptr, "engine");
		sharedHandle = reinterpret_cast<HANDLE>(std::stoul(content, nullptr, 16));

		CreateShaderResourceView();

		ImGui::SetWindowSize(id.c_str(), ImVec2(width, height)); //ADD PANEL ON THE RIGHT
	}
}

void SceneViewportPanel::CreateShaderResourceView()
{
	Logger& logger = Logger::GetInstance();
	HRESULT result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->OpenSharedResource(sharedHandle, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&sharedTexture));

	if (result == S_OK)
	{
		logger.Log(Logger::Level::Info, "Successfully opened shared resource.");
	}
	else
	{
		logger.Log(Logger::Level::Error, "Failed to open shared resource.");

		return;
	}

	D3D11_TEXTURE2D_DESC textureDesc;

	sharedTexture->GetDesc(&textureDesc);

	width = static_cast<float>(textureDesc.Width);
	height = static_cast<float>(textureDesc.Height);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	ZeroMemory(&srvDesc, sizeof(srvDesc));

	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = textureDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;

	result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreateShaderResourceView(sharedTexture, &srvDesc, &shaderResourceView);

	if (result == S_OK)
	{
		logger.Log(Logger::Level::Info, "Successfully created shader resource view.");
	}
	else
	{
		logger.Log(Logger::Level::Error, "Failed to create shader resource view.");
	}
}

void SceneViewportPanel::SetBlendState(const ImDrawList* drawList, const ImDrawCmd* cmd)
{
	//std::pair<ID3D11Device*, ID3D11DeviceContext*>* userCallbackData = static_cast<std::pair<ID3D11Device*, ID3D11DeviceContext*>*>(cmd->UserCallbackData);
	//ID3D11Device* d3dDevice = userCallbackData->first;
	//ID3D11DeviceContext* d3dDeviceContext = userCallbackData->second;

	D3D11_BLEND_DESC blendDesc;
	ID3D11BlendState* blendState;

	ZeroMemory(&blendDesc, sizeof(blendDesc));

	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE;

	Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->OMGetBlendState(&originalBlendState, originalBlendFactor, &originalSampleMask);
	Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreateBlendState(&blendDesc, &blendState);
	Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->OMSetBlendState(blendState, 0, 0xFFFFFFFF);
}

void SceneViewportPanel::RestoreBlendState(const ImDrawList* drawList, const ImDrawCmd* cmd)
{
	Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->OMSetBlendState(originalBlendState, originalBlendFactor, originalSampleMask);
}

bool SceneViewportPanel::IsWindowHovered()
{
	static std::string windowName = "Scene Viewport";
	static std::string name = std::format(" {} {}\t\t###{}{}", ICON_MDI_MONITOR, windowName, 1, windowName);
	static ImGuiID id = ImHashStr(name.c_str());

	return UI::IsWindowHovered(id);
}

void SceneViewportPanel::OnMouseMove()
{
	/*static bool set = true;

	if (set)
	{
		set = false;

		if (!GImGui)
		{
			return;
		}

		ImGuiContext& g = *GImGui;

		ImVec2 size = g.HoveredWindow->Size;

		g.HoveredWindow->Size = ImVec2(1280, 720);

		std::string s = "s";
	}*/

	if (!PipeClient::GetInstance().IsConnectedWithEngine())
	{
		return;
	}

	SharedMemoryClient::Data* data = SharedMemoryClient::GetInstance().GetData();

	ImGuiContext& g = *GImGui;
	ImVec2 mousePosition = ImGui::GetMousePos();
	ImVec2 windowContentRegionMin = UI::GetWindowContentRegionMin(g.HoveredWindow);

	windowContentRegionMin.x += g.HoveredWindow->Pos.x;
	windowContentRegionMin.y += g.HoveredWindow->Pos.y;

	ImVec2 mousePositionRelativeToWindow = ImVec2(mousePosition.x - windowContentRegionMin.x, mousePosition.y - windowContentRegionMin.y);

	data->mouseCoordinates = mousePositionRelativeToWindow;
}
