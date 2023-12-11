#pragma once

#include <map>

#include "Utility/UI.h"
#include "BasePanel.h"
#include "UI/Panels/SceneHierarchyPanel.h"

class TemplateEntityPropertiesPanel : public BasePanel
{
public:
	using GoToEntityCallback = std::function<void(const std::shared_ptr<Resource> templateResource, const unsigned int entityIndex)>;

	TemplateEntityPropertiesPanel(const char* name, const char* icon, std::shared_ptr<TemplateEntity> tempResource);
	void Render() override;
	void OnSelectedEntityTreeNode(std::shared_ptr<SceneHierarchyPanel::EntityTreeNode> selectedEntityTreeNode);
	void SetGoToEntityCallback(GoToEntityCallback goToEntityCallback);

private:
	struct EntityProperty
	{
		EntityProperty() = default;
		EntityProperty(const char* name);
		EntityProperty(const char* name, void* data);
		EntityProperty(const char* name, void* data, const char* description);
		EntityProperty(const char* name, void* data, const char* typeName, const char* description);
		EntityProperty(const size_t index, const char* name, const unsigned int entityIndex, const size_t templateIndex, const size_t asetReferenceIndex, EntityProperty* parentProperty);
		EntityProperty(const size_t index, const char* name, void* data, const unsigned int entityIndex, const size_t templateIndex, const size_t asetReferenceIndex, EntityProperty* parentProperty);
		EntityProperty(const size_t index, const char* name, void* data, const char* typeName, const char* description, const unsigned int entityIndex, const size_t templateIndex, const size_t asetReferenceIndex, EntityProperty* parentProperty);

		unsigned int id;
		size_t index;
		std::string name;
		const char* typeName;
		const char* description;
		void* data;
		bool isPostInitProperty;
		unsigned int entityIndex;
		size_t templateIndex;
		size_t asetReferenceIndex;
		const IType* typeInfo;
		EntityProperty* parentProperty;
	};

	void RenderEntityProperties();
	void RenderEntityProperties(std::shared_ptr<TemplateEntity> tempResource, const int entityIndex, const size_t templateIndex, const size_t asetReferenceIndex, std::set<unsigned int>& visitedProperties);
	void RenderEntityProperties(std::map<std::string, EntityProperty>& entityProperties, std::set<unsigned int>& visitedProperties);

	template <std::integral T>
	void Property(EntityProperty& property, T& value, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false, T min = 0, T max = 0)
	{
		UI::Property(property.name.c_str(), value, min, max, property.description, isDisabled);

		if (addCheckboxToRow)
		{
			RenderPostInitCheckbox(property);
		}

		if (addButtonToRow)
		{
			RenderRemovePropertyButton(property);
		}
	}

	template <std::floating_point T>
	void Property(EntityProperty& property, T& value, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false, T min = 0, T max = 0)
	{
		UI::Property(property.name.c_str(), value, min, max, property.description, isDisabled, 0.1f, "%.3f");

		if (addCheckboxToRow)
		{
			RenderPostInitCheckbox(property);
		}

		if (addButtonToRow)
		{
			RenderRemovePropertyButton(property);
		}
	}

	void BoolProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SMatrix43Property(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SVector2Property(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SVector3Property(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false, std::function<void(SVector3&)> onValueChangeCallback = nullptr);
	void SColorRGBProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SColorRGBAProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SEntityTemplateReferenceProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SBodyPartDamageMultipliersProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SVisibilitySettingProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void SSettingsParamMultiplierProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void ZRuntimeResourceIDProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void ZStringProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void ZCurveProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void ZGameTimeProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void ZSpeakerLevelsProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void TArrayProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);
	void EnumProperty(EntityProperty& property, const bool addCheckboxToRow = true, const bool addButtonToRow = true, const bool isDisabled = false);

	void RenderPostInitCheckbox(EntityProperty& property);
	void RenderAddPropertyButton(EntityProperty& property);
	void RenderAddNewPropertyButton();
	void RenderAddNewItemButton(EntityProperty& property);
	void RenderRemovePropertyButton(EntityProperty& property);
	void RenderPropertyModal();

	void OnPostInitPropertyChange(EntityProperty& property);
	void OnAddPropertyFromExternalTemplate(EntityProperty& property);
	void OnRemoveProperty(EntityProperty& property);
	void AddNewProperty(const char* name, const char* typeName, const bool isPostInit);
	void RemoveItemFromArray(EntityProperty* arrayProperty, const size_t itemIndex);

	void FilterProperties(const std::string& propertyName);
	void FilterProperties(const std::string& propertyName, std::shared_ptr<TemplateEntity> tempResource, const int entityIndex);

	std::shared_ptr<TemplateEntity> tempResource;
	std::shared_ptr<SceneHierarchyPanel::EntityTreeNode> selectedEntityTreeNode;
	std::vector<UI::TableColumn> tableColumns;
	bool showPropertyModal;
	std::set<unsigned int> filteredProperties;
	GoToEntityCallback goToEntityCallback;
};
