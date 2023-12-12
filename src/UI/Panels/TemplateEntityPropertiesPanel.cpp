#include <IconsMaterialDesignIcons.h>

#include "Glacier/Math/SMatrix43.h"
#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/SBodyPartDamageMultipliers.h"
#include "Glacier/Entity/SEntityTemplateReference.h"
#include "Glacier/Resource/ZRuntimeResourceID.h"
#include "Glacier/TypeInfo/STypeID.h"
#include "Glacier/Sound/ZSpeakerLevels.h"
#include "Glacier/ZCurve.h"
#include "Glacier/ZGameTime.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/ECameraState.h"
#include "Glacier/ZSharedSensorDef.h"
#include "Glacier/SSettingsParamMultiplier.h"

#include "UI/Panels/TemplateEntityPropertiesPanel.h"
#include "Editor.h"
#include "Registry/PropertyRegistry.h"
#include "Registry/EnumRegistry.h"
#include "Registry/TypeRegistry.h"
#include "Resources/AspectEntityType.h"
#include "Resources/AspectEntityBlueprint.h"
#include "Hash.h"

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name) :
	name(name),
	data(nullptr),
	typeName(nullptr),
	description(nullptr),
	parentProperty(nullptr)
{
}

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name, void* data) :
	name(name),
	data(data),
	typeName(nullptr),
	description(nullptr),
	parentProperty(nullptr)
{
}

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name, void* data, const char* description) :
	name(name),
	data(data),
	typeName(nullptr),
	description(description),
	parentProperty(nullptr)
{
}

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name, void* data, const char* typeName, const char* description) :
	name(name),
	data(data),
	typeName(typeName),
	description(description),
	parentProperty(nullptr)
{
}

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const size_t index, const char* name, unsigned int entityIndex, size_t templateIndex, const size_t asetReferenceIndex, EntityProperty* parentProperty) :
	index(index),
	name(name),
	entityIndex(entityIndex),
	templateIndex(templateIndex),
	asetReferenceIndex(asetReferenceIndex),
	parentProperty(parentProperty)
{
}

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const size_t index, const char* name, void* data, unsigned int entityIndex, size_t templateIndex, const size_t asetReferenceIndex, EntityProperty* parentProperty) :
	index(index),
	name(name),
	data(data),
	entityIndex(entityIndex),
	templateIndex(templateIndex),
	asetReferenceIndex(asetReferenceIndex),
	parentProperty(parentProperty)
{
}

TemplateEntityPropertiesPanel::EntityProperty::EntityProperty(const size_t index, const char* name, void* data, const char* typeName, const char* description, unsigned int entityIndex, size_t templateIndex, const size_t asetReferenceIndex, EntityProperty* parentProperty) :
	index(index),
	name(name),
	data(data),
	typeName(typeName),
	description(description),
	entityIndex(entityIndex),
	templateIndex(templateIndex),
	asetReferenceIndex(asetReferenceIndex),
	parentProperty(parentProperty)
{
}

TemplateEntityPropertiesPanel::TemplateEntityPropertiesPanel(const char* name, const char* icon, std::shared_ptr<TemplateEntity> tempResource) : BasePanel(name, icon)
{
	this->tempResource = tempResource;
	showPropertyModal = false;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
	tableColumns.push_back({ "Post Init" , 0, 0.2f });
	tableColumns.push_back({ "" , 0, 0.2f });
}

void TemplateEntityPropertiesPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

	RenderEntityProperties();

	if (selectedEntityTreeNode)
	{
		ImGui::Spacing();
		ImGui::Spacing();

		RenderAddNewPropertyButton();
	}

	RenderPropertyModal();

	ImGui::PopFont();

	End();
}

void TemplateEntityPropertiesPanel::OnSelectedEntityTreeNode(std::shared_ptr<SceneHierarchyPanel::EntityTreeNode> selectedEntityTreeNode)
{
    this->selectedEntityTreeNode = selectedEntityTreeNode;
}

void TemplateEntityPropertiesPanel::SetGoToEntityCallback(GoToEntityCallback goToEntityCallback)
{
	this->goToEntityCallback = goToEntityCallback;
}

void TemplateEntityPropertiesPanel::RenderEntityProperties()
{
	if (!selectedEntityTreeNode)
	{
		return;
	}

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Entity Name");
	ImGui::SameLine();

	ImGui::PushItemWidth(-1);
	ImGui::InputText("##EntityName", &selectedEntityTreeNode->entityName);
	ImGui::PopItemWidth();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Template");
	ImGui::SameLine();

	ImGui::PushItemWidth(-1);
	ImGui::InputText("##TemplateResourceID", &selectedEntityTreeNode->templateResourceID);
	ImGui::PopItemWidth();

	ImGui::AlignTextToFramePadding();
	ImGui::Text("Blueprint");
	ImGui::SameLine();

	ImGui::PushItemWidth(-1);
	ImGui::InputText("##BlueprintResourceID", &selectedEntityTreeNode->blueprintResourceID);
	ImGui::PopItemWidth();

	ImGui::Spacing();
	ImGui::Spacing();

	static char propertyName[256]{ "" };
	static std::string propertyNameHint = std::format("{} Property Name...", ICON_MDI_MAGNIFY);
	std::string propertyName2;

	ImGui::PushItemWidth(-1);
	ImGui::InputTextWithHint("##PropertyName", propertyNameHint.c_str(), propertyName, IM_ARRAYSIZE(propertyName));
	ImGui::PopItemWidth();

	ImGui::Spacing();

	if (ImGui::Button("Search Property Name"))
	{
		if (strlen(propertyName) == 0)
		{
			propertyName2 = "";

			filteredProperties.clear();
		}
		else
		{
			propertyName2 = propertyName;

			std::transform(propertyName2.begin(), propertyName2.end(), propertyName2.begin(), tolower);

			FilterProperties(propertyName2);
		}
	}

	ImGui::Spacing();
	ImGui::Spacing();

	if (!UI::BeginProperties("Properties", tableColumns))
	{
		return;
	}

	std::set<unsigned int> visitedProperties;
	std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[0]);

	RenderEntityProperties(tempResource, selectedEntityTreeNode->entityIndex, 0, -1, visitedProperties);

	for (size_t i = 1; i < selectedEntityTreeNode->templateResources.size(); ++i)
	{
		if (selectedEntityTreeNode->templateResources[i]->GetResourceHeaderHeader().m_type == 'TEMP')
		{
			std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[i]);
			const int rootEntityIndex = tempResource->GetTemplateEntity()->rootEntityIndex;

			RenderEntityProperties(tempResource, rootEntityIndex, i, -1, visitedProperties);
		}
		else
		{
			std::shared_ptr<AspectEntityType> asetResource = std::static_pointer_cast<AspectEntityType>(selectedEntityTreeNode->templateResources[i]);
			const std::vector<std::shared_ptr<Resource>>& asetReferences = asetResource->GetReferences();

			for (size_t j = 0; j < asetReferences.size(); ++j)
			{
				if (asetReferences[i]->GetResourceHeaderHeader().m_type == 'TEMP')
				{
					std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(asetReferences[j]);
					const int rootEntityIndex = tempResource->GetTemplateEntity()->rootEntityIndex;

					RenderEntityProperties(tempResource, rootEntityIndex, i, j, visitedProperties);
				}
			}
		}
	}

	UI::EndProperties();
}

