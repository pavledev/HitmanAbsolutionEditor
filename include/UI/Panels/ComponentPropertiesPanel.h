#pragma once

#include "BasePanel.h"
#include "Resources/RenderPrimitive.h"

class ComponentPropertiesPanel : public BasePanel
{
public:
	ComponentPropertiesPanel(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive);
	void Render() override;

private:
	std::shared_ptr<RenderPrimitive> renderPrimitive;
};
