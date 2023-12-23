#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/WaveBank.h"
#include "Registry/EnumRegistry.h"

class WaveBankPanel : public BasePanel
{
public:
	WaveBankPanel(const char* name, const char* icon, std::shared_ptr<WaveBank> waveBankResource);
	void Render() override;
	void OnResourceLoaded();

private:
	template <typename T>
	void EnumProperty(const char* propertyName, const char* typeName, T& value)
	{
		const std::map<int, std::string>& enumItems = EnumRegistry::GetInstance().GetEnum(typeName);

		std::string currentValue;

		for (auto it = enumItems.begin(); it != enumItems.end(); ++it)
		{
			if (it->first == value)
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
				bool isSelected = it->first == value;

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

	std::shared_ptr<WaveBank> waveBankResource;
	std::vector<UI::TableColumn> tableColumns;
};
