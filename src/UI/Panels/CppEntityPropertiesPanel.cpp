#include <IconsMaterialDesignIcons.h>

#include "Glacier/Math/SMatrix43.h"
#include "Glacier/SColorRGB.h"
#include "Glacier/SColorRGBA.h"
#include "Glacier/SBodyPartDamageMultipliers.h"
#include "Glacier/Entity/ZEntityReference.h"
#include "Glacier/Resource/ZRuntimeResourceID.h"
#include "Glacier/TypeInfo/STypeID.h"
#include "Glacier/Sound/ZSpeakerLevels.h"
#include "Glacier/ZCurve.h"
#include "Glacier/ZGameTime.h"
#include "Glacier/Math/SVector2.h"
#include "Glacier/ECameraState.h"
#include "Glacier/ZSharedSensorDef.h"
#include "Glacier/SSettingsParamMultiplier.h"

#include "UI/Panels/CppEntityPropertiesPanel.h"
#include "Editor.h"
#include "Registry/PropertyRegistry.h"
#include "Registry/EnumRegistry.h"
#include "Registry/TypeRegistry.h"
#include "Resources/AspectEntityType.h"
#include "Resources/AspectEntityBlueprint.h"
#include "Hash.h"

CppEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name) :
	name(name),
	data(nullptr),
	typeName(nullptr),
	description(nullptr)
{
}

CppEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name, void* data) :
	name(name),
	data(data),
	typeName(nullptr),
	description(nullptr)
{
}

CppEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name, void* data, const char* description) :
	name(name),
	data(data),
	typeName(nullptr),
	description(description)
{
}

CppEntityPropertiesPanel::EntityProperty::EntityProperty(const char* name, void* data, const char* typeName, const char* description) :
	name(name),
	data(data),
	typeName(typeName),
	description(description)
{
}

CppEntityPropertiesPanel::CppEntityPropertiesPanel(const char* name, const char* icon, std::shared_ptr<CppEntity> cpptResource) : BasePanel(name, icon)
{
	this->cpptResource = cpptResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void CppEntityPropertiesPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!cpptResource->IsResourceDeserialized())
	{
		End();

		return;
	}

	ImGui::PushFont(Editor::GetInstance().GetImGuiRenderer()->GetMiddleFont());

	RenderEntityProperties();

	ImGui::PopFont();

	End();
}

void CppEntityPropertiesPanel::OnResourceLoaded()
{
	cpptResource->Deserialize();
}

void CppEntityPropertiesPanel::RenderEntityProperties()
{
	if (!UI::BeginProperties("Properties", tableColumns))
	{
		return;
	}

	TArray<SEntityTemplateProperty>& properties = cpptResource->GetCppEntity()->propertyValues;
	std::map<std::string, EntityProperty> entityProperties;

	for (size_t i = 0; i < properties.Size(); ++i)
	{
		const IType* typeInfo = properties[i].value.GetTypeID()->pTypeInfo;
		const std::string& propertyName = PropertyRegistry::GetInstance().GetPropertyName(properties[i].nPropertyID);
		const char* propertyDescription = PropertyRegistry::GetInstance().GetPropertyDescription(properties[i].nPropertyID);
		void* data = properties[i].value.GetData();
		EntityProperty entityProperty;

		entityProperty.name = propertyName.c_str();
		entityProperty.typeName = typeInfo->GetTypeName();
		entityProperty.description = propertyDescription;
		entityProperty.data = data;
		entityProperty.typeInfo = typeInfo;

		entityProperties.insert(std::make_pair(propertyName, entityProperty));
	}

	if (entityProperties.size() > 0)
	{
		RenderEntityProperties(entityProperties);
	}

	UI::EndProperties();
}

