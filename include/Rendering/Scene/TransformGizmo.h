#pragma once

#include "imgui.h"

#include "ImGuizmo.h"

class TransformGizmo
{
public:
	TransformGizmo();
	void Tick();
	void Update();
	static bool IsPickingAllowed();

private:
	ImGuizmo::OPERATION transformOperation;
	ImGuizmo::MODE transformSpace;
};
