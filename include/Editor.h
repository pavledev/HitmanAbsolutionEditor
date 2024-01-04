#pragma once

#include "Rendering/DirectXRenderer.h"
#include "Rendering/ImGuiRenderer.h"
#include "UI/Panels/SceneHierarchyPanel.h"
#include "UI/Panels/SceneViewportPanel.h"
#include "UI/Panels/ConsolePanel.h"
#include "UI/Panels/ResourceBrowserPanel.h"
#include "UI/Panels/LibraryPanel.h"
#include "Settings.h"
#include "UI/Documents/Document.h"

class Editor
{
public:
	~Editor();
	static Editor& GetInstance();
	bool Setup();
	void Start();
	void Render();
	void RenderContent();
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void LoadRegistries();

	std::shared_ptr<DirectXRenderer> GetDirectXRenderer() const;
	std::shared_ptr<ImGuiRenderer> GetImGuiRenderer() const;

	std::vector<std::shared_ptr<Document>>& GetDocuments();
	std::shared_ptr<Document> GetLastActiveDocument() const;

	void SetupLayout(std::shared_ptr<Document> document, const ImGuiID dockspaceID, const ImVec2 dockspaceSize);
	void UpdateDocumentLocation(std::shared_ptr<Document> document, const ImGuiID toplevelDockspaceID);
	void UpdateDocumentContents(std::shared_ptr<Document> document);

private:
	Editor();
	Editor(const Editor& other) = delete;
	Editor& operator=(const Editor& other) = delete;

	WNDCLASSEXA wc;
	HWND hwnd;
	inline static unsigned int resizeWidth;
	inline static unsigned int resizeHeight;

	std::shared_ptr<DirectXRenderer> directXRenderer;
	std::shared_ptr<ImGuiRenderer> imGuiRenderer;

	std::vector<std::shared_ptr<Document>> documents;
	std::shared_ptr<Document> lastActiveDocument;
	ImGuiWindowClass topLevelEditorWindowClass;
	bool floatingToolsOnlyVisibleWhenParentIsFocused;
};