void CppEntityPropertiesPanel::RenderEntityProperties(std::map<std::string, EntityProperty>& entityProperties)
{
	for (auto it = entityProperties.begin(); it != entityProperties.end(); ++it)
	{
		EntityProperty& entityProperty = it->second;
		const std::string typeName = entityProperty.typeName;

		if (typeName == "bool")
		{
			BoolProperty(entityProperty);
		}
		else if (typeName == "uint8")
		{
			Property(entityProperty, *static_cast<unsigned char*>(entityProperty.data));
		}
		else if (typeName == "int8")
		{
			Property(entityProperty, *static_cast<char*>(entityProperty.data));
		}
		else if (typeName == "uint16")
		{
			Property(entityProperty, *static_cast<unsigned short*>(entityProperty.data));
		}
		else if (typeName == "int16")
		{
			Property(entityProperty, *static_cast<short*>(entityProperty.data));
		}
		else if (typeName == "uint32")
		{
			Property(entityProperty, *static_cast<unsigned int*>(entityProperty.data));
		}
		else if (typeName == "int32")
		{
			Property(entityProperty, *static_cast<int*>(entityProperty.data));
		}
		else if (typeName == "uint64")
		{
			Property(entityProperty, *static_cast<unsigned long long*>(entityProperty.data));
		}
		else if (typeName == "int64")
		{
			Property(entityProperty, *static_cast<long long*>(entityProperty.data));
		}
		else if (typeName == "float32")
		{
			Property(entityProperty, *static_cast<float*>(entityProperty.data));
		}
		else if (typeName == "float64")
		{
			Property(entityProperty, *static_cast<double*>(entityProperty.data));
		}
		else if (typeName == "SVector2")
		{
			SVector2Property(entityProperty);
		}
		else if (typeName == "SVector3")
		{
			SVector3Property(entityProperty);
		}
		else if (typeName == "SMatrix43")
		{
			SMatrix43Property(entityProperty);
		}
		else if (typeName == "SColorRGB")
		{
			SColorRGBProperty(entityProperty);
		}
		else if (typeName == "SColorRGBA")
		{
			SColorRGBAProperty(entityProperty);
		}
		else if (typeName == "SEntityTemplateReference")
		{
			SEntityTemplateReferenceProperty(entityProperty);
		}
		else if (typeName == "SBodyPartDamageMultipliers")
		{
			SBodyPartDamageMultipliersProperty(entityProperty);
		}
		else if (typeName == "ZRuntimeResourceID")
		{
			ZRuntimeResourceIDProperty(entityProperty);
		}
		else if (typeName == "ZString")
		{
			ZStringProperty(entityProperty);
		}
		else if (typeName == "ZCurve")
		{
			ZCurveProperty(entityProperty);
		}
		else if (typeName == "ZGameTime")
		{
			ZGameTimeProperty(entityProperty);
		}
		else if (typeName == "ZSpeakerLevels")
		{
			ZSpeakerLevelsProperty(entityProperty);
		}
		else if (typeName.starts_with("TArray"))
		{
			TArrayProperty(entityProperty);
		}
		else if (entityProperty.typeInfo->IsEnumType())
		{
			EnumProperty(entityProperty);
		}
	}
}

void CppEntityPropertiesPanel::BoolProperty(EntityProperty& property)
{
	bool* value = static_cast<bool*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description);

	ImGui::Checkbox(UI::GetPropertyID(property.name.c_str()).c_str(), value);

	UI::EndProperty();
}

void CppEntityPropertiesPanel::SMatrix43Property(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		SMatrix43* matrix = static_cast<SMatrix43*>(property.data);
		SVector3 position;
		SVector3 rotation;
		SVector3 scale;

		matrix->Decompose(position, rotation, scale);

		EntityProperty positionProperty = EntityProperty("Position", &matrix->Trans);
		EntityProperty rotationProperty = EntityProperty("Rotation", &rotation);

		SVector3Property(positionProperty);

		SVector3Property(rotationProperty, [&](SVector3& newRotation)
		{
			*matrix = SMatrix43::Recompose(position, *reinterpret_cast<SVector3*>(&newRotation), scale);
		});
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::SVector2Property(EntityProperty& property)
{
	SVector2* vector = static_cast<SVector2*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description, false);

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

	UI::EndProperty();
}

void CppEntityPropertiesPanel::SVector3Property(EntityProperty& property, std::function<void(SVector3&)> onValueChangeCallback)
{
	SVector3* vector = static_cast<SVector3*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description, false);

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

	UI::EndProperty();
}

void CppEntityPropertiesPanel::SColorRGBProperty(EntityProperty& property)
{
	SColorRGB* colorRGB = static_cast<SColorRGB*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description);

	ImGui::ColorEdit3(UI::GetPropertyID(property.name.c_str()).c_str(), &colorRGB->r);

	UI::EndProperty();
}

void CppEntityPropertiesPanel::SColorRGBAProperty(EntityProperty& property)
{
	SColorRGBA* colorRGBA = static_cast<SColorRGBA*>(property.data);

	UI::BeginProperty(property.name.c_str(), property.description);

	ImGui::ColorEdit4(UI::GetPropertyID(property.name.c_str()).c_str(), &colorRGBA->r);

	UI::EndProperty();
}

