#pragma once

#include <map>

#include "Utility/UI.h"
#include "BasePanel.h"
#include "UI/Panels/CppEntityPropertiesPanel.h"
#include "Resources/CppEntity.h"

class CppEntityPropertiesPanel : public BasePanel
{
public:
	CppEntityPropertiesPanel(const char* name, const char* icon, std::shared_ptr<CppEntity> cpptResource);
	void Render() override;
	void OnResourceLoaded();

private:
	struct EntityProperty
	{
		EntityProperty() = default;
		EntityProperty(const char* name);
		EntityProperty(const char* name, void* data);
		EntityProperty(const char* name, void* data, const char* description);
		EntityProperty(const char* name, void* data, const char* typeName, const char* description);

		std::string name;
		const char* typeName;
		const char* description;
		void* data;
		const IType* typeInfo;
	};

	void RenderEntityProperties();
	void RenderEntityProperties(std::map<std::string, EntityProperty>& entityProperties);

	template <std::integral T>
	void Property(EntityProperty& property, T& value, T min = 0, T max = 0)
	{
		UI::Property(property.name.c_str(), value, min, max, property.description, false);
	}

	template <std::floating_point T>
	void Property(EntityProperty& property, T& value, T min = 0, T max = 0)
	{
		UI::Property(property.name.c_str(), value, min, max, property.description, false, 0.1f, "%.3f");
	}

	void BoolProperty(EntityProperty& property);
	void SMatrix43Property(EntityProperty& property);
	void SVector2Property(EntityProperty& property);
	void SVector3Property(EntityProperty& property, std::function<void(SVector3&)> onValueChangeCallback = nullptr);
	void SColorRGBProperty(EntityProperty& property);
	void SColorRGBAProperty(EntityProperty& property);
	void SEntityTemplateReferenceProperty(EntityProperty& property);
	void SBodyPartDamageMultipliersProperty(EntityProperty& property);
	void SVisibilitySettingProperty(EntityProperty& property);
	void SSettingsParamMultiplierProperty(EntityProperty& property);
	void ZRuntimeResourceIDProperty(EntityProperty& property);
	void ZStringProperty(EntityProperty& property);
	void ZCurveProperty(EntityProperty& property);
	void ZGameTimeProperty(EntityProperty& property);
	void ZSpeakerLevelsProperty(EntityProperty& property);
	void TArrayProperty(EntityProperty& property);
	void EnumProperty(EntityProperty& property);

	std::shared_ptr<CppEntity> cpptResource;
	std::vector<UI::TableColumn> tableColumns;
};
