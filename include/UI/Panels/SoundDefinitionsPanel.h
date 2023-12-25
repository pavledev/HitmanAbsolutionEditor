#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/SoundDefinitions.h"
#include "Registry/EnumRegistry.h"

class SoundDefinitionsPanel : public BasePanel
{
public:
	SoundDefinitionsPanel(const char* name, const char* icon, std::shared_ptr<SoundDefinitions> soundDefinitionsResource);
	void Render() override;
	void OnResourceLoaded();

private:
	template <typename T>
	void EnumProperty(const char* propertyName, const char* typeName, T& value)
	{
		const int value2 = static_cast<int>(value);
		const std::map<int, std::string>& enumItems = EnumRegistry::GetInstance().GetEnum(typeName);

		std::string currentValue;

		for (auto it = enumItems.begin(); it != enumItems.end(); ++it)
		{
			if (it->first == value2)
			{
				currentValue = it->second;

				break;
			}
		}

		UI::BeginProperty(propertyName, nullptr);

		if (ImGui::BeginCombo(UI::GetPropertyID(propertyName).c_str(), currentValue.c_str()))
		{
			for (auto it = enumItems.begin(); it != enumItems.end(); ++it)
			{
				bool isSelected = it->first == value2;

				if (ImGui::Selectable(it->second.c_str(), isSelected))
				{
					value = static_cast<T>(it->first);
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

	std::shared_ptr<SoundDefinitions> soundDefinitionsResource;
	std::vector<UI::TableColumn> tableColumns;
};
