#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include <IconsMaterialDesignIcons.h>

#include "ImGuizmo.h"

#include "Editor.h"
#include "Utility/StringUtility.h"
#include "Connection/PipeClient.h"
#include "Connection/SharedMemoryClient.h"
#include "Utility/FileDialog.h"
#include "Registry/ResourceIDRegistry.h"
#include "Registry/TypeRegistry.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Registry/PropertyRegistry.h"
#include "Registry/EnumRegistry.h"
#include "Registry/TextListHashRegistry.h"
#include "Timer.h"
#include "UI/Panels/ResourceViewerPanel.h"
#include "UI/Panels/HeaderLibrariesSearchPanel.h"
#include "UI/Panels/HashMapSearchPanel.h"
#include "UI/Documents/SceneDocument.h"
#include "UI/Documents/LibraryInfoDocument.h"

Editor::Editor()
{
    hwnd = nullptr;
    lastActiveDocument = nullptr;
    floatingToolsOnlyVisibleWhenParentIsFocused = true;

    directXRenderer = std::make_shared<DirectXRenderer>();
    imGuiRenderer = std::make_shared<ImGuiRenderer>();

    // Unique identifier to match/filter top level editors with the top level dockspace. This could be a user enum.
    topLevelEditorWindowClass.ClassId = ImHashStr("TOPLEVEL_EDITOR", 0);
    topLevelEditorWindowClass.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
    topLevelEditorWindowClass.ViewportFlagsOverrideClear = ImGuiViewportFlags_NoDecoration | ImGuiViewportFlags_NoTaskBarIcon;
    topLevelEditorWindowClass.ParentViewportId = 0; // Top level window
    //topLevelEditorWindowClass.ParentViewportId = -1; // Top level window
    topLevelEditorWindowClass.DockingAllowUnclassed = false;
    topLevelEditorWindowClass.DockingAlwaysTabBar = true;
}

Editor::~Editor()
{
    imGuiRenderer->Cleanup();
    directXRenderer->CleanupD3DDevice();
    DestroyWindow(hwnd);
    UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

Editor& Editor::GetInstance()
{
    static Editor instance;

    return instance;
}

bool Editor::Setup()
{
    wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"Hitman Absolution Editor", nullptr };

    RegisterClassExW(&wc);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);
    hwnd = CreateWindowExW(0, wc.lpszClassName, L"Hitman Absolution Editor", WS_OVERLAPPEDWINDOW, 0, 0, width, height, nullptr, nullptr, wc.hInstance, nullptr);

    Logger& logger = Logger::GetInstance();

    if (!directXRenderer->Setup(hwnd, &wc))
    {
        logger.Log(Logger::Level::Error, "Failed to setup DirectX renderer!");

        return false;
    }

    ShowWindow(hwnd, SW_MAXIMIZE);
    UpdateWindow(hwnd);

    if (!imGuiRenderer->Setup(hwnd))
    {
        logger.Log(Logger::Level::Error, "Failed to setup ImGui renderer!");

        return false;
    }

    Timer::Initialize();

    std::shared_ptr<SceneDocument> sceneDocument = std::make_shared<SceneDocument>("Scene", ICON_MDI_TERRAIN, Document::Type::Scene);

    documents.push_back(sceneDocument);

    Settings& settings = Settings::GetInstance();

    settings.LoadSettings();

    std::thread thread(&Editor::LoadRegistries, this);

    thread.detach();

    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    if (FAILED(result))
    {
        logger.Log(Logger::Level::Error, "Failed to initialize COM!");

        return false;
    }

    Logger::GetInstance().Log(Logger::Level::Info, "Editor successfully set up.");

    return true;
}

void Editor::Start()
{
    Timer::SetFPSLimit();

    bool quit = false;

    while (true)
    {
        Timer::Tick();

        // Poll and handle messages (inputs, window resize, etc.)
        // See the WndProc() function below for our to dispatch events to the Win32 backend.
        MSG msg;

        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT)
            {
                quit = true;
            }
        }

        if (quit)
        {
            break;
        }

        if (resizeWidth != 0 && resizeHeight != 0)
        {
            directXRenderer->GetSwapChain()->Resize(resizeWidth, resizeHeight);

            resizeWidth = 0;
            resizeHeight = 0;
        }

        Render();
    }
}

void Editor::Render()
{
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    RenderContent();

    // Rendering
    ImGui::Render();

    directXRenderer->ClearBackBuffer();
    directXRenderer->SetBackBufferAsRenderTarget();
    
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    directXRenderer->GetSwapChain()->Present();
}

