#include <IconsMaterialDesignIcons.h>

#include <UI/Documents/SceneDocument.h>
#include <UI/Documents/LibraryInfoDocument.h>
#include <UI/Documents/HeaderLibrariesSearchDocument.h>
#include <UI/Documents/HashMapSearchDocument.h>
#include <UI/Panels/SceneHierarchyPanel.h>
#include <UI/Panels/SceneViewportPanel.h>
#include <UI/Panels/ConsolePanel.h>
#include <UI/Panels/ResourceBrowserPanel.h>
#include <Editor.h>

SceneDocument::SceneDocument(const char* name, const char* icon, const Type type) : Document(name, icon, type)
{
    std::shared_ptr<SceneHierarchyPanel> sceneHierarchyPanel = std::make_shared<SceneHierarchyPanel>("Scene Hierarchy", ICON_MDI_VIEW_LIST, nullptr);
    std::shared_ptr<SceneViewportPanel> sceneViewportPanel = std::make_shared<SceneViewportPanel>("Scene Viewport", ICON_MDI_MONITOR);
    std::shared_ptr<ConsolePanel> consolePanel = std::make_shared<ConsolePanel>("Console", ICON_MDI_CONSOLE);
    std::shared_ptr<ResourceBrowserPanel> resourceBrowserPanel = std::make_shared<ResourceBrowserPanel>("Resource Browser", ICON_MDI_FOLDER_OPEN);

    AddPanel(sceneHierarchyPanel);
    AddPanel(sceneViewportPanel);
    AddPanel(consolePanel);
    AddPanel(resourceBrowserPanel);
}

void SceneDocument::CreateLayout(const ImGuiID dockspaceID, const ImVec2 dockspaceSize)
{
    ImGuiID mainDockID = dockspaceID;
    ImGuiID leftDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Left, 0.2f, nullptr, &mainDockID);
    ImGuiID rightDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Right, 0.2f, nullptr, &mainDockID);
    ImGuiID downDockID = ImGui::DockBuilderSplitNode(mainDockID, ImGuiDir_Down, 0.25f, nullptr, &mainDockID);

    ImGui::DockBuilderDockWindow(CalculatePanelID(0, currentDockspaceID).c_str(), leftDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(1, currentDockspaceID).c_str(), mainDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(2, currentDockspaceID).c_str(), downDockID);
    ImGui::DockBuilderDockWindow(CalculatePanelID(3, currentDockspaceID).c_str(), rightDockID);
    ImGui::DockBuilderFinish(dockspaceID);
}

void SceneDocument::RenderMenuBar()
{
    static std::string settingsLabel = std::format("{} Settings", ICON_MDI_COG);
    static std::string searchHeaderLibrariesLabel = std::format("{} Search Header Libraries", ICON_MDI_MAGNIFY);
    static std::string searchHashMapLabel = std::format("{} Search Hash Map", ICON_MDI_MAGNIFY);
    static std::string deepSearchLabel = std::format("{} Deep Search", ICON_MDI_MAGNIFY);
    const ImGuiID defaultDockID = Editor::GetInstance().GetLastActiveDocument() ? Editor::GetInstance().GetLastActiveDocument()->GetCurrentDockID() : 0;

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("View"))
        {
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Engine"))
        {
            if (ImGui::MenuItem("Connect with engine"))
            {
                /*PipeClient& pipeClient = PipeClient::GetInstance();
                SharedMemoryClient& sharedMemoryClient = SharedMemoryClient::GetInstance();

                if (pipeClient.IsConnectedWithEngine())
                {
                    Logger::GetInstance().Log(Logger::Level::Info, "Editor is already connected with engine.");
                }
                else
                {
                    pipeClient.Connect();
                    pipeClient.SetMessageCallback(MemberDelegate<SceneViewportPanel, void(const std::string & type, const std::string & content)>(sceneViewportPanel.get(), &SceneViewportPanel::OnReceiveMessage));

                    sharedMemoryClient.Connect();
                }*/
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Library"))
        {
            if (ImGui::MenuItem("View Library Info"))
            {
                std::shared_ptr<LibraryInfoDocument> libraryInfoDocument = std::make_shared<LibraryInfoDocument>("Library Info", ICON_MDI_FILE_DOCUMENT, Document::Type::LibraryInfo, defaultDockID);

                Editor::GetInstance().GetDocuments().push_back(libraryInfoDocument);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Search"))
        {
            if (ImGui::MenuItem(searchHeaderLibrariesLabel.c_str()))
            {
                std::shared_ptr<HeaderLibrariesSearchDocument> headerLibrariesSearchDocument = std::make_shared<HeaderLibrariesSearchDocument>("Search Header Libraries", ICON_MDI_FILE_DOCUMENT, Document::Type::SearchHeaderLibraries, defaultDockID);

                Editor::GetInstance().GetDocuments().push_back(headerLibrariesSearchDocument);
            }

            if (ImGui::MenuItem(searchHashMapLabel.c_str()))
            {
                std::shared_ptr<HashMapSearchDocument> hashMapSearchDocument = std::make_shared<HashMapSearchDocument>("Search Hash Map", ICON_MDI_FILE_DOCUMENT, Document::Type::SearchHashMap, defaultDockID);

                Editor::GetInstance().GetDocuments().push_back(hashMapSearchDocument);
            }

            if (ImGui::MenuItem(deepSearchLabel.c_str()))
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Tools"))
        {
            if (ImGui::MenuItem(settingsLabel.c_str()))
            {
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
    }
}
