#include <imgui_internal.h>

#include <IconsMaterialDesignIcons.h>

#include "UI/Panels/ConsolePanel.h"
#include "Utility/UI.h"
#include "Logger.h"
#include "Editor.h"

ConsolePanel::ConsolePanel(const char* name, const char* icon) : BasePanel(name, icon)
{
	currentType = "All";

	errorIconColor = { 245, 89, 99, 255 };
	warningIconColor = { 254, 228, 160, 255 };
	infoIconColor = { 97, 177, 254, 255 };
	autoScrollIconColor = { 0.537f, 0.753f, 0.286f, 1.0f };
	backgroundColor = { 0.22f, 0.22f, 0.22f, 0.00f };
	showErrorButtonBorder = true;
	showWarningButtonBorder = true;
	showInfoButtonBorder = true;
	showAutoScrollButtonBorder = true;
	errorButtonClicked = false;
	warningButtonClicked = false;
	infoButtonClicked = false;
	autoScrollButtonClicked = false;
}

void ConsolePanel::Render()
{
	if (!Begin())
	{
		return;
	}

	Logger& logger = Logger::GetInstance();
	const std::vector<Logger::Message> messages = logger.GetMessages();

	ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());
	//ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 8.0f));
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Type");
	ImGui::SameLine();
	ImGui::PushItemWidth(100);

	if (ImGui::BeginCombo("##Type", currentType))
	{
		for (int i = 0; i < 4; ++i)
		{
			bool isSelected = strcmp(currentType, types[i]) == 0;

			if (ImGui::Selectable(types[i], isSelected))
			{
				currentType = types[i];
			}
		}

		ImGui::EndCombo();
	}

	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();

	RenderButtons(messages);

	ImGui::Spacing();

	std::vector<Logger::Message> filteredMessages;

	FilterMessages(messages, filteredMessages);

	if (filteredMessages.size() > 0)
	{
		RenderTable(filteredMessages);
	}
	else
	{
		RenderTable(messages);
	}

	ImGui::PopFont();

	End();
}

void ConsolePanel::RenderButtons(const std::vector<Logger::Message>& messages)
{
	Logger& logger = Logger::GetInstance();
	unsigned int errorMessageCount = 0;
	unsigned int warningMessageCount = 0;
	unsigned int infoMessageCount = 0;

	for (size_t i = 0; i < messages.size(); ++i)
	{
		if (messages[i].level == Logger::Level::Error)
		{
			++errorMessageCount;
		}
		else if (messages[i].level == Logger::Level::Warning)
		{
			++warningMessageCount;
		}
		else if (messages[i].level == Logger::Level::Info)
		{
			++infoMessageCount;
		}
	}

	std::string errorButtonText = std::format(" {} Error ", errorMessageCount);
	std::string warningButtonText = std::format(" {} Warning ", warningMessageCount);
	std::string infoButtonText = std::format(" {} Message ", infoMessageCount);

	if (errorMessageCount != 1)
	{
		errorButtonText.insert(errorButtonText.length() - 1, "s ");
	}

	if (warningMessageCount != 1)
	{
		warningButtonText.insert(warningButtonText.length() - 1, "s ");
	}

	if (infoMessageCount != 1)
	{
		infoButtonText.insert(infoButtonText.length() - 1, "s ");
	}

	if (!showErrorButtonBorder)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	}

	if (UI::IconButton("  " ICON_MDI_CLOSE_CIRCLE, errorButtonText.c_str(), errorIconColor, backgroundColor))
	{
		errorButtonClicked = true;
	}

	if (!showErrorButtonBorder)
	{
		ImGui::PopStyleVar(1);
	}

	if (errorButtonClicked)
	{
		showErrorButtonBorder = !showErrorButtonBorder;
		errorButtonClicked = false;
	}

	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();

	if (!showWarningButtonBorder)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	}

	if (UI::IconButton("  " ICON_MDI_ALERT, warningButtonText.c_str(), warningIconColor, backgroundColor))
	{
		warningButtonClicked = true;
	}

	if (!showWarningButtonBorder)
	{
		ImGui::PopStyleVar(1);
	}

	if (warningButtonClicked)
	{
		showWarningButtonBorder = !showWarningButtonBorder;
		warningButtonClicked = false;
	}

	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();

	if (!showInfoButtonBorder)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	}

	if (UI::IconButton("  " ICON_MDI_INFORMATION, infoButtonText.c_str(), infoIconColor, backgroundColor))
	{
		infoButtonClicked = true;
	}

	if (!showInfoButtonBorder)
	{
		ImGui::PopStyleVar(1);
	}

	if (infoButtonClicked)
	{
		showInfoButtonBorder = !showInfoButtonBorder;
		infoButtonClicked = false;
	}

	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();

	if (!showAutoScrollButtonBorder)
	{
		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
	}

	if (UI::IconButton("  " ICON_MDI_ARROW_DOWN, " Auto-scroll ", autoScrollIconColor, backgroundColor))
	{
		autoScrollButtonClicked = true;
	}

	if (!showAutoScrollButtonBorder)
	{
		ImGui::PopStyleVar(1);
	}

	if (autoScrollButtonClicked)
	{
		showAutoScrollButtonBorder = !showAutoScrollButtonBorder;
		autoScrollButtonClicked = false;
	}

	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();

	if (UI::IconButton("  " ICON_MDI_CLOSE, " Clear ", errorIconColor, backgroundColor))
	{
		logger.ClearAllMessages();
	}

	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();
	ImGui::Text("");
	ImGui::SameLine();

	std::string hint = std::format("{} Search message...", ICON_MDI_MAGNIFY);

	ImGui::PushItemWidth(-1);
	ImGui::InputTextWithHint("##SearchMessage", hint.c_str(), message, IM_ARRAYSIZE(message));
	ImGui::PopItemWidth();
}

