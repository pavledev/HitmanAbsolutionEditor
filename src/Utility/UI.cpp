#include <format>

#include <IconsMaterialDesignIcons.h>

#include "Glacier/Math/SMatrix43.h"
#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/SBodyPartDamageMultipliers.h"
#include "Glacier/Entity/SEntityTemplateReference.h"
#include "Glacier/Resource/ZRuntimeResourceID.h"
#include "Glacier/TypeInfo/STypeID.h"
#include "Glacier/Sound/ZSpeakerLevels.h"
#include "Glacier/ZCurve.h"
#include "Glacier/ZGameTime.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/ECameraState.h"
#include "Glacier/ZSharedSensorDef.h"
#include "Glacier/SSettingsParamMultiplier.h"

#include "Utility/UI.h"
#include "Registry/EnumRegistry.h"
#include "Registry/PropertyRegistry.h"
#include "Hash.h"
#include "Editor.h"
#include "Utility/FileDialog.h"
#include "Resources/WaveBankFSB.h"
#include "Resources/FlashMovie.h"
#include "Utility/ResourceUtility.h"

bool UI::IsWindowHovered(const ImGuiID windowID)
{
	if (!GImGui)
	{
		return false;
	}

	ImGuiContext& g = *GImGui;

	return g.HoveredWindow && g.HoveredWindow->ID == windowID;
}

ImVec2 UI::GetWindowContentRegionMin(ImGuiWindow* window)
{
	return ImVec2(window->ContentRegionRect.Min.x - window->Pos.x, window->ContentRegionRect.Min.y - window->Pos.y);
}

Vector2 UI::GetMousePositionRelativeToWindow()
{
	ImVec2 mousePosition = ImGui::GetMousePos();
	ImVec2 windowContentRegionMin = ImGui::GetWindowContentRegionMin();

	windowContentRegionMin.x += ImGui::GetWindowPos().x;
	windowContentRegionMin.y += ImGui::GetWindowPos().y;

	return Vector2(mousePosition.x - windowContentRegionMin.x, mousePosition.y - windowContentRegionMin.y);
}

Vector2 UI::GetMousePositionRelativeToWindow(ImGuiWindow* window)
{
	ImVec2 mousePosition = ImGui::GetMousePos();
	ImVec2 windowContentRegionMin = GetWindowContentRegionMin(window);

	windowContentRegionMin.x += window->Pos.x;
	windowContentRegionMin.y += window->Pos.y;

	return Vector2(mousePosition.x - windowContentRegionMin.x, mousePosition.y - windowContentRegionMin.y);
}

std::string UI::GetPropertyID(const char* label)
{
	return std::format("##{}_{}_{}", tableName, label, propertyCounter);
}

bool UI::BeginProperties(const char* tableName, const std::vector<TableColumn> tableColumns, const bool showHeaders, const ImGuiTableFlags tableFlags)
{
	UI::tableName = tableName;

	constexpr ImGuiTableFlags tableFlags2 = ImGuiTableFlags_PadOuterX;

	if (ImGui::BeginTable(tableName, tableColumns.size(), tableFlags2 | tableFlags))
	{
		ImGui::TableSetupScrollFreeze(0, 1);

		for (size_t i = 0; i < tableColumns.size(); ++i)
		{
			ImGui::TableSetupColumn(tableColumns[i].name, tableColumns[i].flags, tableColumns[i].size);
		}

		if (showHeaders)
		{
			ImGui::TableHeadersRow();
		}

		return true;
	}

	return false;
}

void UI::EndProperties()
{
	ImGui::EndTable();

	propertyCounter = 0;
}

void UI::BeginProperty(const char* label, const char* tooltip, bool rightAlignNextColumn)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	const std::string id = std::format("{}_{}", tableName, propertyCounter);

	ImGui::PushID(id.c_str());
	ImGui::AlignTextToFramePadding();
	ImGui::TextUnformatted(label);

	if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay))
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(tooltip);
		ImGui::EndTooltip();
	}

	ImGui::TableNextColumn();

	if (rightAlignNextColumn)
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	++propertyCounter;
}

