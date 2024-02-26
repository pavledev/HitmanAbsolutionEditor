#pragma once

#include "BasePanel.h"
#include "Resources/RenderPrimitive.h"

class Renderer3D;

class ComponentPropertiesPanel : public BasePanel
{
public:
	ComponentPropertiesPanel(const char* name, const char* icon);
	void Render() override;
	void SetRenderer3D(std::shared_ptr<Renderer3D> renderer3D);

private:
	std::shared_ptr<Renderer3D> renderer3D;
};
