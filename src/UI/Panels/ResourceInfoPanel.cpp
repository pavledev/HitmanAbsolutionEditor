#include <thread>

#include <IconsMaterialDesignIcons.h>

#include "misc/cpp/imgui_stdlib.h"
#include "imgui_internal.h"

#include "UI/Panels/ResourceInfoPanel.h"
#include "Utility/ResourceUtility.h"
#include "Resources/Texture.h"

ResourceInfoPanel::ResourceInfoPanel(const char* name, const char* icon, std::shared_ptr<Resource> resource) : BasePanel(name, icon)
{
	this->resource = resource;
}

void ResourceInfoPanel::Render()
{
	Begin();

	if (!resource->IsResourceLoaded())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetWindowSize().x / 2, ImGui::GetWindowSize().y / 2));
		ImGui::Text("Loading Resource Info...");
		End();

		return;
	}

	const std::vector<Resource::HeaderLibrary>* headerLibraries = resource->GetHeaderLibraries();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Show Resource Info From");
	ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

	const bool isInputTextEnterPressed = ImGui::InputText("##HeaderLibrary", &currentHeaderLibraryResourceID, ImGuiInputTextFlags_EnterReturnsTrue);

	ImGui::PopItemWidth();

	const bool isInputTextActive = ImGui::IsItemActive();

	if (ImGui::IsItemActivated())
	{
		ImGui::OpenPopup("##headerLibraiesPopup2");
	}

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
	ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

	if (ImGui::BeginPopup("##headerLibraiesPopup2", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
	{
		for (size_t i = 0; i < headerLibraries->size(); ++i)
		{
			std::string headerLibraryResourceID = (*headerLibraries)[i].resourceID;

			if (!headerLibraryResourceID.starts_with("["))
			{
				headerLibraryResourceID = std::format("[[assembly:/Common/PC.layoutconfig].pc_layoutdef](assembly:/scenes/{}/{}_main.entity).pc_headerlib", headerLibraryResourceID, headerLibraryResourceID);
			}

			if (!headerLibraryResourceID.contains(currentHeaderLibraryResourceID))
			{
				continue;
			}

			if (ImGui::Selectable(headerLibraryResourceID.c_str()))
			{
				ImGui::ClearActiveID();

				currentHeaderLibraryResourceID = headerLibraryResourceID;

				std::thread thread(&Resource::LoadResource, resource, i, (*headerLibraries)[i].chunkIndex, (*headerLibraries)[i].indexInLibrary, true, true, false);

				thread.detach();
			}
		}

		if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	switch (resource->GetResourceHeaderHeader().m_type)
	{
	case 'TEXT':
		DisplayTextureInfo();
		break;
	default:
		break;
	}

	const SResourceHeaderHeader& resourceHeaderHeader = resource->GetResourceHeaderHeader();
	const std::vector<std::shared_ptr<Resource>>& references = resource->GetReferences();
	const std::vector<std::shared_ptr<Resource>>& backRefereces = resource->GetBackReferences();

	ImGui::Text("");
	ImGui::Text("Type: %s", ResourceUtility::ConvertResourceTypeToString(resourceHeaderHeader.m_type).c_str());
	ImGui::Text("References Chunk Size: 0x%X", resourceHeaderHeader.m_nReferencesChunkSize);
	ImGui::Text("States Chunk Size: 0x%X", resourceHeaderHeader.m_nStatesChunkSize);
	ImGui::Text("Data size: 0x%X", resourceHeaderHeader.m_nDataSize);
	ImGui::Text("System Memory Requirement: 0x%X", resourceHeaderHeader.m_nSystemMemoryRequirement);
	ImGui::Text("Video Memory Requirement: 0x%X", resourceHeaderHeader.m_nVideoMemoryRequirement);
	ImGui::Text("Resource ID: %s", resource->GetResourceID().c_str());
	ImGui::Text("Header Library Chunk Resource ID: %s", resource->GetHeaderLibraryChunkResourceID().c_str());
	ImGui::Text("Index In Library: %d", resource->GetIndexInLibrary());
	ImGui::Text("Runtime Resource ID: %s", StringUtility::ConvertValueToHexString(resource->GetRuntimeResourceID().GetID()).c_str());
	ImGui::Text("Offset In Header Library: 0x%X", resource->GetOffsetInHeaderLibrary());
	ImGui::Text("Offset In Resource Library: 0x%X", resource->GetOffsetInResourceLibrary());
	ImGui::Text("Header data size: 0x%X", resource->GetHeaderDataSize());
	ImGui::Text("Resource data size: 0x%X", resource->GetResourceDataSize());

	if (references.size() > 0)
	{
		std::vector<std::shared_ptr<Resource>> filteredReferences;
		std::string hint = std::format("{} Search reference...", ICON_MDI_MAGNIFY);

		FilterReferences(reference, hint.c_str(), references, filteredReferences);

		const float textBaseHeight = ImGui::GetTextLineHeightWithSpacing();
		static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY;
		ImVec2 outerSize = ImVec2(0.0f, textBaseHeight * 8);

		ImGui::Text("");
		ImGui::Text("References");
		ImGui::Spacing();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputTextWithHint("##SearchReference", hint.c_str(), reference, IM_ARRAYSIZE(reference));
		ImGui::PopItemWidth();
		ImGui::Spacing();

		if (ImGui::BeginTable("ReferencesTable", 3, tableFlags, outerSize))
		{
			ImGui::TableSetupColumn("Resource ID", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Runtime Resource ID", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			ImGuiListClipper clipper;

			clipper.Begin(filteredReferences.size());

			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text(filteredReferences[row]->GetResourceID().c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(StringUtility::ConvertValueToHexString(filteredReferences[row]->GetRuntimeResourceID().GetID()).c_str());

					ImGui::TableSetColumnIndex(2);
					ImGui::Text(ResourceUtility::ConvertResourceReferenceFlagsToString(filteredReferences[row]->GetResourceReferenceFlags()).c_str());
				}
			}

			ImGui::EndTable();
		}
	}

	if (backRefereces.size() > 0)
	{
		std::vector<std::shared_ptr<Resource>> filteredBackReferences;
		std::string hint = std::format("{} Search back reference...", ICON_MDI_MAGNIFY);

		FilterReferences(backReference, hint.c_str(), backRefereces, filteredBackReferences);

		const float textBaseHeight = ImGui::GetTextLineHeightWithSpacing();
		static ImGuiTableFlags tableFlags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_ScrollY;
		ImVec2 outerSize = ImVec2(0.0f, textBaseHeight * 8);

		ImGui::Text("");
		ImGui::Text("Back References");
		ImGui::Spacing();

		ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
		ImGui::InputTextWithHint("##SearchBackReference", hint.c_str(), backReference, IM_ARRAYSIZE(backReference));
		ImGui::PopItemWidth();
		ImGui::Spacing();

		if (ImGui::BeginTable("BackReferencesTable", 3, tableFlags, outerSize))
		{
			ImGui::TableSetupColumn("Resource ID", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Runtime Resource ID", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableSetupColumn("Flags", ImGuiTableColumnFlags_WidthFixed);
			ImGui::TableHeadersRow();

			ImGuiListClipper clipper;

			clipper.Begin(filteredBackReferences.size());

			while (clipper.Step())
			{
				for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
				{
					ImGui::TableNextRow();

					ImGui::TableSetColumnIndex(0);
					ImGui::Text(filteredBackReferences[row]->GetResourceID().c_str());

					ImGui::TableSetColumnIndex(1);
					ImGui::Text(StringUtility::ConvertValueToHexString(filteredBackReferences[row]->GetRuntimeResourceID().GetID()).c_str());

					ImGui::TableSetColumnIndex(2);
					ImGui::Text(ResourceUtility::ConvertResourceReferenceFlagsToString(filteredBackReferences[row]->GetResourceReferenceFlags()).c_str());
				}
			}

			ImGui::EndTable();
		}
	}

	End();
}

void ResourceInfoPanel::OnResourceLoaded()
{
	if (resource->GetHeaderLibraries()->size() > 0)
	{
		currentHeaderLibraryResourceID = (*resource->GetHeaderLibraries())[0].resourceID;

		if (!currentHeaderLibraryResourceID.starts_with("["))
		{
			currentHeaderLibraryResourceID = std::format("[[assembly:/Common/PC.layoutconfig].pc_layoutdef](assembly:/scenes/{}/{}_main.entity).pc_headerlib", currentHeaderLibraryResourceID, currentHeaderLibraryResourceID);
		}
	}
}

void ResourceInfoPanel::FilterReferences(const char* reference, const char* hint, const std::vector<std::shared_ptr<Resource>>& references, std::vector<std::shared_ptr<Resource>>& filteredReferences)
{
	for (size_t i = 0; i < references.size(); ++i)
	{
		std::string resourceID = references[i]->GetResourceID();
		std::string runtimeResourceID = StringUtility::ConvertValueToHexString(references[i]->GetRuntimeResourceID().GetID());
		std::string flags = ResourceUtility::ConvertResourceReferenceFlagsToString(references[i]->GetResourceReferenceFlags());

		if (strcmp(reference, hint) != 0 && !resourceID.contains(reference) && !runtimeResourceID.contains(reference) && !flags.contains(reference))
		{
			continue;
		}

		filteredReferences.push_back(references[i]);
	}
}

void ResourceInfoPanel::DisplayTextureInfo()
{
	if (!resource->GetResourceData())
	{
		return;
	}

	static ZTextureMap::STextureMapHeader textureMapHeader = Texture::GetTextureMapHeader(resource->GetResourceData(), resource->GetResourceDataSize());

	ImGui::Text("Texture Info");
	ImGui::Spacing();
	ImGui::Text("\tNumSlices: %u", textureMapHeader.nNumSlices);
	ImGui::Text("\tTotal Size: 0x%X", textureMapHeader.nTotalSize);
	ImGui::Text("\tFlags: %s", Texture::ConvertTextureFlagsToString(static_cast<ZTextureMap::ETextureFlags>(textureMapHeader.nFlags)).c_str());
	ImGui::Text("\tWidth: %hu", textureMapHeader.nWidth);
	ImGui::Text("\tHeight: %hu", textureMapHeader.nHeight);
	ImGui::Text("\tFormat: %s", Texture::ConvertRenderFormatToString(static_cast<ERenderFormat>(textureMapHeader.nFormat)).c_str());
	ImGui::Text("\tMip Levels: %u", textureMapHeader.nNumMipLevels);
	ImGui::Text("\tDefault Mip Level: %u", textureMapHeader.nDefaultMipLevel);
	ImGui::Text("\tInterpret As: %s", Texture::ConvertInterpretAsToString(static_cast<ZTextureMap::EInterpretAs>(textureMapHeader.nInterpretAs)).c_str());
	ImGui::Text("\tDimensions: %s", Texture::ConvertDimensionsToString(static_cast<ZTextureMap::EDimensions>(textureMapHeader.nDimensions)).c_str());
	ImGui::Text("\tMip Interpolation: %s", Texture::ConvertRenderResourceMipInterpolationToString(static_cast<ERenderResourceMipInterpolation>(textureMapHeader.nMipInterpolation)).c_str());
	ImGui::Text("\tIA Data Size: 0x%X", textureMapHeader.nIADataSize);
	ImGui::Text("\tIA Data Offset: 0x%X", textureMapHeader.nIADataOffset);
	ImGui::Separator();
}
