#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/RenderMaterialEntityType.h"
#include "Resources/RenderMaterialEntityBlueprint.h"

class RenderMaterialEntityTypePanel : public BasePanel
{
public:
	RenderMaterialEntityTypePanel(const char* name, const char* icon, std::shared_ptr<RenderMaterialEntityType> renderMaterialEntityTypeResource);
	void Render() override;
	void OnResourceLoaded();

private:
	template <std::integral T>
	void Property(std::string& name, T& value, const size_t index, T min = 0, T max = 0)
	{
		UI::Property(name, value, min, max, nullptr, false);

		RenderRemoveModifierButton(index);
	}

	template <std::floating_point T>
	void Property(std::string& name, T& value, const size_t index, T min = 0, T max = 0)
	{
		UI::Property(name, value, min, max, nullptr, false, 0.1f, "%.3f");

		RenderRemoveModifierButton(index);
	}

	void RenderModifiers();

	void SColorRGBProperty(std::string& name, void* value, const size_t index);
	void SColorRGBAProperty(std::string& name, void* value, const size_t index);
	void SVector2Property(std::string& name, void* value, const size_t index);
	void SVector3Property(std::string& name, void* value, const size_t index);
	void SVector4Property(std::string& name, void* value, const size_t index);
	void StringProperty(std::string& name, void* value, const size_t index);

	void RenderRemoveModifierButton(const rsize_t modifierIndex);

	std::shared_ptr<RenderMaterialEntityType> renderMaterialEntityTypeResource;
	std::shared_ptr<RenderMaterialEntityBlueprint> renderMaterialEntityBlueprint;
	std::vector<UI::TableColumn> tableColumns;
};
