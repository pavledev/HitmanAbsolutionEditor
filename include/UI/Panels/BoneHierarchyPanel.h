#pragma once

#include "BasePanel.h"
#include "Resources/RenderPrimitive.h"

class BoneHierarchyPanel : public BasePanel
{
public:
	BoneHierarchyPanel(const char* name, const char* icon, std::shared_ptr<RenderPrimitive> renderPrimitive);
	void Render() override;
	void RenderTree(unsigned int parentIndex);

private:
	std::shared_ptr<RenderPrimitive> renderPrimitive;
};
