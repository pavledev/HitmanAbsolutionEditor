#pragma once

#include <memory>

#include "imgui.h"

#include "ImGuizmo.h"

class Camera;

class TransformGizmo
{
public:
	TransformGizmo();
	void Tick();
	void Update();
	static bool IsPickingAllowed();
	void SetCamera(std::shared_ptr<Camera> camera);

private:
	ImGuizmo::OPERATION transformOperation;
	ImGuizmo::MODE transformSpace;
	std::shared_ptr<Camera> camera;
};
