#pragma once

#include <string>

#include "imgui.h"

class BasePanel
{
public:
	BasePanel(const char* name, const char* icon, const bool open = true);
	virtual ~BasePanel() = default;
	const char* GetName() const;
	const char* GetIcon() const;
	const char* GetID() const;
	bool* GetOpen();
	bool Begin(ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None);
	void End() const;
	virtual void Render() = 0;
	void SetID(const std::string& id);
	void SetOpen(const bool open);

protected:
	std::string name;
	const char* icon;
	std::string id;
	bool open;
};
