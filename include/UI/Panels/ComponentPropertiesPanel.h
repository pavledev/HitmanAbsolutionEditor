#pragma once

#include "BasePanel.h"
#include "Resources/RenderPrimitive.h"

class ComponentPropertiesPanel : public BasePanel
{
public:
	ComponentPropertiesPanel(const char* name, const char* icon);
	void Render() override;
};