void UI::BeginProperty(std::string& label, const char* tooltip, bool rightAlignNextColumn)
{
	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	const std::string id = std::format("{}_{}", tableName, propertyCounter);

	ImGui::PushID(id.c_str());
	ImGui::InputText("##", &label);

	if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay))
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(tooltip);
		ImGui::EndTooltip();
	}

	ImGui::TableNextColumn();

	if (rightAlignNextColumn)
	{
		ImGui::SetNextItemWidth(-FLT_MIN);
	}

	++propertyCounter;
}

bool UI::BeginTreeNodeProperty(const char* label, const char* tooltip, bool shouldNodeBeOpenedByDefault)
{
	const std::string id = std::format("{}_{}", tableName, propertyCounter);
	static ImGuiTreeNodeFlags treeNodeFlags =
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;

	if (shouldNodeBeOpenedByDefault)
	{
		treeNodeFlags |= ImGuiTreeNodeFlags_DefaultOpen;
	}

	ImGui::TableNextRow();
	ImGui::TableNextColumn();

	ImGui::PushID(id.c_str());

	const bool isTreeNodeOpen = ImGui::TreeNodeEx(label, treeNodeFlags);

	if (tooltip && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal | ImGuiHoveredFlags_NoSharedDelay))
	{
		ImGui::BeginTooltip();
		ImGui::TextUnformatted(tooltip);
		ImGui::EndTooltip();
	}

	ImGui::TableNextColumn();

	++propertyCounter;

	return isTreeNodeOpen;
}

void UI::EndProperty()
{
	ImGui::PopID();
}

void UI::EndTreeNodeProperty(const bool isTreeNodeOpen)
{
	if (isTreeNodeOpen)
	{
		ImGui::TreePop();
	}

	ImGui::PopID();
}

bool UI::StringProperty(const char* label, std::string& string, const char* tooltip, const bool isDisabled)
{
	BeginProperty(label, tooltip);
	ImGui::BeginDisabled(isDisabled);

	const bool modified = ImGui::InputText(GetPropertyID(label).c_str(), &string);

	ImGui::EndDisabled();
	EndProperty();

	return modified;
}

bool UI::StringProperty(std::string& label, std::string& string, const char* tooltip, const bool isDisabled)
{
	BeginProperty(label, tooltip);
	ImGui::BeginDisabled(isDisabled);

	const bool modified = ImGui::InputText(GetPropertyID(label.c_str()).c_str(), &string);

	ImGui::EndDisabled();
	EndProperty();

	return modified;
}

bool UI::Property(const char* label, char* stringBuffer, size_t stringBufferSize, const char* tooltip, const bool isDisabled, std::function<void(char*)> onTextChangeCallback)
{
	BeginProperty(label, tooltip);
	ImGui::BeginDisabled(isDisabled);

	const bool modified = ImGui::InputText(GetPropertyID(label).c_str(), stringBuffer, stringBufferSize);

	ImGui::EndDisabled();
	EndProperty();

	if (modified && onTextChangeCallback)
	{
		onTextChangeCallback(stringBuffer);
	}

	return modified;
}

bool UI::Property(const char* label, bool& flag, const char* tooltip)
{
	BeginProperty(label, tooltip);

	bool modified = ImGui::Checkbox(GetPropertyID(label).c_str(), &flag);

	EndProperty();

	return modified;
}

bool UI::Property(const char* label, std::vector<std::string>& flagStrings, std::vector<bool>& flags, const bool renderInSameLine, const char* tooltip)
{
	BeginProperty(label, tooltip);

	bool modified = false;

	for (size_t i = 0; i < flags.size(); ++i)
	{
		ImGui::PushID(flagStrings[i].c_str());

		bool flag = flags[i];
		bool modified2 = ImGui::Checkbox(flagStrings[i].c_str(), &flag);

		ImGui::PopID();

		if (modified2)
		{
			flags[i] = flag;
			modified = true;
		}

		if (renderInSameLine)
		{
			ImGui::SameLine();
		}
	}

	EndProperty();

	return modified;
}

