#pragma once

#include "BasePanel.h"
#include "Logger.h"

class ConsolePanel : public BasePanel
{
public:
	ConsolePanel(const char* name, const char* icon);
	void Render() override;
	void RenderButtons(const std::vector<Logger::Message>& messages);
	void RenderTable(const std::vector<Logger::Message>& messages);
	void FilterMessages(const std::vector<Logger::Message>& messages, std::vector<Logger::Message>& filteredMessages);
	ImColor GetTextColor(const Logger::Level level);
	static void CopyToClipboard(const std::string& string);

private:
	const char* types[4] = { "All", "Editor", "SDK", "Pipe" };
	const char* currentType;
	char message[256]{ "" };

	ImColor errorIconColor;
	ImColor warningIconColor;
	ImColor infoIconColor;
	ImVec4 backgroundColor;
	ImVec4 autoScrollIconColor;
	bool showErrorButtonBorder;
	bool showWarningButtonBorder;
	bool showInfoButtonBorder;
	bool showAutoScrollButtonBorder;
	bool errorButtonClicked;
	bool warningButtonClicked;
	bool infoButtonClicked;
	bool autoScrollButtonClicked;
};