void ConsolePanel::RenderTable(const std::vector<Logger::Message>& messages)
{
	static ImGuiTableFlags tableFlags =
		ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable
		| ImGuiTableFlags_Borders | ImGuiTableFlags_NoBordersInBody
		| ImGuiTableFlags_SizingFixedFit;
	static ImVector<int> selections;
	static float minRowHeight = 0.0f;
	static int columnCount = 5;

	if (ImGui::BeginTable("Table", columnCount, tableFlags))
	{
		ImGuiTableColumnFlags columnFlags = ImGuiTableColumnFlags_NoSort | ImGuiTableColumnFlags_NoHide;

		ImGui::TableSetupColumn("ID", columnFlags | ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Time", columnFlags | ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Process", columnFlags | ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Type", columnFlags | ImGuiTableColumnFlags_WidthFixed);
		ImGui::TableSetupColumn("Message", ImGuiTableColumnFlags_WidthStretch);

		ImGui::TableHeadersRow();

		ImGuiListClipper clipper;

		clipper.Begin(messages.size());

		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; ++row)
			{
				const Logger::Message* message = &messages[row];
				const bool isItemSelected = selections.contains(message->id);
				ImVec4 textColor = GetTextColor(message->level);

				ImGui::PushID(message->id);
				ImGui::TableNextRow(ImGuiTableRowFlags_None, minRowHeight);

				ImGui::TableSetColumnIndex(0);

				char label[32];

				sprintf_s(label, "%d", message->id);

				ImGuiSelectableFlags selectableFlags = ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowItemOverlap;

				ImGui::PushStyleColor(ImGuiCol_Text, textColor);

				if (ImGui::Selectable(label, isItemSelected, selectableFlags, ImVec2(0, minRowHeight)))
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (isItemSelected)
						{
							selections.find_erase_unsorted(message->id);
						}
						else
						{
							selections.push_back(message->id);
						}
					}
					else
					{
						selections.clear();
						selections.push_back(message->id);
					}
				}

				ImGui::PopStyleColor();

				if (ImGui::BeginPopupContextItem("tablePopup"))
				{
					std::string copySelectedMessageLabel = std::format("  {} Copy selected message(s) ", ICON_MDI_CONTENT_COPY);
					std::string copySelectedRowLabel = std::format("  {} Copy selected row(s) ", ICON_MDI_CONTENT_COPY);
					std::string clearMessageLabel = std::format("  {} Clear selected message(s) ", ICON_MDI_CLOSE);

					if (ImGui::MenuItem(copySelectedMessageLabel.c_str()))
					{
						std::string content;

						for (int i = 0; i < selections.size(); ++i)
						{
							int selection = selections[i];

							content += messages[selection].content + "\n";
						}

						CopyToClipboard(content);
					}

					if (ImGui::MenuItem(copySelectedRowLabel.c_str()))
					{
						std::string content;

						for (int i = 0; i < selections.size(); ++i)
						{
							int selection = selections[i];

							content += messages[selection].ToString() + "\n";
						}

						CopyToClipboard(content);
					}

					if (ImGui::MenuItem(clearMessageLabel.c_str()))
					{
						for (int i = 0; i < selections.size(); ++i)
						{
							int selection = selections[i];

							Logger::GetInstance().ClearMessage(selection);
						}
					}

					ImGui::EndPopup();
				}

				ImGui::TableSetColumnIndex(1);
				ImGui::TextColored(textColor, message->time.c_str());

				ImGui::TableSetColumnIndex(2);
				ImGui::TextColored(textColor, message->process.c_str());

				ImGui::TableSetColumnIndex(3);
				ImGui::TextColored(textColor, Logger::TypeToString(message->type));

				ImGui::TableSetColumnIndex(4);
				ImGui::TextColored(textColor, message->content.c_str());

				ImGui::PopID();
			}
		}

		ImGui::EndTable();

		if (showAutoScrollButtonBorder && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		{
			ImGui::SetScrollHereY(1.0f);
		}
	}
}