bool UI::Property(const char* label, int& value, const char** dropdownStrings, int count, const char* tooltip)
{
	BeginProperty(label, tooltip);

	bool modified = false;
	const char* current = dropdownStrings[value];

	if (ImGui::BeginCombo(GetPropertyID(label).c_str(), current))
	{
		for (int i = 0; i < count; i++)
		{
			bool isSelected = current == dropdownStrings[i];

			if (ImGui::Selectable(dropdownStrings[i], isSelected))
			{
				current = dropdownStrings[i];
				value = i;
				modified = true;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	EndProperty();

	return modified;
}

bool UI::ColorRGBProperty(const char* label, Vector3& vector, const char* tooltip)
{
	BeginProperty(label, tooltip);

	bool modified = ImGui::ColorEdit3(GetPropertyID(label).c_str(), &vector.x);

	EndProperty();

	return modified;
}

bool UI::ColorRGBAProperty(const char* label, Vector4& vector, const char* tooltip)
{
	BeginProperty(label, tooltip);

	bool modified = ImGui::ColorEdit4(GetPropertyID(label).c_str(), &vector.x);

	EndProperty();

	return modified;
}

void UI::Vector2Property(const char* label, Vector2& vector, const char* tooltip, float resetValue)
{
	BeginProperty(label, tooltip, false);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 3.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("X", buttonSize))
	{
		vector.x = resetValue;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector.y = resetValue;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	EndProperty();
}

void UI::Vector3Property(const char* label, Vector3& vector, const char* tooltip, float resetValue, std::function<void(Vector3&)> onValueChangeCallback)
{
	BeginProperty(label, tooltip, false);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 3.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	bool isValueChanged = false;

	if (ImGui::Button("X", buttonSize))
	{
		vector.x = resetValue;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##X", &vector.x, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector.y = resetValue;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Y", &vector.y, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Z
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Z", buttonSize))
	{
		vector.z = resetValue;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Z", &vector.z, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	if (isValueChanged && onValueChangeCallback)
	{
		onValueChangeCallback(vector);
	}

	EndProperty();
}

bool UI::IconButton(const char* icon, const char* label, ImVec4 iconColor, ImVec4 backgroundColor)
{
	ImVec2 iconButtonSize = GetIconButtonSize(icon, label);

	ImGui::PushStyleColor(ImGuiCol_Button, backgroundColor);

	const float cursorPosX = ImGui::GetCursorPosX();
	bool clicked = ImGui::Button(std::format("##{}", label).c_str(), iconButtonSize);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });

	ImGui::SameLine();
	ImGui::SetCursorPosX(cursorPosX);
	ImGui::TextColored(iconColor, "%s", icon);
	ImGui::SameLine();
	ImGui::TextUnformatted(label);

	ImGui::PopStyleVar();
	ImGui::PopStyleColor();

	return clicked;
}

ImVec2 UI::GetIconButtonSize(const char* icon, const char* label)
{
	float lineHeight = ImGui::GetTextLineHeight();
	ImVec2 padding = ImGui::GetStyle().FramePadding;

	float width = ImGui::CalcTextSize(icon).x;
	width += ImGui::CalcTextSize(label).x;
	width += padding.x * 2.0f;

	return ImVec2(width, lineHeight + padding.y * 2.0f);
}

bool UI::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;

	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically ? ImVec2(thickness, splitter_long_axis_size) : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);

	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2, 0.0f);
}

bool UI::BeginToolbar(const char* id, ImVec2 screenPosition, ImVec2 size)
{
	static constexpr ImGuiWindowFlags flags =
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoSavedSettings;

	if (size.x == 0)
	{
		size.x = ImGui::GetContentRegionAvail().x;
	}

	ImGui::SetNextWindowPos(screenPosition);
	ImGui::SetNextWindowSize(size);

	return ImGui::BeginChild(id, size, false, flags);
}

void UI::EndToolbar()
{
	ImVec2 position = ImGui::GetWindowPos();
	ImVec2 size = ImGui::GetWindowSize();

	ImGui::EndChild();
}