void TemplateEntityPropertiesPanel::RenderEntityProperties(std::shared_ptr<TemplateEntity> tempResource, const int entityIndex, const size_t templateIndex, const size_t asetReferenceIndex, std::set<unsigned int>& visitedProperties)
{
	TArray<SEntityTemplateProperty>& properties = tempResource->GetTemplateEntity()->entityTemplates[entityIndex].propertyValues;
	TArray<SEntityTemplateProperty>& postInitProperties = tempResource->GetTemplateEntity()->entityTemplates[entityIndex].postInitPropertyValues;
	std::map<std::string, EntityProperty> entityProperties;

	for (size_t i = 0; i < properties.Size(); ++i)
	{
		if (!filteredProperties.empty() && !filteredProperties.contains(properties[i].nPropertyID))
		{
			continue;
		}

		const IType* typeInfo = properties[i].value.GetTypeID()->pTypeInfo;
		const std::string& propertyName = PropertyRegistry::GetInstance().GetPropertyName(properties[i].nPropertyID);
		const char* propertyDescription = PropertyRegistry::GetInstance().GetPropertyDescription(properties[i].nPropertyID);
		void* data = properties[i].value.GetData();
		EntityProperty entityProperty;

		entityProperty.id = properties[i].nPropertyID;
		entityProperty.index = i;
		entityProperty.name = propertyName.c_str();
		entityProperty.typeName = typeInfo->GetTypeName();
		entityProperty.description = propertyDescription;
		entityProperty.data = data;
		entityProperty.isPostInitProperty = false;
		entityProperty.entityIndex = entityIndex;
		entityProperty.templateIndex = templateIndex;
		entityProperty.asetReferenceIndex = asetReferenceIndex;
		entityProperty.typeInfo = typeInfo;
		entityProperty.parentProperty = nullptr;

		entityProperties.insert(std::make_pair(propertyName, entityProperty));
	}

	for (size_t i = 0; i < postInitProperties.Size(); ++i)
	{
		if (!filteredProperties.empty() && !filteredProperties.contains(postInitProperties[i].nPropertyID))
		{
			continue;
		}

		const IType* typeInfo = postInitProperties[i].value.GetTypeID()->pTypeInfo;
		const std::string& propertyName = PropertyRegistry::GetInstance().GetPropertyName(postInitProperties[i].nPropertyID);
		const char* propertyDescription = PropertyRegistry::GetInstance().GetPropertyDescription(postInitProperties[i].nPropertyID);
		void* data = postInitProperties[i].value.GetData();
		EntityProperty entityProperty;

		entityProperty.id = postInitProperties[i].nPropertyID;
		entityProperty.index = i;
		entityProperty.name = propertyName.c_str();
		entityProperty.typeName = typeInfo->GetTypeName();
		entityProperty.description = propertyDescription;
		entityProperty.data = data;
		entityProperty.isPostInitProperty = true;
		entityProperty.entityIndex = entityIndex;
		entityProperty.templateIndex = templateIndex;
		entityProperty.asetReferenceIndex = asetReferenceIndex;
		entityProperty.typeInfo = typeInfo;
		entityProperty.parentProperty = nullptr;

		entityProperties.insert(std::make_pair(propertyName, entityProperty));
	}

	RenderEntityProperties(entityProperties, visitedProperties);
}