void CppEntityPropertiesPanel::SEntityTemplateReferenceProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		SEntityTemplateReference* entityTemplateReference = static_cast<SEntityTemplateReference*>(property.data);
		EntityProperty entityIndexProperty = EntityProperty("entityIndex");
		EntityProperty exposedEntityProperty = EntityProperty("exposedEntity", &entityTemplateReference->exposedEntity);

		Property(entityIndexProperty, entityTemplateReference->entityIndex);
		ZStringProperty(exposedEntityProperty);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::SBodyPartDamageMultipliersProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		SBodyPartDamageMultipliers* bodyPartDamageMultipliers = static_cast<SBodyPartDamageMultipliers*>(property.data);
		EntityProperty headDamageMultiplierProperty = EntityProperty("m_fHeadDamageMultiplier");
		EntityProperty faceDamageMultiplierentityIndexProperty = EntityProperty("m_fFaceDamageMultiplier");
		EntityProperty armDamageMultiplierentityIndexProperty = EntityProperty("m_fArmDamageMultiplier");
		EntityProperty handDamageMultiplierentityIndexProperty = EntityProperty("m_fHandDamageMultiplier");
		EntityProperty legDamageMultiplierentityIndexProperty = EntityProperty("m_fLegDamageMultiplier");
		EntityProperty torsoDamageMultiplierentityIndexProperty = EntityProperty("m_fTorsoDamageMultiplier");

		Property(headDamageMultiplierProperty, bodyPartDamageMultipliers->m_fHeadDamageMultiplier);
		Property(faceDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fFaceDamageMultiplier);
		Property(armDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fArmDamageMultiplier);
		Property(handDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fHandDamageMultiplier);
		Property(legDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fLegDamageMultiplier);
		Property(torsoDamageMultiplierentityIndexProperty, bodyPartDamageMultipliers->m_fTorsoDamageMultiplier);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::SVisibilitySettingProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

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

		EnumProperty(sensitivityProperty);
		Property(closeRangeProperty, visibilitySetting->m_fCloseRange);
		Property(peripheralRangeProperty, visibilitySetting->m_fPeripheralRange);
		Property(peripheralAngleProperty, visibilitySetting->m_fPeripheralAngle);
		Property(peripheralHighProperty, visibilitySetting->m_fPeripheralHigh);
		Property(peripheralLowProperty, visibilitySetting->m_fPeripheralLow);
		Property(focusConeWidthAngleProperty, visibilitySetting->m_fFocusConeWidthAngle);
		Property(focusConeHeightAngleProperty, visibilitySetting->m_fFocusConeHeightAngle);
		Property(focusConeRangeProperty, visibilitySetting->m_fFocusConeRange);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::SSettingsParamMultiplierProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		static PropertyRegistry& propertyRegistry = PropertyRegistry::GetInstance();
		static const char* settingsParamDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_eSettingsParam"));
		static const char* multiplierDescription = propertyRegistry.GetPropertyDescription(Hash::Crc32("m_fMultiplier"));

		SSettingsParamMultiplier* settingsParamMultiplier = static_cast<SSettingsParamMultiplier*>(property.data);
		EntityProperty settingsParamProperty = EntityProperty("m_eSettingsParam", &settingsParamMultiplier->m_eSettingsParam, "ESettingsParam", settingsParamDescription);
		EntityProperty multiplierProperty = EntityProperty("m_fMultiplier");

		EnumProperty(settingsParamProperty);
		Property(multiplierProperty, settingsParamMultiplier->m_fMultiplier);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::ZRuntimeResourceIDProperty(EntityProperty& property)
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

	UI::Property(property.name.c_str(), stringBuffer, sizeof(stringBuffer), property.description);
}

void CppEntityPropertiesPanel::ZStringProperty(EntityProperty& property)
{
	ZString* string = static_cast<ZString*>(property.data);
	static char stringBuffer[2048] = {};

	memcpy(stringBuffer, string->ToCString(), string->Length() + 1);

	UI::Property(property.name.c_str(), stringBuffer, sizeof(stringBuffer), property.description);
}

