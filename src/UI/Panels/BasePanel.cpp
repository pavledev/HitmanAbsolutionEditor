#include <format>

#include "UI/Panels/BasePanel.h"
#include "Utility/StringUtility.h"

BasePanel::BasePanel(const char* name, const char* icon, const bool open) : icon(icon), open(open)
{
	this->name = std::format(" {} {}\t\t", icon, name);
}

const char* BasePanel::GetName() const
{
	return name.c_str();
}

const char* BasePanel::GetIcon() const
{
	return icon;
}

const char* BasePanel::GetID() const
{
	return id.c_str();
}

void BasePanel::Begin(ImGuiWindowFlags windowFlags)
{
	ImGui::SetNextWindowSize(ImVec2(480, 640), ImGuiCond_FirstUseEver);

	ImGui::Begin(id.c_str(), &open, windowFlags | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoFocusOnAppearing);
}

void BasePanel::End() const
{
	ImGui::End();
}

void BasePanel::SetID(const std::string& id)
{
	this->id = id;
}

void BasePanel::SetOpen(const bool open)
{
	this->open = open;
}
