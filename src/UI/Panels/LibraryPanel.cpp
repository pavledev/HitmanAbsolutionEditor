#include <thread>
#include <fstream>

#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"

#include "UI/Panels/LibraryPanel.h"
#include "Logger.h"
#include "Utility/ResourceUtility.h"
#include "Utility/UI.h"
#include "Editor.h"

LibraryPanel::LibraryPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
	std::thread thread(&LibraryPanel::LoadHeaderLibraryResourceIDs, this);

	thread.detach();
}

void LibraryPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

	static float leftPaneWidth = ImGui::GetWindowWidth() / 2;
	static float rightPaneWidth = ImGui::GetWindowWidth() / 2;

	UI::Splitter(true, 4.0f, &leftPaneWidth, &rightPaneWidth, 50.0f, 50.0f);

	ImGui::BeginChild("leftPanel", ImVec2(leftPaneWidth, ImGui::GetWindowHeight()), true);

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Header Library");
	ImGui::SameLine();

	const bool isInputTextEnterPressed = ImGui::InputText("##HeaderLibrary", &currentHeaderLibraryResourceID, ImGuiInputTextFlags_EnterReturnsTrue);
	const bool isInputTextActive = ImGui::IsItemActive();

	if (ImGui::IsItemActivated())
	{
		ImGui::OpenPopup("##headerLibraiesPopup");
	}

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

	if (ImGui::BeginPopup("##headerLibraiesPopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
	{
		for (size_t i = 0; i < headerLibraryResourceIDs.size(); ++i)
		{
			std::string headerLibraryResourceID = headerLibraryResourceIDs[i];

			if (!headerLibraryResourceID.contains(currentHeaderLibraryResourceID))
			{
				continue;
			}

			if (ImGui::Selectable(headerLibraryResourceID.c_str()))
			{
				ImGui::ClearActiveID();

				currentHeaderLibraryResourceID = headerLibraryResourceID;

				headerLibrary.ParseHeaderLibrary(currentHeaderLibraryResourceID);

				headerLibraryChunk = headerLibrary.ParseHeaderLibraryChunk(0);
			}
		}

		if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	const SResourceHeaderHeader& resourceHeaderHeader = headerLibrary.GetResourceHeaderHeader();
	const std::vector<Resource>& references = headerLibrary.GetReferences();
	const std::vector<std::string>& chunkResourceIDs = headerLibrary.GetChunkResourceIDs();
	const std::vector<std::string>& externalResourceIDs = headerLibrary.GetExternalResourceIDs();

	ImGui::Text("");
	ImGui::Text("File Path: %s", headerLibrary.GetFilePath().c_str());
	ImGui::Text("Type: HLIB");
	ImGui::Text("References Chunk Size: 0x%X", resourceHeaderHeader.m_nReferencesChunkSize);
	ImGui::Text("States Chunk Size: 0x%X", resourceHeaderHeader.m_nStatesChunkSize);
	ImGui::Text("Data size: 0x%X", resourceHeaderHeader.m_nDataSize);
	ImGui::Text("System Memory Requirement: 0x%X", resourceHeaderHeader.m_nSystemMemoryRequirement);
	ImGui::Text("Video Memory Requirement: 0x%X", resourceHeaderHeader.m_nVideoMemoryRequirement);

	if (references.size() > 0)
	{
		static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable;

		ImGui::Text("");
		ImGui::Text("References");

		if (ImGui::BeginTable("ReferencesTable", 3, tableFlags))
		{
			ImGui::TableSetupColumn("Resource ID", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			for (size_t i = 0; i < references.size(); ++i)
			{
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text(references[i].GetResourceID().c_str());

				ImGui::TableSetColumnIndex(1);
				ImGui::Text(ResourceUtility::ConvertResourceReferenceFlagsToString(references[i].GetResourceReferenceFlags()).c_str());
			}

			ImGui::EndTable();
		}
	}

	if (chunkResourceIDs.size() > 0)
	{
		static int currentItemIndex = 0;

		ImGui::Text("");
		ImGui::Text("Chunk Resource IDs");

		if (ImGui::BeginListBox("##ChunkResourceIDs", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (size_t i = 0; i < chunkResourceIDs.size(); ++i)
			{
				const bool isSelected = currentItemIndex == i;

				if (ImGui::Selectable(chunkResourceIDs[i].c_str(), isSelected))
				{
					currentItemIndex = i;
					headerLibraryChunk = headerLibrary.ParseHeaderLibraryChunk(currentItemIndex);
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	if (externalResourceIDs.size() > 0)
	{
		static int currentItemIndex2 = 0;

		ImGui::Text("");
		ImGui::Text("External Resource IDs");

		if (ImGui::BeginListBox("##ExternalResourceIDs", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (size_t i = 0; i < externalResourceIDs.size(); ++i)
			{
				const bool isSelected = currentItemIndex2 == i;

				if (ImGui::Selectable(externalResourceIDs[i].c_str(), isSelected))
				{
					currentItemIndex2 = i;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	ImGui::EndChild();

	ImGui::SameLine();
	ImGui::BeginChild("rightPanel", ImVec2(rightPaneWidth, ImGui::GetWindowHeight()), true);

	ImGui::Text("File Path: %s", headerLibraryChunk.filePath.c_str());
	ImGui::Text("Type: RLIB");
	ImGui::Text("References Chunk Size: 0x%X", headerLibraryChunk.resourceHeaderHeader.m_nReferencesChunkSize);
	ImGui::Text("States Chunk Size: 0x%X", headerLibraryChunk.resourceHeaderHeader.m_nStatesChunkSize);
	ImGui::Text("Data size: 0x%X", headerLibraryChunk.resourceHeaderHeader.m_nDataSize);
	ImGui::Text("System Memory Requirement: 0x%X", headerLibraryChunk.resourceHeaderHeader.m_nSystemMemoryRequirement);
	ImGui::Text("Video Memory Requirement: 0x%X", headerLibraryChunk.resourceHeaderHeader.m_nVideoMemoryRequirement);
	ImGui::Text("Offset: 0x%X", headerLibraryChunk.offset);
	ImGui::Text("Flags: %s", ResourceUtility::ConvertHeaderLibraryChunkFlagsToString(headerLibraryChunk.flags).c_str());
	ImGui::Text("LocalizationCategory: 0x%X", headerLibraryChunk.localizationCategory);

	if (headerLibraryChunk.languages.size() > 0)
	{
		static int currentItemIndex3 = 0;

		ImGui::Text("");
		ImGui::Text("Languages");

		if (ImGui::BeginListBox("##Languages", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (size_t i = 0; i < headerLibraryChunk.languages.size(); ++i)
			{
				const bool isSelected = currentItemIndex3 == i;

				if (ImGui::Selectable(headerLibraryChunk.languages[i].c_str(), isSelected))
				{
					currentItemIndex3 = i;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	if (headerLibraryChunk.states.size() > 0)
	{
		static int currentItemIndex4 = 0;

		ImGui::Text("");
		ImGui::Text("States");

		if (ImGui::BeginListBox("##States", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (size_t i = 0; i < headerLibraryChunk.states.size(); ++i)
			{
				const bool isSelected = currentItemIndex4 == i;

				if (ImGui::Selectable(std::to_string(headerLibraryChunk.states[i]).c_str(), isSelected))
				{
					currentItemIndex4 = i;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	if (headerLibraryChunk.ridMappingIndices.size() > 0)
	{
		static int currentItemIndex5 = 0;

		ImGui::Text("");
		ImGui::Text("Mapping Indices");

		if (ImGui::BeginListBox("##MappingIndices", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (size_t i = 0; i < headerLibraryChunk.ridMappingIndices.size(); ++i)
			{
				const bool isSelected = currentItemIndex5 == i;

				if (ImGui::Selectable(std::to_string(headerLibraryChunk.ridMappingIndices[i]).c_str(), isSelected))
				{
					currentItemIndex5 = i;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	if (headerLibraryChunk.ridMappingIDs.size() > 0)
	{
		static int currentItemIndex6 = 0;

		ImGui::Text("");
		ImGui::Text("Mapping IDs");

		if (ImGui::BeginListBox("##MappingIDs", ImVec2(-FLT_MIN, 5 * ImGui::GetTextLineHeightWithSpacing())))
		{
			for (size_t i = 0; i < headerLibraryChunk.ridMappingIDs.size(); ++i)
			{
				const bool isSelected = currentItemIndex6 == i;

				if (ImGui::Selectable(StringUtility::ConvertValueToHexString(headerLibraryChunk.ridMappingIDs[i]).c_str(), isSelected))
				{
					currentItemIndex6 = i;
				}

				// Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndListBox();
		}
	}

	ImGui::EndChild();

	ImGui::PopFont();

	End();
}

void LibraryPanel::LoadHeaderLibraryResourceIDs()
{
	std::ifstream inputFile = std::ifstream("assets/HeaderLibraries.txt");

	if (inputFile.is_open())
	{
		std::string line;

		while (getline(inputFile, line))
		{
			headerLibraryResourceIDs.push_back(line);
		}

		inputFile.close();
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to open HeaderLibraries.txt file!");
	}
}