void CppEntityPropertiesPanel::ZCurveProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		ZCurve* curve = static_cast<ZCurve*>(property.data);
		TArray<TFixedArray<float, 8>>& data = curve->GetData();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			std::string partIndex = std::to_string(i);
			const bool isChildTreeNodeOpen = UI::BeginTreeNodeProperty(partIndex.c_str(), nullptr);

			if (isChildTreeNodeOpen)
			{
				for (size_t j = 0; j < data[i].Size(); ++j)
				{
					std::string valueIndex = std::to_string(j);
					EntityProperty property2 = EntityProperty(valueIndex.c_str());

					Property(property2, data[i][j]);
				}
			}

			UI::EndTreeNodeProperty(isChildTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::ZGameTimeProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		ZGameTime* gameTime = static_cast<ZGameTime*>(property.data);
		EntityProperty ticksProperty = EntityProperty("m_nTicks");

		Property(ticksProperty, gameTime->GetTicks());
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::ZSpeakerLevelsProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		ZSpeakerLevels* speakerLevels = static_cast<ZSpeakerLevels*>(property.data);
		TArray<float>& speakerMatrix = speakerLevels->GetSpeakerMatrix();

		for (size_t i = 0; i < speakerMatrix.Size(); ++i)
		{
			std::string index = std::to_string(i);
			EntityProperty property2 = EntityProperty(index.c_str());

			Property(property2, speakerMatrix[i]);
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::TArrayProperty(EntityProperty& property)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(property.name.c_str(), property.description);

	if (isTreeNodeOpen && property.data)
	{
		const std::string typeName = property.typeName;

		if (typeName == "TArray<SEntityTemplateReference>")
		{
			TArray<SEntityTemplateReference>* entityTemplateReferences = static_cast<TArray<SEntityTemplateReference>*>(property.data);

			for (size_t i = 0; i < entityTemplateReferences->Size(); ++i)
			{
				std::string treeNodeName = std::format("Entity Template Reference {}", i);
				EntityProperty entityTemplateReferenceProperty = EntityProperty(treeNodeName.c_str(), &(*entityTemplateReferences)[i]);

				SEntityTemplateReferenceProperty(entityTemplateReferenceProperty);
			}
		}
		else if (typeName == "TArray<float32>")
		{
			TArray<float>* floats = static_cast<TArray<float>*>(property.data);

			for (size_t i = 0; i < floats->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty floatProperty = EntityProperty(index.c_str());

				Property(floatProperty, (*floats)[i]);
			}
		}
		else if (typeName == "TArray<ZGameTime>")
		{
			TArray<ZGameTime>* gameTimes = static_cast<TArray<ZGameTime>*>(property.data);

			for (size_t i = 0; i < gameTimes->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty gameTimeProperty = EntityProperty(index.c_str(), &(*gameTimes)[i]);

				ZGameTimeProperty(gameTimeProperty);
			}
		}
		else if (typeName == "TArray<SVector2>")
		{
			TArray<SVector2>* vectors = static_cast<TArray<SVector2>*>(property.data);

			for (size_t i = 0; i < vectors->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty vectorProperty = EntityProperty(index.c_str(), &(*vectors)[i]);

				SVector2Property(vectorProperty);
			}
		}
		else if (typeName == "TArray<bool>")
		{
			TArray<bool>* bools = static_cast<TArray<bool>*>(property.data);

			for (size_t i = 0; i < bools->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty boolProperty = EntityProperty(index.c_str(), &(*bools)[i]);

				BoolProperty(boolProperty);
			}
		}
		else if (typeName == "TArray<ZSharedSensorDef.SVisibilitySetting>")
		{
			TArray<ZSharedSensorDef::SVisibilitySetting>* visibilitySettings = static_cast<TArray<ZSharedSensorDef::SVisibilitySetting>*>(property.data);

			for (size_t i = 0; i < visibilitySettings->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty visibilitySettingProperty = EntityProperty(index.c_str(), &(*visibilitySettings)[i]);

				SVisibilitySettingProperty(visibilitySettingProperty);
			}
		}
		else if (typeName == "TArray<ZString>")
		{
			TArray<ZString>* strings = static_cast<TArray<ZString>*>(property.data);

			for (size_t i = 0; i < strings->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty stringProperty = EntityProperty(index.c_str(), &(*strings)[i]);

				ZStringProperty(stringProperty);
			}
		}
		else if (typeName == "TArray<SSettingsParamMultiplier>")
		{
			TArray<SSettingsParamMultiplier>* visibilitySettings = static_cast<TArray<SSettingsParamMultiplier>*>(property.data);

			for (size_t i = 0; i < visibilitySettings->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty settingsParamMultiplierProperty = EntityProperty(index.c_str(), &(*visibilitySettings)[i]);

				SSettingsParamMultiplierProperty(settingsParamMultiplierProperty);
			}
		}
		else if (typeName == "TArray<SColorRGB>")
		{
			TArray<SColorRGB>* colors = static_cast<TArray<SColorRGB>*>(property.data);

			for (size_t i = 0; i < colors->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty colorRGBProperty = EntityProperty(index.c_str(), &(*colors)[i]);

				SColorRGBProperty(colorRGBProperty);
			}
		}
		else if (typeName == "TArray<ECameraState>")
		{
			TArray<ECameraState>* cameraStates = static_cast<TArray<ECameraState>*>(property.data);

			for (size_t i = 0; i < cameraStates->Size(); ++i)
			{
				std::string index = std::to_string(i);
				EntityProperty cameraStateProperty = EntityProperty(index.c_str(), &(*cameraStates)[i], property.typeName, nullptr);

				EnumProperty(cameraStateProperty);
			}
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CppEntityPropertiesPanel::EnumProperty(EntityProperty& property)
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

	UI::EndProperty();
}