void TemplateEntityPropertiesPanel::RenderEntityProperties(std::map<std::string, EntityProperty>& entityProperties, std::set<unsigned int>& visitedProperties)
{
	for (auto it = entityProperties.begin(); it != entityProperties.end(); ++it)
	{
		EntityProperty& entityProperty = it->second;

		if (visitedProperties.contains(entityProperty.id))
		{
			if (entityProperty.templateIndex != 0)
			{
				continue;
			}
		}
		else
		{
			visitedProperties.insert(entityProperty.id);
		}

		const std::string typeName = entityProperty.typeName;
		const bool isDisabled = entityProperty.templateIndex != 0;

		if (typeName == "bool")
		{
			BoolProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "uint8")
		{
			Property(entityProperty, *static_cast<unsigned char*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "int8")
		{
			Property(entityProperty, *static_cast<char*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "uint16")
		{
			Property(entityProperty, *static_cast<unsigned short*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "int16")
		{
			Property(entityProperty, *static_cast<short*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "uint32")
		{
			Property(entityProperty, *static_cast<unsigned int*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "int32")
		{
			Property(entityProperty, *static_cast<int*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "uint64")
		{
			Property(entityProperty, *static_cast<unsigned long long*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "int64")
		{
			Property(entityProperty, *static_cast<long long*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "float32")
		{
			Property(entityProperty, *static_cast<float*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "float64")
		{
			Property(entityProperty, *static_cast<double*>(entityProperty.data), true, true, isDisabled);
		}
		else if (typeName == "SVector2")
		{
			SVector2Property(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "SVector3")
		{
			SVector3Property(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "SMatrix43")
		{
			SMatrix43Property(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "SColorRGB")
		{
			SColorRGBProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "SColorRGBA")
		{
			SColorRGBAProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "SEntityTemplateReference")
		{
			SEntityTemplateReferenceProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "SBodyPartDamageMultipliers")
		{
			SBodyPartDamageMultipliersProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "ZRuntimeResourceID")
		{
			ZRuntimeResourceIDProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "ZString")
		{
			ZStringProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "ZCurve")
		{
			ZCurveProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "ZGameTime")
		{
			ZGameTimeProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName == "ZSpeakerLevels")
		{
			ZSpeakerLevelsProperty(entityProperty, true, true, isDisabled);
		}
		else if (typeName.starts_with("TArray"))
		{
			TArrayProperty(entityProperty, true, true, isDisabled);
		}
		else if (entityProperty.typeInfo->IsEnumType())
		{
			EnumProperty(entityProperty, true, true, isDisabled);
		}
	}
}

void TemplateEntityPropertiesPanel::BoolProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	bool* value = static_cast<bool*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description);
	ImGui::BeginDisabled(isDisabled);

	ImGui::Checkbox(UI::GetPropertyID(property.name.c_str()).c_str(), value);

	ImGui::EndDisabled();
	UI::EndProperty();

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::SMatrix43Property(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		SMatrix43* matrix = static_cast<SMatrix43*>(property.data);
		SVector3 position;
		SVector3 rotation;
		SVector3 scale;

		matrix->Decompose(position, rotation, scale);

		EntityProperty positionProperty = EntityProperty("Position", &matrix->Trans);
		EntityProperty rotationProperty = EntityProperty("Rotation", &rotation);

		SVector3Property(positionProperty, false, false, isDisabled);

		SVector3Property(rotationProperty, false, false, isDisabled, [&](SVector3& newRotation)
		{
			*matrix = SMatrix43::Recompose(position, *reinterpret_cast<SVector3*>(&newRotation), scale);
		});
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::SVector2Property(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	SVector2* vector = static_cast<SVector2*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description, false);
	ImGui::BeginDisabled(isDisabled);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 2.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("X", buttonSize))
	{
		vector->x = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::DragFloat("##X", &vector->x, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector->y = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();
	ImGui::DragFloat("##Y", &vector->y, 0.1f, 0.0f, 0.0f, "%.2f");

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	ImGui::EndDisabled();
	UI::EndProperty();

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::SVector3Property(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled, std::function<void(SVector3&)> onValueChangeCallback)
{
	SVector3* vector = static_cast<SVector3*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description, false);
	ImGui::BeginDisabled(isDisabled);

	ImGuiIO& io = ImGui::GetIO();
	auto boldFont = io.Fonts->Fonts[1];

	ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());

	float frameHeight = ImGui::GetFrameHeight();
	ImVec2 buttonSize = { frameHeight + 3.0f, frameHeight };

	ImVec2 innerItemSpacing = ImGui::GetStyle().ItemInnerSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, innerItemSpacing);

	// X
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
	ImGui::PushFont(boldFont);

	bool isValueChanged = false;

	if (ImGui::Button("X", buttonSize))
	{
		vector->x = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##X", &vector->x, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Y
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Y", buttonSize))
	{
		vector->y = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Y", &vector->y, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::SameLine();

	// Z
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1.0f, 1.0f, 1.0f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
	ImGui::PushFont(boldFont);

	if (ImGui::Button("Z", buttonSize))
	{
		vector->z = 0.f;
	}

	ImGui::PopFont();
	ImGui::PopStyleColor(4);

	ImGui::SameLine();

	if (ImGui::DragFloat("##Z", &vector->z, 0.1f, 0.0f, 0.0f, "%.2f"))
	{
		isValueChanged = true;
	}

	ImGui::PopItemWidth();
	ImGui::PopStyleVar();

	ImGui::PopStyleVar();

	if (isValueChanged && onValueChangeCallback)
	{
		onValueChangeCallback(*vector);
	}

	ImGui::EndDisabled();
	UI::EndProperty();

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::SColorRGBProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	SColorRGB* colorRGB = static_cast<SColorRGB*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description);
	ImGui::BeginDisabled(isDisabled);

	ImGui::ColorEdit3(UI::GetPropertyID(property.name.c_str()).c_str(), &colorRGB->r);

	ImGui::EndDisabled();
	UI::EndProperty();

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::SColorRGBAProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	SColorRGBA* colorRGBA = static_cast<SColorRGBA*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description);
	ImGui::BeginDisabled(isDisabled);

	ImGui::ColorEdit4(UI::GetPropertyID(property.name.c_str()).c_str(), &colorRGBA->r);

	ImGui::EndDisabled();
	UI::EndProperty();

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::SEntityTemplateReferenceProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		SEntityTemplateReference* entityTemplateReference = static_cast<SEntityTemplateReference*>(property.data);
		EntityProperty entityIndexProperty = EntityProperty("entityIndex");
		EntityProperty exposedEntityProperty = EntityProperty("exposedEntity", &entityTemplateReference->exposedEntity);

		Property(entityIndexProperty, entityTemplateReference->entityIndex, false, false, isDisabled);
		ZStringProperty(exposedEntityProperty, false, false, isDisabled);

		if (entityTemplateReference->entityIndex != -1)
		{
			std::shared_ptr<TemplateEntityBlueprint> tbluResource;

			if (selectedEntityTreeNode->blueprintResources[property.templateIndex]->GetResourceHeaderHeader().m_type == 'TBLU')
			{
				tbluResource = std::static_pointer_cast<TemplateEntityBlueprint>(selectedEntityTreeNode->blueprintResources[property.templateIndex]);
			}
			else
			{
				std::shared_ptr<AspectEntityBlueprint> asebResource = std::static_pointer_cast<AspectEntityBlueprint>(selectedEntityTreeNode->blueprintResources[property.templateIndex]);
				const std::vector<std::shared_ptr<Resource>>& asebReferences = asebResource->GetReferences();
				tbluResource = std::static_pointer_cast<TemplateEntityBlueprint>(asebReferences[property.asetReferenceIndex]);
			}

			const char* entityName = tbluResource->GetTemplateEntityBlueprint()->entityTemplates[entityTemplateReference->entityIndex].entityName.ToCString();
			std::string buttonText = std::format(" Go to {} entity ", entityName);

			UI::BeginProperty("", nullptr);

			const float columnWidth = ImGui::GetColumnWidth();
			const ImVec2 iconButtonSize = UI::GetIconButtonSize("  " ICON_MDI_ARROW_TOP_RIGHT, buttonText.c_str());
			const float padding = (columnWidth - iconButtonSize.x) * 0.5f;

			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);

			if (UI::IconButton("  " ICON_MDI_ARROW_TOP_RIGHT, buttonText.c_str()))
			{
				goToEntityCallback(selectedEntityTreeNode->templateResources[property.templateIndex], entityTemplateReference->entityIndex);
			}

			UI::EndProperty();
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::SBodyPartDamageMultipliersProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		SBodyPartDamageMultipliers* bodyPartDamageMultipliers = static_cast<SBodyPartDamageMultipliers*>(property.data);
		EntityProperty headDamageMultiplierProperty = EntityProperty("m_fHeadDamageMultiplier");
		EntityProperty faceDamageMultiplierentityIndexProperty = EntityProperty("m_fFaceDamageMultiplier");
		EntityProperty armDamageMultiplierentityIndexProperty = EntityProperty("m_fArmDamageMultiplier");
		EntityProperty handDamageMultiplierentityIndexProperty = EntityProperty("m_fHandDamageMultiplier");
		EntityProperty legDamageMultiplierentityIndexProperty = EntityProperty("m_fLegDamageMultiplier");
		EntityProperty torsoDamageMultiplierentityIndexProperty = EntityProperty("m_fTorsoDamageMultiplier");

		Property(headDamageMultiplierProperty, bodyPartDamageMultipliers->m_fHeadDamageMultiplier, false, false, isDisabled);
		Property(faceDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fFaceDamageMultiplier, false, false, isDisabled);
		Property(armDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fArmDamageMultiplier, false, false, isDisabled);
		Property(handDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fHandDamageMultiplier, false, false, isDisabled);
		Property(legDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fLegDamageMultiplier, false, false, isDisabled);
		Property(torsoDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fTorsoDamageMultiplier, false, false, isDisabled);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::SVisibilitySettingProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		static PropertyRegistry& propertyRegistry = PropertyRegistry::GetInstance();
		static const char* sensitivityPropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_eSensitivity"));
		static const char* closeRangePropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fCloseRange"));
		static const char* peripheralRangeDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fPeripheralRange"));
		static const char* peripheralAnglePropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fPeripheralAngle"));
		static const char* peripheralHighPropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fPeripheralHigh"));
		static const char* peripheralLowPropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fPeripheralLow"));
		static const char* focusConeWidthAnglePropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fFocusConeWidthAngle"));
		static const char* focusConeHeightAnglePropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fFocusConeHeightAngle"));
		static const char* focusConeRangePropertyDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fFocusConeRange"));

		ZSharedSensorDef::SVisibilitySetting* visibilitySetting = static_cast<ZSharedSensorDef::SVisibilitySetting*>(property.data);
		EntityProperty sensitivityProperty = EntityProperty("m_eSensitivity", &visibilitySetting->m_eSensitivity, "EActorPerceptionSensitivity", sensitivityPropertyDescription);
		EntityProperty closeRangeProperty = EntityProperty("m_fCloseRange");
		EntityProperty peripheralRangeProperty = EntityProperty("m_fPeripheralRange");
		EntityProperty peripheralAngleProperty = EntityProperty("m_fPeripheralAngle");
		EntityProperty peripheralHighProperty = EntityProperty("m_fPeripheralHigh");
		EntityProperty peripheralLowProperty = EntityProperty("m_fPeripheralLow");
		EntityProperty focusConeWidthAngleProperty = EntityProperty("m_fFocusConeWidthAngle");
		EntityProperty focusConeHeightAngleProperty = EntityProperty("m_fFocusConeHeightAngle");
		EntityProperty focusConeRangeProperty = EntityProperty("m_fFocusConeRange");

		EnumProperty(sensitivityProperty, false, false, isDisabled);
		Property(closeRangeProperty, visibilitySetting->m_fCloseRange, false, false, isDisabled);
		Property(peripheralRangeProperty, visibilitySetting->m_fPeripheralRange, false, false, isDisabled);
		Property(peripheralAngleProperty, visibilitySetting->m_fPeripheralAngle, false, false, isDisabled);
		Property(peripheralHighProperty, visibilitySetting->m_fPeripheralHigh, false, false, isDisabled);
		Property(peripheralLowProperty, visibilitySetting->m_fPeripheralLow, false, false, isDisabled);
		Property(focusConeWidthAngleProperty, visibilitySetting->m_fFocusConeWidthAngle, false, false, isDisabled);
		Property(focusConeHeightAngleProperty, visibilitySetting->m_fFocusConeHeightAngle, false, false, isDisabled);
		Property(focusConeRangeProperty, visibilitySetting->m_fFocusConeRange, false, false, isDisabled);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::SSettingsParamMultiplierProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		static PropertyRegistry& propertyRegistry = PropertyRegistry::GetInstance();
		static const char* settingsParamDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_eSettingsParam"));
		static const char* multiplierDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fMultiplier"));

		SSettingsParamMultiplier* settingsParamMultiplier = static_cast<SSettingsParamMultiplier*>(property.data);
		EntityProperty settingsParamProperty = EntityProperty("m_eSettingsParam", &settingsParamMultiplier->m_eSettingsParam, "ESettingsParam", settingsParamDescription);
		EntityProperty multiplierProperty = EntityProperty("m_fMultiplier");

		EnumProperty(settingsParamProperty, false, false, isDisabled);
		Property(multiplierProperty, settingsParamMultiplier->m_fMultiplier, false, false, isDisabled);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::ZRuntimeResourceIDProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	ZRuntimeResourceID* runtimeResourceID = static_cast<ZRuntimeResourceID*>(property.data);
	static char stringBuffer[2048] = {};

	if (runtimeResourceID->GetID() != -1)
	{
		const std::string& resourceID = ZRuntimeResourceID::QueryResourceID(*runtimeResourceID);

		memcpy(stringBuffer, resourceID.c_str(), resourceID.length() + 1);
	}
	else
	{
		stringBuffer[0] = '\0';
	}

	UI::Property(property.name.c_str(), stringBuffer, sizeof(stringBuffer), property.description, isDisabled, [&](char* newString)
	{
		*runtimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID(stringBuffer);
	});

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::ZStringProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	ZString* string = static_cast<ZString*>(property.data);
	static char stringBuffer[2048] = {};

	memcpy(stringBuffer, string->ToCString(), string->Length() + 1);

	UI::Property(property.name.c_str(), stringBuffer, sizeof(stringBuffer), property.description, isDisabled, [&](char* newString)
	{
		*string = newString;
	});

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::ZCurveProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		ZCurve* curve = static_cast<ZCurve*>(property.data);
		TArray<TFixedArray<float, 8>>& data = curve->GetData();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			std::string partIndex = std::to_string(i);
			const bool isChildTreeNodeOpen = UI::BeginTreeNodeProperty(partIndex.c_str(), nullptr);

			if (addButtonToRow)
			{
				RenderRemovePropertyButton(property);
			}

			if (isChildTreeNodeOpen)
			{
				for (size_t j = 0; j < data[i].Size(); ++j)
				{
					std::string valueIndex = std::to_string(j);
					EntityProperty property2 = EntityProperty(valueIndex.c_str());

					Property(property2, data[i][j], false, true, isDisabled);
				}
			}

			UI::EndTreeNodeProperty(isChildTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::ZGameTimeProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		ZGameTime* gameTime = static_cast<ZGameTime*>(property.data);
		EntityProperty ticksProperty = EntityProperty("m_nTicks");

		Property(ticksProperty, gameTime->GetTicks(), false, false, isDisabled);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::ZSpeakerLevelsProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		ZSpeakerLevels* speakerLevels = static_cast<ZSpeakerLevels*>(property.data);
		TArray<float>& speakerMatrix = speakerLevels->GetSpeakerMatrix();

		for (size_t i = 0; i < speakerMatrix.Size(); ++i)
		{
			std::string index = std::to_string(i);
			EntityProperty property2 = EntityProperty(index.c_str());

			Property(property2, speakerMatrix[i], false, true, isDisabled);
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::TArrayProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}

	if (isTreeNodeOpen && property.data)
	{
		const std::string typeName = property.typeName;
		const bool addButtonToRow = property.templateIndex == 0;

		if (typeName == "TArray<SEntityTemplateReference>")
		{
			TArray<SEntityTemplateReference>* entityTemplateReferences = static_cast<TArray<SEntityTemplateReference>*>(property.data);

			for (size_t i = 0; i < entityTemplateReferences->Size(); ++i)
			{
				std::string treeNodeName = std::format("Entity Template Reference {}", i);
				EntityProperty entityTemplateReferenceProperty = EntityProperty(i, treeNodeName.c_str(), &(*entityTemplateReferences)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				SEntityTemplateReferenceProperty(entityTemplateReferenceProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<float32>")
		{
			TArray<float>* floats = static_cast<TArray<float>*>(property.data);

			for (size_t i = 0; i < floats->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty floatProperty = EntityProperty(i, index.c_str(), property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				Property(floatProperty, (*floats)[i], false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<ZGameTime>")
		{
			TArray<ZGameTime>* gameTimes = static_cast<TArray<ZGameTime>*>(property.data);

			for (size_t i = 0; i < gameTimes->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty gameTimeProperty = EntityProperty(i, index.c_str(), &(*gameTimes)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				ZGameTimeProperty(gameTimeProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<SVector2>")
		{
			TArray<SVector2>* vectors = static_cast<TArray<SVector2>*>(property.data);

			for (size_t i = 0; i < vectors->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty vectorProperty = EntityProperty(i, index.c_str(), &(*vectors)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				SVector2Property(vectorProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<bool>")
		{
			TArray<bool>* bools = static_cast<TArray<bool>*>(property.data);

			for (size_t i = 0; i < bools->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty boolProperty = EntityProperty(i, index.c_str(), &(*bools)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				BoolProperty(boolProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
		{
			TArray<ZSharedSensorDef::SVisibilitySetting>* visibilitySettings = static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(property.data);

			for (size_t i = 0; i < visibilitySettings->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty visibilitySettingProperty = EntityProperty(i, index.c_str(), &(*visibilitySettings)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				SVisibilitySettingProperty(visibilitySettingProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<ZString>")
		{
			TArray<ZString>* strings = static_cast<TArray<ZString>*>(property.data);

			for (size_t i = 0; i < strings->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty stringProperty = EntityProperty(i, index.c_str(), &(*strings)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				ZStringProperty(stringProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<SSettingsParamMultiplier>")
		{
			TArray<SSettingsParamMultiplier>* visibilitySettings = static_cast<TArray<SSettingsParamMultiplier>*>(property.data);

			for (size_t i = 0; i < visibilitySettings->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty settingsParamMultiplierProperty = EntityProperty(i, index.c_str(), &(*visibilitySettings)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				SSettingsParamMultiplierProperty(settingsParamMultiplierProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<SColorRGB>")
		{
			TArray<SColorRGB>* colors = static_cast<TArray<SColorRGB>*>(property.data);

			for (size_t i = 0; i < colors->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty colorRGBProperty = EntityProperty(i, index.c_str(), &(*colors)[i], property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				SColorRGBProperty(colorRGBProperty, false, addButtonToRow, isDisabled);
			}
		}
		else if (typeName == "TArray<ECameraState>")
		{
			TArray<ECameraState>* cameraStates = static_cast<TArray<ECameraState>*>(property.data);

			for (size_t i = 0; i < cameraStates->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty cameraStateProperty = EntityProperty(i, index.c_str(), &(*cameraStates)[i], property.typeName, nullptr, property.entityIndex, property.templateIndex, property.asetReferenceIndex, &property);

				EnumProperty(cameraStateProperty, false, addButtonToRow, isDisabled);
			}
		}

		RenderAddNewItemButton(property);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void TemplateEntityPropertiesPanel::EnumProperty(EntityProperty& property, const bool addCheckboxToRow, const bool addButtonToRow, const bool isDisabled)
{
	int* value = static_cast<int*>(property.data);
	const std::map<int, std::string>& enumItems = EnumRegistry::GetInstance().GetEnum(property.typeName);

	std::string currentValue;

	for (auto it = enumItems.begin(); it != enumItems.end(); ++it)
	{
		if (it->first == *value)
		{
			currentValue = it->second;

			break;
		}
	}

	UI::BeginProperty(property.name.c_str(), property.description);

	ImGui::BeginDisabled(isDisabled);

	if (ImGui::BeginCombo(UI::GetPropertyID(property.name.c_str()).c_str(), currentValue.c_str()))
	{
		for (auto it = enumItems.begin(); it != enumItems.end(); ++it)
		{
			bool isSelected = it->first == *value;

			if (ImGui::Selectable(it->second.c_str(), isSelected))
			{
				*value = it->first;
			}

			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	ImGui::EndDisabled();

	UI::EndProperty();

	if (addCheckboxToRow)
	{
		ImGui::BeginDisabled(isDisabled);

		RenderPostInitCheckbox(property);

		ImGui::EndDisabled();
	}
	else
	{
		ImGui::TableNextColumn();
	}

	if (addButtonToRow)
	{
		if (property.templateIndex == 0)
		{
			RenderRemovePropertyButton(property);
		}
		else
		{
			RenderAddPropertyButton(property);
		}
	}
}

void TemplateEntityPropertiesPanel::RenderPostInitCheckbox(EntityProperty& property)
{
	ImGui::TableNextColumn();

	const std::string checkboxID = UI::GetPropertyID("Checkbox");
	const float columnWidth = ImGui::GetColumnWidth();
	const float checkboxSize = ImGui::GetFrameHeight();
	const float paddingSide = (columnWidth - checkboxSize) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingSide);

	if (ImGui::Checkbox(checkboxID.c_str(), &property.isPostInitProperty))
	{
		OnPostInitPropertyChange(property);
	}
}

void TemplateEntityPropertiesPanel::RenderAddPropertyButton(EntityProperty& property)
{
	ImGui::TableNextColumn();

	const float checkboxWidth = ImGui::GetFrameHeight();
	const ImVec2 buttonSize = { checkboxWidth , 0.f };
	const float columnWidth = ImGui::GetColumnWidth();
	const float padding = (columnWidth - buttonSize.x) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);

	const std::string buttonID = UI::GetPropertyID("Button");

	ImGui::PushID(buttonID.c_str());

	if (ImGui::Button(ICON_MDI_PLUS, buttonSize))
	{
		OnAddPropertyFromExternalTemplate(property);
	}

	ImGui::PopID();
}

void TemplateEntityPropertiesPanel::RenderAddNewPropertyButton()
{
	const ImVec2 windowSize = ImGui::GetWindowSize();
	const ImVec2 iconButtonSize = UI::GetIconButtonSize("  " ICON_MDI_PLUS, " Add New Property ");
	const float centerPositionX = (windowSize.x - iconButtonSize.x) * 0.5f;

	ImGui::SetCursorPosX(centerPositionX);

	if (UI::IconButton("  " ICON_MDI_PLUS, " Add New Property ", ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0)))
	{
		showPropertyModal = true;
	}
}

void TemplateEntityPropertiesPanel::RenderAddNewItemButton(EntityProperty& property)
{
	UI::BeginProperty("", nullptr);

	const float columnWidth = ImGui::GetColumnWidth();
	const ImVec2 iconButtonSize = UI::GetIconButtonSize("  " ICON_MDI_PLUS, " Add New Item ");
	const float padding = (columnWidth - iconButtonSize.x) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + padding);

	if (UI::IconButton("  " ICON_MDI_PLUS, " Add New Item ", ImVec4(1, 1, 1, 1), ImVec4(0, 0, 0, 0)))
	{
		const std::string typeName = property.typeName;

		if (typeName == "TArray<SEntityTemplateReference>")
		{
			TArray<SEntityTemplateReference>* entityTemplateReferences = static_cast<TArray<SEntityTemplateReference>*>(property.data);

			entityTemplateReferences->PushBack(SEntityTemplateReference());
		}
		else if (typeName == "TArray<float32>")
		{
			TArray<float>* floats = static_cast<TArray<float>*>(property.data);

			floats->PushBack(0.f);
		}
		else if (typeName == "TArray<ZGameTime>")
		{
			TArray<ZGameTime>* gameTimes = static_cast<TArray<ZGameTime>*>(property.data);

			gameTimes->PushBack(ZGameTime());
		}
		else if (typeName == "TArray<SVector2>")
		{
			TArray<SVector2>* vectors = static_cast<TArray<SVector2>*>(property.data);

			vectors->PushBack(SVector2());
		}
		else if (typeName == "TArray<bool>")
		{
			TArray<bool>* bools = static_cast<TArray<bool>*>(property.data);

			bools->PushBack(false);
		}
		else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
		{
			TArray<ZSharedSensorDef::SVisibilitySetting>* visibilitySettings = static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(property.data);

			visibilitySettings->PushBack(ZSharedSensorDef::SVisibilitySetting());
		}
		else if (typeName == "TArray<ZString>")
		{
			TArray<ZString>* strings = static_cast<TArray<ZString>*>(property.data);

			strings->PushBack(ZString());
		}
		else if (typeName == "TArray<SSettingsParamMultiplier>")
		{
			TArray<SSettingsParamMultiplier>* visibilitySettings = static_cast<TArray<SSettingsParamMultiplier>*>(property.data);

			visibilitySettings->PushBack(SSettingsParamMultiplier());
		}
		else if (typeName == "TArray<SColorRGB>")
		{
			TArray<SColorRGB>* colors = static_cast<TArray<SColorRGB>*>(property.data);

			colors->PushBack(SColorRGB());
		}
		else if (typeName == "TArray<ECameraState>")
		{
			TArray<ECameraState>* cameraStates = static_cast<TArray<ECameraState>*>(property.data);

			cameraStates->PushBack(ECameraState::eCamSneakStand);
		}
	}

	UI::EndProperty();
}

void TemplateEntityPropertiesPanel::RenderRemovePropertyButton(EntityProperty& property)
{
	ImGui::TableNextColumn();

	const float checkboxWidth = ImGui::GetFrameHeight();
	const ImVec2 buttonSize = { checkboxWidth , 0.f };
	const float columnWidth = ImGui::GetColumnWidth();
	const float paddingSide = (columnWidth - buttonSize.x) * 0.5f;

	ImGui::SetCursorPosX(ImGui::GetCursorPosX() + paddingSide);

	const std::string buttonID = UI::GetPropertyID("Button");

	ImGui::PushID(buttonID.c_str());

	if (ImGui::Button(ICON_MDI_CLOSE, buttonSize))
	{
		OnRemoveProperty(property);
	}

	ImGui::PopID();
}

void TemplateEntityPropertiesPanel::RenderPropertyModal()
{
	if (showPropertyModal)
	{
		ImGui::OpenPopup("Add New Property");
	}

	ImVec2 center = ImGui::GetMainViewport()->GetCenter();

	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal("Add New Property", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::AlignTextToFramePadding();
		ImGui::Text("Name");
		ImGui::SameLine();

		static char propertyName[100]{ "" };
		bool isInputTextEnterPressed = ImGui::InputText("##PropertyName", propertyName, sizeof(propertyName), ImGuiInputTextFlags_EnterReturnsTrue);
		bool isInputTextActive = ImGui::IsItemActive();

		if (ImGui::IsItemActivated())
		{
			ImGui::OpenPopup("##PropertyNamePopup");
		}

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

		if (ImGui::BeginPopup("##PropertyNamePopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
		{
			const std::unordered_map<unsigned int, PropertyRegistry::Property>& properties = PropertyRegistry::GetInstance().GetProperties();

			for (auto it = properties.begin(); it != properties.end(); ++it)
			{
				const std::string& propertyName2 = it->second.name;

				if (!propertyName2.contains(propertyName))
				{
					continue;
				}

				if (ImGui::Selectable(propertyName2.c_str()))
				{
					ImGui::ClearActiveID();
					strcpy_s(propertyName, propertyName2.c_str());
				}
			}

			if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		ImGui::AlignTextToFramePadding();
		ImGui::Text("Type Name");
		ImGui::SameLine();

		static char typeName[100]{ "" };
		isInputTextEnterPressed = ImGui::InputText("##TypeName", typeName, sizeof(typeName), ImGuiInputTextFlags_EnterReturnsTrue);
		isInputTextActive = ImGui::IsItemActive();

		if (ImGui::IsItemActivated())
		{
			ImGui::OpenPopup("##TypeNamePopup");
		}

		ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
		ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

		if (ImGui::BeginPopup("##TypeNamePopup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
		{
			const std::unordered_map<std::string, STypeID*>& typeIDs = TypeRegistry::GetInstance().GetTypeIDs();
			const std::unordered_map<std::string, std::map<int, std::string>>& enums = EnumRegistry::GetInstance().GetEnums();

			for (auto it = typeIDs.begin(); it != typeIDs.end(); ++it)
			{
				const std::string& typeNameName2 = it->first;

				if (!typeNameName2.contains(typeName))
				{
					continue;
				}

				if (ImGui::Selectable(typeNameName2.c_str()))
				{
					ImGui::ClearActiveID();
					strcpy_s(typeName, typeNameName2.c_str());
				}
			}

			for (auto it = enums.begin(); it != enums.end(); ++it)
			{
				const std::string& typeNameName2 = it->first;

				if (!typeNameName2.contains(typeName))
				{
					continue;
				}

				if (ImGui::Selectable(typeNameName2.c_str()))
				{
					ImGui::ClearActiveID();
					strcpy_s(typeName, typeNameName2.c_str());
				}
			}

			if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		static bool isPostInitProperty = false;

		ImGui::Checkbox(" Post Init", &isPostInitProperty);
		ImGui::Spacing();
		ImGui::Spacing();

		if (UI::IconButton("  " ICON_MDI_CHECK, " Ok "))
		{
			showPropertyModal = false;

			AddNewProperty(propertyName, typeName, isPostInitProperty);

			ImGui::CloseCurrentPopup();
		}

		const ImVec2 framePadding = ImGui::GetStyle().FramePadding;

		ImGui::SameLine();
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() + framePadding.x);

		if (UI::IconButton("  " ICON_MDI_CLOSE, " Cancel "))
		{
			showPropertyModal = false;

			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void TemplateEntityPropertiesPanel::OnPostInitPropertyChange(EntityProperty& property)
{
	std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[property.templateIndex]);
	TArray<SEntityTemplateProperty>& properties = tempResource->GetTemplateEntity()->entityTemplates[property.entityIndex].propertyValues;
	TArray<SEntityTemplateProperty>& postInitProperties = tempResource->GetTemplateEntity()->entityTemplates[property.entityIndex].postInitPropertyValues;

	if (property.isPostInitProperty)
	{
		postInitProperties.PushBack(properties[property.index]);
		properties.RemoveAt(property.index);
	}
	else
	{
		properties.PushBack(postInitProperties[property.index]);
		postInitProperties.RemoveAt(property.index);
	}
}

void TemplateEntityPropertiesPanel::OnAddPropertyFromExternalTemplate(EntityProperty& property)
{
	std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[0]);
	std::shared_ptr<TemplateEntity> externalTempResource;

	if (selectedEntityTreeNode->templateResources[property.templateIndex]->GetResourceHeaderHeader().m_type == 'TEMP')
	{
		externalTempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[property.templateIndex]);
	}
	else
	{
		std::shared_ptr<AspectEntityType> asetResource = std::static_pointer_cast<AspectEntityType>(selectedEntityTreeNode->templateResources[property.templateIndex]);
		const std::vector<std::shared_ptr<Resource>>& asetReferences = asetResource->GetReferences();
		externalTempResource = std::static_pointer_cast<TemplateEntity>(asetReferences[property.asetReferenceIndex]);
	}

	if (property.isPostInitProperty)
	{
		TArray<SEntityTemplateProperty>& tempPostInitProperties = tempResource->GetTemplateEntity()->entityTemplates[selectedEntityTreeNode->entityIndex].postInitPropertyValues;
		TArray<SEntityTemplateProperty>& externalTempPostInitProperties = externalTempResource->GetTemplateEntity()->entityTemplates[property.entityIndex].postInitPropertyValues;

		tempPostInitProperties.PushBack(externalTempPostInitProperties[property.index]);
	}
	else
	{
		TArray<SEntityTemplateProperty>& tempProperties = tempResource->GetTemplateEntity()->entityTemplates[selectedEntityTreeNode->entityIndex].propertyValues;
		TArray<SEntityTemplateProperty>& externalTempProperties = externalTempResource->GetTemplateEntity()->entityTemplates[property.entityIndex].propertyValues;

		tempProperties.PushBack(externalTempProperties[property.index]);
	}
}

void TemplateEntityPropertiesPanel::OnRemoveProperty(EntityProperty& property)
{
	if (property.parentProperty)
	{
		RemoveItemFromArray(property.parentProperty, property.index);
	}
	else
	{
		std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[property.templateIndex]);

		if (property.isPostInitProperty)
		{
			TArray<SEntityTemplateProperty>& postInitProperties = tempResource->GetTemplateEntity()->entityTemplates[property.entityIndex].postInitPropertyValues;

			postInitProperties.RemoveAt(property.index);
		}
		else
		{
			TArray<SEntityTemplateProperty>& properties = tempResource->GetTemplateEntity()->entityTemplates[property.entityIndex].propertyValues;

			properties.RemoveAt(property.index);
		}
	}

	property.data = nullptr;
}

void TemplateEntityPropertiesPanel::AddNewProperty(const char* name, const char* typeName, const bool isPostInit)
{
	if (strlen(name) == 0 || strlen(typeName) == 0)
	{
		return;
	}

	SEntityTemplateProperty entityTemplateProperty;
	STypeID* typeID = TypeRegistry::GetInstance().GetTypeID(typeName);

	if (!typeID)
	{
		typeID = new STypeID();

		typeID->flags = 0;
		typeID->typeNum = -1;

		IType* type = new IType(4, 4, 8, typeName, typeID);

		typeID->pTypeInfo = type;

		TypeRegistry::GetInstance().RegisterType(typeID);
	}

	entityTemplateProperty.nPropertyID = Hash::Crc32(name);
	entityTemplateProperty.value.Allocate(typeID);

	std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[0]);

	if (isPostInit)
	{
		TArray<SEntityTemplateProperty>& properties = tempResource->GetTemplateEntity()->entityTemplates[selectedEntityTreeNode->entityIndex].propertyValues;

		properties.PushBack(entityTemplateProperty);
	}
	else
	{
		TArray<SEntityTemplateProperty>& tempPostInitProperties = tempResource->GetTemplateEntity()->entityTemplates[selectedEntityTreeNode->entityIndex].propertyValues;

		tempPostInitProperties.PushBack(entityTemplateProperty);
	}
}

void TemplateEntityPropertiesPanel::RemoveItemFromArray(EntityProperty* arrayProperty, const size_t itemIndex)
{
	const std::string typeName = arrayProperty->typeName;

	if (typeName == "TArray<SEntityTemplateReference>")
	{
		TArray<SEntityTemplateReference>* entityTemplateReferences = static_cast<TArray<SEntityTemplateReference>*>(arrayProperty->data);

		entityTemplateReferences->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<float32>")
	{
		TArray<float>* floats = static_cast<TArray<float>*>(arrayProperty->data);

		floats->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<ZGameTime>")
	{
		TArray<ZGameTime>* gameTimes = static_cast<TArray<ZGameTime>*>(arrayProperty->data);

		gameTimes->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<SVector2>")
	{
		TArray<SVector2>* vectors = static_cast<TArray<SVector2>*>(arrayProperty->data);

		vectors->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<bool>")
	{
		TArray<bool>* bools = static_cast<TArray<bool>*>(arrayProperty->data);

		bools->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
	{
		TArray<ZSharedSensorDef::SVisibilitySetting>* visibilitySettings = static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(arrayProperty->data);

		visibilitySettings->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<ZString>")
	{
		TArray<ZString>* strings = static_cast<TArray<ZString>*>(arrayProperty->data);

		strings->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<SSettingsParamMultiplier>")
	{
		TArray<SSettingsParamMultiplier>* visibilitySettings = static_cast<TArray<SSettingsParamMultiplier>*>(arrayProperty->data);

		visibilitySettings->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<SColorRGB>")
	{
		TArray<SColorRGB>* colors = static_cast<TArray<SColorRGB>*>(arrayProperty->data);

		colors->RemoveAt(itemIndex);
	}
	else if (typeName == "TArray<ECameraState>")
	{
		TArray<ECameraState>* cameraStates = static_cast<TArray<ECameraState>*>(arrayProperty->data);

		cameraStates->RemoveAt(itemIndex);
	}
}

void TemplateEntityPropertiesPanel::FilterProperties(const std::string& propertyName)
{
	for (size_t i = 0; i < selectedEntityTreeNode->templateResources.size(); ++i)
	{
		if (selectedEntityTreeNode->templateResources[i]->GetResourceHeaderHeader().m_type == 'TEMP')
		{
			std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(selectedEntityTreeNode->templateResources[i]);
			const int rootEntityIndex = tempResource->GetTemplateEntity()->rootEntityIndex;

			FilterProperties(propertyName, tempResource, rootEntityIndex);
		}
		else
		{
			std::shared_ptr<AspectEntityType> asetResource = std::static_pointer_cast<AspectEntityType>(selectedEntityTreeNode->templateResources[i]);
			const std::vector<std::shared_ptr<Resource>>& asetReferences = asetResource->GetReferences();

			for (size_t j = 0; j < asetReferences.size(); ++j)
			{
				std::shared_ptr<TemplateEntity> tempResource = std::static_pointer_cast<TemplateEntity>(asetReferences[j]);
				const int rootEntityIndex = tempResource->GetTemplateEntity()->rootEntityIndex;

				FilterProperties(propertyName, tempResource, rootEntityIndex);
			}
		}
	}
}

void TemplateEntityPropertiesPanel::FilterProperties(const std::string& propertyName, std::shared_ptr<TemplateEntity> tempResource, const int entityIndex)
{
	TArray<SEntityTemplateProperty>& properties = tempResource->GetTemplateEntity()->entityTemplates[entityIndex].propertyValues;
	TArray<SEntityTemplateProperty>& postInitProperties = tempResource->GetTemplateEntity()->entityTemplates[entityIndex].postInitPropertyValues;

	for (size_t i = 0; i < properties.Size(); ++i)
	{
		if (filteredProperties.contains(properties[i].nPropertyID))
		{
			continue;
		}

		const std::string& propertyName2 = PropertyRegistry::GetInstance().GetPropertyName(properties[i].nPropertyID);
		std::string propertyName3 = propertyName2;

		std::transform(propertyName3.begin(), propertyName3.end(), propertyName3.begin(), tolower);

		if (propertyName3.contains(propertyName))
		{
			filteredProperties.insert(properties[i].nPropertyID);
		}
	}

	for (size_t i = 0; i < postInitProperties.Size(); ++i)
	{
		if (filteredProperties.contains(postInitProperties[i].nPropertyID))
		{
			continue;
		}

		const std::string& propertyName2 = PropertyRegistry::GetInstance().GetPropertyName(postInitProperties[i].nPropertyID);
		std::string propertyName3 = propertyName2;

		std::transform(propertyName3.begin(), propertyName3.end(), propertyName3.begin(), tolower);

		if (propertyName3.contains(propertyName))
		{
			filteredProperties.insert(postInitProperties[i].nPropertyID);
		}
	}
}
