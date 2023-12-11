#pragma once

#include <d3d11.h>

#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImGuiRenderer
{
public:
	bool Setup(HWND hwnd);
	void Cleanup();
	void SetStyle();
	static void AddIconFont(float fontSize);
	void SetFont();
	ImFont* GetDefaultFont();
	ImFont* GetMiddleFont();
	ImFont* GetSmallFont();
	ImFont* GetBoldFont();
	ImFont* GetMiddleItalicFont();
	ImFont* GetConsolasRegularFont();
	ImFont* GetConsolasBoldFont();

private:
	ImFont* defaultFont;
	ImFont* middleFont;
	ImFont* smallFont;
	ImFont* boldFont;
	ImFont* middleitalicFont;
	ImFont* consolasRegularFont;
	ImFont* consolasBoldFont;
};
