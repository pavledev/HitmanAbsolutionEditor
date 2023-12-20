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

bool UI::BeginTreeNodeProperty(const char* label, const char* tooltip)
{
	const std::string id = std::format("{}_{}", tableName, propertyCounter);
	static constexpr ImGuiTreeNodeFlags treeNodeFlags =
		ImGuiTreeNodeFlags_DefaultOpen |
		ImGuiTreeNodeFlags_SpanAvailWidth |
		ImGuiTreeNodeFlags_FramePadding;

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

	const bool modified = ImGui::InputText(GetPropertyID(label).c_str(), &value);

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

	UI::EndProperty();

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
