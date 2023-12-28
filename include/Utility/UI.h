#pragma once

#include <functional>

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include "StringUtility.h"
#include "Rendering/Scene/Component.h"

struct STypeID;
struct SEntityTemplateReference;
class ZString;

class UI
{
public:
	struct TableColumn
	{
		const char* name;
		ImGuiTableColumnFlags flags;
		float size;
	};

	static bool IsWindowHovered(const ImGuiID windowID);
	static ImVec2 GetWindowContentRegionMin(ImGuiWindow* window);
	static Vector2 GetMousePositionRelativeToWindow();
	static Vector2 GetMousePositionRelativeToWindow(ImGuiWindow* window);

	static std::string GetPropertyID(const char* label);

	static bool BeginProperties(const char* tableName, const std::vector<TableColumn> tableColumns, const bool showHeaders = true, const ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp | ImGuiTableFlags_BordersInner | ImGuiTableFlags_BordersOuterH);
	static void EndProperties();
	static void BeginProperty(const char* label, const char* tooltip, bool rightAlignNextColumn = true);
	static void BeginProperty(std::string& label, const char* tooltip, bool rightAlignNextColumn = true);
	static bool BeginTreeNodeProperty(const char* label, const char* tooltip, bool shouldNodeBeOpenedByDefault = false);
	static void EndProperty();
	static void EndTreeNodeProperty(const bool isTreeNodeOpen);

	static bool StringProperty(const char* label, std::string& string, const char* tooltip = nullptr, const bool isDisabled = false);
	static bool StringProperty(std::string& label, std::string& string, const char* tooltip = nullptr, const bool isDisabled = false);
	static bool Property(const char* label, char* stringBuffer, size_t stringBufferSize, const char* tooltip = nullptr, const bool isDisabled = false, std::function<void(char*)> onTextChangeCallback = nullptr);

	template <std::integral T>
	static bool Property(const char* label, T& value, T min = 0, T max = 0, const char* tooltip = nullptr, const bool isDisabled = false)
	{
		BeginProperty(label, tooltip);

		bool modified;
		int dataType = ImGuiDataType_S32;

		if constexpr (std::is_signed_v<T>)
		{
			if constexpr (sizeof(T) == 1)
			{
				dataType = ImGuiDataType_S8;
			}
			else if constexpr (sizeof(T) == 2)
			{
				dataType = ImGuiDataType_S16;
			}
			else if constexpr (sizeof(T) == 4)
			{
				dataType = ImGuiDataType_S32;
			}
			else if constexpr (sizeof(T) == 8)
			{
				dataType = ImGuiDataType_S64;
			}
		}
		else
		{
			if constexpr (sizeof(T) == 1)
			{
				dataType = ImGuiDataType_U8;
			}
			else if constexpr (sizeof(T) == 2)
			{
				dataType = ImGuiDataType_U16;
			}
			else if constexpr (sizeof(T) == 4)
			{
				dataType = ImGuiDataType_U32;
			}
			else if constexpr (sizeof(T) == 8)
			{
				dataType = ImGuiDataType_U64;
			}
		}

		ImGui::BeginDisabled(isDisabled);

		if (max > min)
		{
			modified = ImGui::SliderScalar(GetPropertyID(label).c_str(), dataType, &value, &min, &max);
		}
		else
		{
			modified = ImGui::DragScalar(GetPropertyID(label).c_str(), dataType, &value);
		}

		ImGui::EndDisabled();

		EndProperty();

		return modified;
	}

	template <std::integral T>
	static bool Property(std::string& label, T& value, T min = 0, T max = 0, const char* tooltip = nullptr, const bool isDisabled = false)
	{
		BeginProperty(label, tooltip);

		bool modified;
		int dataType = ImGuiDataType_S32;

		if constexpr (std::is_signed_v<T>)
		{
			if constexpr (sizeof(T) == 1)
			{
				dataType = ImGuiDataType_S8;
			}
			else if constexpr (sizeof(T) == 2)
			{
				dataType = ImGuiDataType_S16;
			}
			else if constexpr (sizeof(T) == 4)
			{
				dataType = ImGuiDataType_S32;
			}
			else if constexpr (sizeof(T) == 8)
			{
				dataType = ImGuiDataType_S64;
			}
		}
		else
		{
			if constexpr (sizeof(T) == 1)
			{
				dataType = ImGuiDataType_U8;
			}
			else if constexpr (sizeof(T) == 2)
			{
				dataType = ImGuiDataType_U16;
			}
			else if constexpr (sizeof(T) == 4)
			{
				dataType = ImGuiDataType_U32;
			}
			else if constexpr (sizeof(T) == 8)
			{
				dataType = ImGuiDataType_U64;
			}
		}

		ImGui::BeginDisabled(isDisabled);

		if (max > min)
		{
			modified = ImGui::SliderScalar(GetPropertyID(label.c_str()).c_str(), dataType, &value, &min, &max);
		}
		else
		{
			modified = ImGui::DragScalar(GetPropertyID(label.c_str()).c_str(), dataType, &value);
		}

		ImGui::EndDisabled();

		EndProperty();

		return modified;
	}