void Editor::RenderContent()
{
    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::SetNextWindowSize(viewport->Size);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("###DocumentRoot", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImGuiID rootDockspaceID = ImGui::GetID("RootDockspace");
    ImGui::DockSpace(rootDockspaceID, ImVec2(0, 0), ImGuiDockNodeFlags_NoCloseButton | ImGuiDockNodeFlags_NoSplit, &topLevelEditorWindowClass);

    for (size_t i = 0; i < documents.size(); ++i)
    {
        if (!*documents[i]->GetOpen())
        {
            continue;
        }

        const bool isLastFocusedDocument = lastActiveDocument == documents[i];

        if (isLastFocusedDocument)
        {
            documents[i]->RenderMenuBar();
        }

        UpdateDocumentLocation(documents[i], rootDockspaceID);
    }

    for (size_t i = 0; i < documents.size(); ++i)
    {
        std::shared_ptr<Document> document = documents[i];

        if (!*document->GetOpen())
        {
            if (lastActiveDocument == document)
            {
                lastActiveDocument = nullptr;
            }

            documents.erase(documents.begin() + i);

            i--;
        }
        else
        {
            UpdateDocumentContents(document);
        }
    }

    Settings& settings = Settings::GetInstance();

    if (settings.GetRuntimeFolderPath().empty())
    {
        ImGui::OpenPopup("Runtime Folder Path");
    }

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();

    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Runtime Folder Path", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static std::string runtimeFolderPath;

        ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());
        ImGui::TextUnformatted("Runtime Folder Path");
        ImGui::InputText("##RuntimeFolderPath", &runtimeFolderPath);
        ImGui::SameLine();

        if (ImGui::Button(ICON_MDI_FOLDER))
        {
            runtimeFolderPath = FileDialog::OpenFolder(hwnd);
        }

        ImGui::Separator();

        ImGui::SetItemDefaultFocus();

        ImGui::BeginDisabled(runtimeFolderPath.empty());

        if (ImGui::Button("Ok", ImVec2(120, 0)))
        {
            settings.SetRuntimeFolderPath(runtimeFolderPath);
            settings.UpdateIniFile("RuntimeFolderPath", runtimeFolderPath);

            ImGui::CloseCurrentPopup();
        }

        ImGui::EndDisabled();
        ImGui::PopFont();

        ImGui::EndPopup();
    }

    ImGui::End();
}

