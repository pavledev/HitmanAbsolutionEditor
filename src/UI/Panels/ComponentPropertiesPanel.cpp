#include <IconsMaterialDesignIcons.h>

#include "Utility/UI.h"
#include "UI/Panels/ComponentPropertiesPanel.h"
#include "Rendering/Scene/Transform.h"
#include "Rendering/Scene/SceneRenderer.h"

ComponentPropertiesPanel::ComponentPropertiesPanel(const char* name, const char* icon) : BasePanel(name, icon)
{
}

void ComponentPropertiesPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	std::shared_ptr<Camera> camera = SceneRenderer::GetCamera();

	if (!camera)
	{
		End();

		return;
	}

	std::shared_ptr<Entity> selectedEntity = SceneRenderer::GetCamera()->GetSelectedEntity();

	if (!selectedEntity)
	{
		End();

		return;
	}

	const std::vector< std::shared_ptr<Component>>& components = selectedEntity->GetComponents();

	for (size_t i = 0; i < components.size(); ++i)
	{
		UI::RenderComponentProperties(components[i].get());
	}

	End();
}