bool UI::PlayerBar(const char* label, float* value, const float min, const float max, const ImVec2& sizeArg)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();

	if (window->SkipItems)
	{
		return false;
	}

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImRect bb = ImRect(pos, pos + ImGui::CalcItemSize(sizeArg, ImGui::CalcItemWidth(), g.FontSize + style.FramePadding.y * 2.0f));

	ImGui::ItemSize(bb, style.FramePadding.y);

	if (!ImGui::ItemAdd(bb, id))
	{
		return false;
	}

	const ImGuiItemFlags itemFlags = g.LastItemData.InFlags;
	const bool isItemHovered = ImGui::ItemHoverable(bb, id, itemFlags);

	if (isItemHovered && g.IO.MouseClicked[0])
	{
		ImGui::SetActiveID(id, window);
		ImGui::SetFocusID(id, window);
	}

	bool valueChanged = false;

	if (g.ActiveId == id)
	{
		if (g.ActiveIdSource == ImGuiInputSource_Mouse)
		{
			if (!g.IO.MouseDown[0])
			{
				ImGui::ClearActiveID();
			}
			else
			{
				const float normalizedClickPosition = ImClamp((g.IO.MousePos.x - bb.Min.x) / bb.GetWidth(), 0.0f, 1.0f);
				*value = ImLerp(min, max, normalizedClickPosition);
				valueChanged = true;
			}
		}
	}

	if (valueChanged)
	{
		ImGui::MarkItemEdited(id);
	}

	float fraction = ImSaturate((*value - min) / (max - min));

	ImGui::RenderFrame(bb.Min, bb.Max, ImGui::GetColorU32(ImGuiCol_FrameBg), true, style.FrameRounding);
	bb.Expand(ImVec2(-style.FrameBorderSize, -style.FrameBorderSize));
	ImGui::RenderRectFilledRangeH(window->DrawList, bb, ImGui::GetColorU32(ImGuiCol_PlotHistogram), 0.f, fraction, style.FrameRounding);

	return valueChanged;
}

void UI::ResourceExportPopup(bool& showPopup, std::shared_ptr<Resource> resource)
{
	if (showPopup)
	{
		ImGui::OpenPopup("Export Resource");
	}

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 modalSize = ImVec2(600, 400);
	ImVec2 centerPosition = ImVec2(
		viewport->GetCenter().x - modalSize.x / 2,
		viewport->GetCenter().y - modalSize.y / 2
	);

	ImGui::SetNextWindowSize(modalSize);
	ImGui::SetNextWindowPos(centerPosition);

	if (ImGui::BeginPopupModal("Export Resource", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static std::string outputFolderPath;
		const float windowWidth = ImGui::GetContentRegionAvail().x;
		const float buttonWidth = GetIconButtonSize(ICON_MDI_FOLDER, "").x;
		const float inputTextWidth = windowWidth - buttonWidth - ImGui::GetStyle().ItemSpacing.x;

		ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());
		ImGui::TextUnformatted("Ouput Folder Path");

		ImGui::PushItemWidth(inputTextWidth);
		ImGui::InputText("##OuputFolderPath", &outputFolderPath);
		ImGui::PopItemWidth();
		ImGui::SameLine();

		if (ImGui::Button(ICON_MDI_FOLDER))
		{
			outputFolderPath = FileDialog::OpenFolder();
		}

		ImGui::Spacing();
		ImGui::Spacing();

		static size_t selectedExportOptionIndex = 0;
		static const char* selectedExportOption = nullptr;

		if (resource->IsResourceLoaded())
		{
			ImGui::Text("Export as:");
			ImGui::Spacing();

			ImGui::BeginChild("ScrollingRegion", ImVec2(600, 200));

			const std::vector<std::string>& exportOptions = GetExportOptions(resource);

			if (!selectedExportOption)
			{
				selectedExportOption = exportOptions[0].c_str();
			}

			for (size_t i = 0; i < exportOptions.size(); ++i)
			{
				if (ImGui::RadioButton(exportOptions[i].c_str(), selectedExportOptionIndex == i))
				{
					selectedExportOptionIndex = i;
					selectedExportOption = exportOptions[i].c_str();
				}

				ImGui::Spacing();
				ImGui::Spacing();
			}

			ImGui::EndChild();
		}
		else
		{
			ImGui::Text("Loading available export options...");
		}

		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::Spacing();

		if (UI::IconButton("  " ICON_MDI_EXPORT, " Export "))
		{
			showPopup = false;

			ImGui::CloseCurrentPopup();

			if (!resource->IsResourceDeserialized())
			{
				resource->Deserialize();
			}

			const std::string fileName = ResourceUtility::GenerateFileName(resource);
			std::string outputPath;
			const std::string selectedExportOption2 = selectedExportOption;

			if (strncmp(selectedExportOption, "Folder", 6) == 0)
			{
				outputPath = std::format("{}\\{}", outputFolderPath, fileName);

				if (!std::filesystem::exists(outputPath))
				{
					std::filesystem::create_directory(outputPath);
				}
			}
			else
			{
				const std::string selectedExportOption2 = selectedExportOption;
				const size_t index = selectedExportOption2.find("(");
				const std::string extension = selectedExportOption2.substr(index + 1, selectedExportOption2.find(")") - index - 1);

				outputPath = std::format("{}\\{}{}", outputFolderPath, fileName, extension);
			}

			resource->Export(outputPath, selectedExportOption2);

			resource.reset();
		}

		const ImVec2 framePadding = ImGui::GetStyle().FramePadding;

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + framePadding.x);

		if (UI::IconButton("  " ICON_MDI_CLOSE, " Cancel "))
		{
			showPopup = false;

			ImGui::CloseCurrentPopup();

			resource.reset();
		}

		ImGui::PopFont();

		ImGui::EndPopup();
	}
}

