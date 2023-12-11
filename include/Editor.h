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
	void SetupSceneDocumentLayout(std::shared_ptr<Document> document, const ImGuiID dockspaceID, const ImVec2 dockspaceSize);
	void SetupLibraryInfoDocumentLayout(std::shared_ptr<Document> document, const ImGuiID dockspaceID, const ImVec2 dockspaceSize);
	void SetupResourceDocumentLayout(std::shared_ptr<Document> document, const ImGuiID dockspaceID, const ImVec2 dockspaceSize);
	void SetupRenderPrimitiveDocumentLayout(std::shared_ptr<Document> document, const ImGuiID dockspaceID, const ImVec2 dockspaceSize);
	void TopLevelMenuBar(std::shared_ptr<Document> document, ImGuiID dockspace_id, ImVec2 dockspace_size);
	void UpdatePlatformTitleBar(std::shared_ptr<Document> document);
	void UpdateDocumentLocation(std::shared_ptr<Document> document, const ImGuiID toplevelDockspaceID);
	void UpdateDocumentContents(std::shared_ptr<Document> document);
	void RenderMenuBarForSceneDocument();

private:
	Editor();
	Editor(const Editor& other) = delete;
	Editor& operator=(const Editor& other) = delete;

	WNDCLASSEXW wc;
	HWND hwnd;
	inline static unsigned int resizeWidth;
	inline static unsigned int resizeHeight;

	std::shared_ptr<DirectXRenderer> directXRenderer;
	std::shared_ptr<ImGuiRenderer> imGuiRenderer;

	std::shared_ptr<SceneViewportPanel> sceneViewportPanel;
	std::shared_ptr<ResourceBrowserPanel> resourceBrowserPanel;

	std::vector<std::shared_ptr<Document>> documents;
	std::shared_ptr<Document> lastActiveDocument;
	ImGuiWindowClass topLevelEditorWindowClass;
	bool floatingToolsOnlyVisibleWhenParentIsFocused;
};