void ConsolePanel::FilterMessages(const std::vector<Logger::Message>& messages, std::vector<Logger::Message>& filteredMessages)
{
	for (size_t i = 0; i < messages.size(); ++i)
	{
		std::string type = Logger::TypeToString(messages[i].type);

		if (currentType != "All" && type != currentType)
		{
			continue;
		}

		if (!showErrorButtonBorder && messages[i].level == Logger::Level::Error)
		{
			continue;
		}

		if (!showWarningButtonBorder && messages[i].level == Logger::Level::Warning)
		{
			continue;
		}

		if (!showInfoButtonBorder && messages[i].level == Logger::Level::Info)
		{
			continue;
		}

		if (strcmp(message, "Search Message...") != 0 && !messages[i].content.contains(message))
		{
			continue;
		}

		filteredMessages.push_back(messages[i]);
	}
}

ImColor ConsolePanel::GetTextColor(const Logger::Level level)
{
	switch (level)
	{
	case Logger::Level::Info:
		return infoIconColor;
	case Logger::Level::Warning:
		return warningIconColor;
	case Logger::Level::Error:
		return errorIconColor;
	default:
		return ImVec4();
	}
}

void ConsolePanel::CopyToClipboard(const std::string& string)
{
	if (!OpenClipboard(nullptr))
	{
		return;
	}

	if (!EmptyClipboard())
	{
		return;
	}

	HGLOBAL globalData = GlobalAlloc(GMEM_MOVEABLE, string.size() + 1);

	if (!globalData)
	{
		CloseClipboard();

		return;
	}

	auto globalDataPtr = GlobalLock(globalData);

	if (!globalDataPtr)
	{
		CloseClipboard();
		GlobalFree(globalData);

		return;
	}

	memset(globalDataPtr, 0, string.size() + 1);
	memcpy(globalDataPtr, string.c_str(), string.size());

	GlobalUnlock(globalData);

	SetClipboardData(CF_TEXT, globalData);
	CloseClipboard();
}