const std::vector<std::string>& UI::GetExportOptions(std::shared_ptr<Resource> resource)
{
	static std::unordered_map<unsigned int, std::vector<std::string>> resourceTypeToExportOptions;

	static std::vector<std::string> fsbfExportOptions = { "Raw File (.FSBF)", "FSB File (.FSB)", "Folder (OGG Files)" };
	static std::vector<std::string> fsbfExportOptions2 = { "Raw File (.FSBF)", "FSB File (.FSB)", "Folder (WAV Files)" };
	static std::vector<std::string> fsbmExportOptions = { "Raw File (.FSBM)", "FSB File (.FSB)", "Folder (OGG Files)" };
	static std::vector<std::string> fsbmExportOptions2 = { "Raw File (.FSBM)", "FSB File (.FSB)", "Folder (WAV Files)" };
	static std::vector<std::string> fsbsExportOptions = { "Raw File (.FSBS)", "FSB File (.FSB)", "Folder (OGG Files)" };
	static std::vector<std::string> fsbsExportOptions2 = { "Raw File (.FSBS)", "FSB File (.FSB)", "Folder (WAV Files)" };

	static std::vector<std::string> swffExportOptions = { "Raw File (.SWFF)", "DDS File (*.DDS)" };
	static std::vector<std::string> swffExportOptions2 = { "Raw File (.SWFF)", "PNG File (*.PNG)" };
	static std::vector<std::string> swffExportOptions3 = { "Raw File (.SWFF)", "SWF File (*.SWF)" };

	if (resourceTypeToExportOptions.empty())
	{
		static std::vector<std::string> aibbExportOptions = { "Raw File (.AIBB)", "Json File (.JSON)" };
		static std::vector<std::string> cbluExportOptions = { "Raw File (.CBLU)", "Json File (.JSON)" };
		static std::vector<std::string> closExportOptions = { "Raw File (.CLOS)", "XML File (.XML)" };
		static std::vector<std::string> clotExportOptions = { "Raw File (.CLOT)", "Json File (.JSON)" };
		static std::vector<std::string> cpptExportOptions = { "Raw File (.CPPT)", "Json File (.JSON)" };
		static std::vector<std::string> chrtExportOptions = { "Raw File (.ChrT)", "Json File (.JSON)" };
		static std::vector<std::string> gfxfExportOptions = { "Raw File (.GFXF)", "Folder (GFX with DDS files)" };
		static std::vector<std::string> gidxExportOptions = { "Raw File (.GIDX)", "Json File (.JSON)" };
		static std::vector<std::string> locmExportOptions = { "Raw File (.LOCM)", "Json File (.JSON)" };
		static std::vector<std::string> locrExportOptions = { "Raw File (.LOCR)", "Json File (.JSON)" };
		static std::vector<std::string> mateExportOptions = { "Raw File (.MATE)", "Folder (JSON with HLSL files)" };
		static std::vector<std::string> matiExportOptions = { "Raw File (.MATI)", "Json File (.JSON)" };
		static std::vector<std::string> mattExportOptions = { "Raw File (.MATT)", "Json File (.JSON)" };
		static std::vector<std::string> mucbExportOptions = { "Raw File (.MUCB)", "Json File (.JSON)" };
		static std::vector<std::string> pkglExportOptions = { "Raw File (.PKGL)", "Json File (.JSON)" };
		static std::vector<std::string> sbpbExportOptions = { "Raw File (.SBPB)", "Json File (.JSON)" };
		static std::vector<std::string> sbpdExportOptions = { "Raw File (.SBPD)", "Json File (.JSON)" };
		static std::vector<std::string> scdaExportOptions = { "Raw File (.SCDA)", "Json File (.JSON)" };
		static std::vector<std::string> sdefExportOptions = { "Raw File (.SDEF)", "Json File (.JSON)" };
		static std::vector<std::string> tbluExportOptions = { "Raw File (.TBLU)", "Json File (.JSON)" };
		static std::vector<std::string> teliExportOptions = { "Raw File (.TELI)", "Json File (.JSON)" };
		static std::vector<std::string> tempExportOptions = { "Raw File (.TEMP)", "Json File (.JSON)" };
		static std::vector<std::string> textExportOptions = { "Raw File (.TEXT)", "DDS File (.DDS)", "PNG File (.PNG)", "BMP File (.BMP)", "TGA File (.TGA)" };
		static std::vector<std::string> videExportOptions = { "Raw File (.VIDE)", "BINK File (.BIK)" };
		static std::vector<std::string> wavbExportOptions = { "Raw File (.WAVB)", "Json File (.JSON)" };
		static std::vector<std::string> wbfxExportOptions = { "Raw File (.WBFX)", "Json File (.JSON)" };

		resourceTypeToExportOptions.insert(std::make_pair('AIBB', aibbExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('CBLU', cbluExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('CLOS', closExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('CLOT', clotExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('CPPT', cpptExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('ChrT', chrtExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('GFXF', gfxfExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('GIDX', gidxExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('LOCM', locmExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('LOCR', locrExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('MATE', mateExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('MATI', matiExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('MATT', mattExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('MUCB', mucbExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('PKGL', pkglExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('SBPB', sbpbExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('SBPD', sbpdExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('SCDA', scdaExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('SDEF', sdefExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('TBLU', tbluExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('TELI', teliExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('TEMP', tempExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('TEXT', textExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('VIDE', videExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('WAVB', wavbExportOptions));
		resourceTypeToExportOptions.insert(std::make_pair('WBFX', wbfxExportOptions));
	}

	const unsigned int resourceType = resource->GetResourceHeaderHeader().m_type;

	if (resourceType == 'FSBF' || resourceType == 'FSBM' || resourceType == 'FSBS')
	{
		std::shared_ptr<WaveBankFSB> waveBank = std::static_pointer_cast<WaveBankFSB>(resource);

		if (resourceType == 'FSBF')
		{
			if (waveBank->GetFormat() == FSB::Format::Vorbis)
			{
				return fsbfExportOptions;
			}
			else
			{
				return fsbfExportOptions2;
			}
		}
		else if (resourceType == 'FSBM')
		{
			if (waveBank->GetFormat() == FSB::Format::Vorbis)
			{
				return fsbmExportOptions;
			}
			else
			{
				return fsbmExportOptions2;
			}
		}
		else if (resourceType == 'FSBS')
		{
			if (waveBank->GetFormat() == FSB::Format::Vorbis)
			{
				return fsbsExportOptions;
			}
			else
			{
				return fsbsExportOptions2;
			}
		}
	}
	else if (resourceType == 'SWFF')
	{
		std::shared_ptr<FlashMovie> flashMovie = std::static_pointer_cast<FlashMovie>(resource);
		const FlashMovie::Format format = flashMovie->GetFormat();

		switch (format)
		{
			case FlashMovie::Format::DDS:
				return swffExportOptions;
			case FlashMovie::Format::PNG:
				return swffExportOptions2;
			case FlashMovie::Format::SWF:
				return swffExportOptions3;
		}
	}

	return resourceTypeToExportOptions[resourceType];
}