LRESULT Editor::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (SceneViewportPanel::IsWindowHovered() && msg == WM_MOUSEMOVE)
    {
        SceneViewportPanel::OnMouseMove();
    }

    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
    {
        return true;
    }

    switch (msg)
    {
        case WM_SIZE:
        {
            if (wParam == SIZE_MINIMIZED)
            {
                return 0;
            }

            resizeWidth = (UINT)LOWORD(lParam);
            resizeHeight = (UINT)HIWORD(lParam);

            return 0;
        }
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_DPICHANGED:
        {
            if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DpiEnableScaleViewports)
            {
                const RECT* suggested_rect = reinterpret_cast<RECT*>(lParam);

                SetWindowPos(hwnd, nullptr, suggested_rect->left, suggested_rect->top, suggested_rect->right - suggested_rect->left, suggested_rect->bottom - suggested_rect->top, SWP_NOZORDER | SWP_NOACTIVATE);
            }

            break;
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void Editor::LoadRegistries()
{
    ResourceIDRegistry& resourceIDRegistry = ResourceIDRegistry::GetInstance();
    TypeRegistry& typeRegistry = TypeRegistry::GetInstance();
    ResourceInfoRegistry& resourceInfoRegistry = ResourceInfoRegistry::GetInstance();
    PropertyRegistry& propertyRegistry = PropertyRegistry::GetInstance();
    EnumRegistry& enumRegistry = EnumRegistry::GetInstance();
    TextListHashRegistry& textListHashRegistry = TextListHashRegistry::GetInstance();

    resourceIDRegistry.Load();
    typeRegistry.Load();
    resourceInfoRegistry.Load();
    propertyRegistry.Load();
    enumRegistry.Load();
    textListHashRegistry.Load();
}

std::shared_ptr<DirectXRenderer> Editor::GetDirectXRenderer() const
{
    return directXRenderer;
}

std::shared_ptr<ImGuiRenderer> Editor::GetImGuiRenderer() const
{
    return imGuiRenderer;
}

std::vector<std::shared_ptr<Document>>& Editor::GetDocuments()
{
    return documents;
}

std::shared_ptr<Document> Editor::GetLastActiveDocument() const
{
    return lastActiveDocument;
}

void Editor::SetupLayout(std::shared_ptr<Document> document, const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    document->ResetToolsVisibility();

    ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
    ImGui::DockBuilderSetNodeSize(dockspaceID, dockspaceSize);

    document->CreateLayout(dockspaceID, dockspaceSize);
}

// Submit document so we can retrieve its docking location
void Editor::UpdateDocumentLocation(std::shared_ptr<Document> document, const ImGuiID toplevelDockspaceID)
{
    //IM_ASSERT(document != NULL && document->GetOpen());
    IM_ASSERT(toplevelDockspaceID != 0);

    // Top level editors can only be docked with each others
    ImGui::SetNextWindowClass(&topLevelEditorWindowClass);

    if (document->GetDockID() != 0)
    {
        ImGui::SetNextWindowDockID(document->GetDockID());

        document->SetDockID(0);
    }
    else
    {
        ImGui::SetNextWindowDockID(toplevelDockspaceID, ImGuiCond_FirstUseEver);
    }

    // Create top level editor tab/window
    //ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None;
    bool* open = document->GetOpen();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    bool visible;

    if (document->GetType() == Document::Type::Scene)
    {
        visible = ImGui::Begin(std::format("{}##{}", document->GetName(), document->GetID()).c_str(), nullptr, windowFlags);
    }
    else
    {
        visible = ImGui::Begin(std::format("{}##{}", document->GetName(), document->GetID()).c_str(), open, windowFlags);
    }

    ImGui::PopStyleVar();

    // Store last focused document
    // This should ideally be a stack so we can handle closure immediately without an awkward gap where LastActiveDocument is NULL.
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        lastActiveDocument = document;

    ImGuiWindowClass* toolWindowsClass = document->GetToolWindowsClass();
    // Set WindowClass based on per-document ID, so tabs from Document A are not dockable in Document B etc. We could be using any ID suiting us, e.g. &doc
    // We also set ParentViewportId to request the platform back-end to set parent/child relationship at the windowing level.
    toolWindowsClass->ClassId = document->GetID();
    toolWindowsClass->ViewportFlagsOverrideSet = ImGuiViewportFlags_NoTaskBarIcon | ImGuiViewportFlags_NoDecoration;
    toolWindowsClass->ParentViewportId = ImGui::GetWindowViewport()->ID; // Make child of the top-level editor window
    toolWindowsClass->DockingAllowUnclassed = true;
    toolWindowsClass->DockNodeFlagsOverrideSet = ImGuiDockNodeFlags_NoCloseButton; //Hides the close button on the right

    // Track LocationID change so we can fork/copy the layout data according to where the window is going + reference count
    // LocationID ~~ (DockId != 0 ? DockId : DocumentID) // When we are in a loose floating window we use our own document id instead of the dock id
    document->SetCurrentDockID(ImGui::GetWindowDockID());
    document->SetPreviousLocationID(document->GetCurrentLocationID());
    document->SetCurrentLocationID(document->GetCurrentDockID() != 0 ? document->GetCurrentDockID() : document->GetID());

    // Dockspace ID ~~ Hash of LocationID + DocType
    // So all editors of a same type inside a same tab-bar will share the same layout.
    // We will also use this value as a suffix to create window titles, but we could perfectly have an indirection to allocate and use nicer names for window names (e.g. 0001, 0002).
    document->SetPreviousDockspaceID(document->GetCurrentDockspaceID());
    document->SetCurrentDockspaceID(document->CalculateDockspaceID());
    IM_ASSERT(document->GetCurrentDockspaceID() != 0);

    ImGui::End();
}

void Editor::UpdateDocumentContents(std::shared_ptr<Document> document)
{
    // This is the second Begin(), as UpdateDocumentLocationAndLayout() has already done one
    // (Therefore only the p_open and flags of the first call to Begin() applies)
    ImGui::Begin(std::format("{}##{}", document->GetName(), document->GetID()).c_str());
    //IM_ASSERT(ImGui::GetCurrentWindow()->BeginCount == 2);

    if (ImGui::GetCurrentWindow()->BeginCount != 2) //Case when document is created (UpdateDocumentLocation wasn't called)
    {
        ImGui::End();

        return;
    }

    const ImGuiID dockspaceID = document->GetCurrentDockspaceID();
    ImVec2 dockspaceSize = ImGui::GetContentRegionAvail();

    if (document->HasToolBar())
    {
        const float toolbarHeight = ImGui::GetFrameHeightWithSpacing() + ImGui::GetStyle().FramePadding.y * 2;

        dockspaceSize.y -= toolbarHeight + 1.f;
    }

    // Fork settings when extracting to a new location, or Overwrite settings when docking back into an existing location
    if (document->GetPreviousLocationID() != 0 && document->GetPreviousLocationID() != document->GetCurrentLocationID())
    {
        // Count references to tell if we should Copy or Move the layout.
        int previousDockspaceReferenceCount = 0;
        int currentDockspaceReferenceCount = 0;

        for (size_t i = 0; i < documents.size(); i++)
        {
            std::shared_ptr<Document> document2 = documents[i];

            if (document2->GetCurrentDockspaceID() == document->GetPreviousDockspaceID())
            {
                previousDockspaceReferenceCount++;
            }

            if (document2->GetCurrentDockspaceID() == document->GetCurrentDockspaceID())
            {
                currentDockspaceReferenceCount++;
            }
        }

        // Fork or overwrite settings
        // FIXME: should be able to do a "move window but keep layout" if currentDockspaceReferencecCount > 1.
        //IMGUI_DEBUG_LOG("LayoutCopy DockID %08X -> DockID %08X requested by doc '%s'\n", doc->PrevDockspaceID, doc->CurrDockspaceID, doc->Name);
        //IMGUI_DEBUG_LOG("--> previousDockspaceReferencecCount = %d --> %s\n", previousDockspaceReferencecCount, (previousDockspaceReferencecCount == 0) ? "Remove" : "Keep");
        //IMGUI_DEBUG_LOG("--> currentDockspaceReferencecCount = %d\n", currentDockspaceReferencecCount);

#if EDITOR_CONFIG_ONMERGE_OVERWRITE_WITH_SOURCE_LAYOUT
        //Always overwrite with dragged layout
        document->LayoutCopy(document->GetPreviousDockspaceID(), document->GetCurrentDockspaceID());
#else
        //Priority existing layout
        if (currentDockspaceReferenceCount <= 1)
        {
            document->LayoutCopy(document->GetPreviousDockspaceID(), document->GetCurrentDockspaceID());
        }
#endif

        if (previousDockspaceReferenceCount == 0)
        {
            ImGui::DockBuilderRemoveNode(document->GetPreviousDockspaceID());

            // Delete settings of old windows
            // Rely on window name to ditch their .ini settings forever..
            char windowSuffix[16];

            ImFormatString(windowSuffix, IM_ARRAYSIZE(windowSuffix), "##%08X", document->GetPreviousDockspaceID());

            size_t windowSuffixLength = strlen(windowSuffix);
            ImGuiContext& g = *GImGui;

            for (ImGuiWindowSettings* settings = g.SettingsWindows.begin(); settings != NULL; settings = g.SettingsWindows.next_chunk(settings))
            {
                if (settings->ID == 0)
                {
                    continue;
                }

                const char* windowName = settings->GetName();
                size_t windowNameLength = strlen(windowName);

                if (windowNameLength >= windowSuffixLength)
                {
                    if (strcmp(windowName + windowNameLength - windowSuffixLength, windowSuffix) == 0) // Compare suffix
                    {
                        ImGui::ClearWindowSettings(windowName);
                    }
                }
            }
        }
    }
    else if (ImGui::DockBuilderGetNode(document->GetCurrentDockspaceID()) == nullptr)
    {
        // Initialize new dockspace with preset
        // (to be able to call DockBuilderSplitNode() we need a node size ahead, so any preset override will probably have to be applied between Begin and Dockspace calls)
        // FIXME: Use g_LastActiveDoc as source reference and do a copy?
        SetupLayout(document, dockspaceID, dockspaceSize);
    }

    // FIXME-DOCK: This is a little tricky to explain but we currently need this to use the pattern of sharing a same dockspace between tabs of a same tab bar
    bool visible = true;

#if EDITOR_CONFIG_SAME_LOCATION_SHARE_LAYOUT
    if (ImGui::GetCurrentWindow()->Hidden)
    {
        visible = false;
    }
#endif

    if (!visible)
    {
        // Keep alive document dockspace so windows that are docked into it but which visibility are not linked to the dockspace visibility won't get undocked.
        ImGui::DockSpace(dockspaceID, dockspaceSize, ImGuiDockNodeFlags_KeepAliveOnly, document->GetToolWindowsClass());
        ImGui::End();

        return;
    }

    if (document->HasToolBar())
    {
        document->RenderToolBar();
    }

    // Submit the dockspace node
    ImGui::DockSpace(dockspaceID, dockspaceSize, ImGuiDockNodeFlags_None, document->GetToolWindowsClass());
    ImGui::End();

    const bool isLastFocusedDocument = lastActiveDocument == document;

    document->RenderPanels(isLastFocusedDocument);
}