	template <std::floating_point T>
	static bool Property(const char* label, T& value, T min = 0, T max = 0, const char* tooltip = nullptr, const bool isDisabled = false, float delta = 0.1f, const char* fmt = "%.3f")
	{
		BeginProperty(label, tooltip);

		bool modified;
		int dataType = ImGuiDataType_Float;

		if constexpr (sizeof(T) == 8)
		{
			dataType = ImGuiDataType_Double;
		}

		ImGui::BeginDisabled(isDisabled);

		if (max > min)
		{
			modified = ImGui::SliderScalar(GetPropertyID(label).c_str(), dataType, &value, &min, &max, fmt);
		}
		else
		{
			modified = ImGui::DragScalar(GetPropertyID(label).c_str(), dataType, &value, delta, nullptr, nullptr, fmt);
		}

		ImGui::EndDisabled();

		EndProperty();

		return modified;
	}

	template <std::floating_point T>
	static bool Property(std::string& label, T& value, T min = 0, T max = 0, const char* tooltip = nullptr, const bool isDisabled = false, float delta = 0.1f, const char* fmt = "%.3f")
	{
		BeginProperty(label, tooltip);

		bool modified;
		int dataType = ImGuiDataType_Float;

		if constexpr (sizeof(T) == 8)
		{
			dataType = ImGuiDataType_Double;
		}

		ImGui::BeginDisabled(isDisabled);

		if (max > min)
		{
			modified = ImGui::SliderScalar(GetPropertyID(label.c_str()).c_str(), dataType, &value, &min, &max, fmt);
		}
		else
		{
			modified = ImGui::DragScalar(GetPropertyID(label.c_str()).c_str(), dataType, &value, delta, nullptr, nullptr, fmt);
		}

		ImGui::EndDisabled();

		EndProperty();

		return modified;
	}

	template <typename T>
	static bool VectorProperty(const char* label, T& value, bool color = false, bool showAlpha = true, const char* tooltip = nullptr, float delta = 0.1f)
	{
		BeginProperty(label, tooltip);

		bool modified;
		int componentCount = T::ComponentCount();

		if (componentCount >= 3 && color)
		{
			if (showAlpha)
			{
				modified = ImGui::ColorEdit4(GetPropertyID(label).c_str(), &value.x);
			}
			else
			{
				modified = ImGui::ColorEdit3(GetPropertyID(label).c_str(), &value.x);
			}
		}
		else
		{
			modified = ImGui::DragScalarN(GetPropertyID(label).c_str(), ImGuiDataType_Float, &value.x, componentCount, delta);
		}

		EndProperty();

		return modified;
	}

	static bool Property(const char* label, bool& flag, const char* tooltip = nullptr);
	static bool Property(const char* label, std::vector<std::string>& flagStrings, std::vector<bool>& flags, const bool renderInSameLine = false, const char* tooltip = nullptr);

	static bool Property(const char* label, int& value, const char** dropdownStrings, int count, const char* tooltip = nullptr);

	static bool ColorRGBProperty(const char* label, Vector3& vector, const char* tooltip = nullptr);
	static bool ColorRGBAProperty(const char* label, Vector4& vector, const char* tooltip = nullptr);

	static void Vector2Property(const char* label, Vector2& vector, const char* tooltip = nullptr, float resetValue = 0.0f);
	static void Vector3Property(const char* label, Vector3& vector, const char* tooltip = nullptr, float resetValue = 0.0f, std::function<void(Vector3&)> onValueChangeCallback = nullptr);

	static bool IconButton(const char* icon, const char* label, ImVec4 iconColor = { 1.f, 1.f, 1.f, 1.0f }, ImVec4 backgroundColor = { 0.22f, 0.22f, 0.22f, 1.00f });
	static ImVec2 GetIconButtonSize(const char* icon, const char* label);

	static bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

	static void RenderComponentProperties(Component* component)
	{
		static constexpr ImGuiTreeNodeFlags treeFlags = ImGuiTreeNodeFlags_DefaultOpen
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_AllowItemOverlap
			| ImGuiTreeNodeFlags_Framed
			| ImGuiTreeNodeFlags_FramePadding;

		const float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() + lineHeight * 0.25f);
		ImGui::PushID(component);

		const bool open = ImGui::TreeNodeEx("Component", treeFlags, component->GetName().c_str());

		if (open)
		{
			component->RenderProperties();

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	static bool BeginToolbar(const char* id, ImVec2 screenPosition, ImVec2 size);
	static void EndToolbar();

	static bool PlayerBar(const char* label, float* value, const float min, const float max, const ImVec2& size);

private:
	inline static const char* tableName;
	inline static unsigned int propertyCounter = 0;
};
