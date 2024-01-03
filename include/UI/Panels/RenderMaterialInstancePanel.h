#pragma once

#include <unordered_map>

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/RenderMaterialInstance.h"

class RenderMaterialInstancePanel : public BasePanel
{
public:
	RenderMaterialInstancePanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialInstance> renderMaterialInstance);
	void Render() override;
	void OnResourceLoaded();
	void SetResource(std::shared_ptr<RenderMaterialInstance> renderMaterialInstance);

private:
	template <std::integral T>
	void Property(const char* name, T& value, T min = 0, T max = 0)
	{
		UI::Property(name, value, min, max, nullptr, false);

		//RenderRemoveModifierButton(index);
	}

	template <std::floating_point T>
	void Property(const char* name, T& value, T min = 0, T max = 0)
	{
		UI::Property(name, value, min, max, nullptr, false, 0.1f, "%.3f");

		//RenderRemoveModifierButton(index);
	}

	void RenderMaterialInfo();
	void RenderProperties();
	void RenderProperties(RenderMaterialInstance::Property& property);

	void RenderRemoveModifierButton(const unsigned int modifierIndex);

	std::shared_ptr<RenderMaterialInstance> renderMaterialInstance;
	std::vector<UI::TableColumn> tableColumns;
};
