#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/CompositionBlueprint.h"
#include "Registry/EnumRegistry.h"

class CompositionBlueprintPanel : public BasePanel
{
public:
	CompositionBlueprintPanel(const char* name, const char* icon, std::shared_ptr<CompositionBlueprint> compositionBlueprintResource);
	void Render() override;
	void OnResourceLoaded();

private:
	void TArrayProperty(const char* propertyName, const char* propertyTypeName, void* data);
	void TSparseShortArrayProperty(const char* propertyName, const char* propertyTypeName, void* data);
	void SMusicGameCueDataProperty(const char* propertyName, SMusicGameCueData& musicGameCueData);
	void SMusicPlaylistDataProperty(const char* propertyName, SMusicPlaylistData& musicPlaylistData);
	void SMusicPlaylistNodeDataProperty(const char* propertyName, SMusicPlaylistNodeData& musicPlaylistNodeData);
	void SMusicStingerDataProperty(const char* propertyName, SMusicStingerData& musicStingerData);
	void SMusicSegmentDataProperty(const char* propertyName, SMusicSegmentData& musicSegmentData);
	void SMusicTimeSignatureProperty(const char* propertyName, SMusicTimeSignature& musicTimeSignature);
	void SMusicPlaylistEndBehaviorProperty(const char* propertyName, SMusicPlaylistEndBehavior& musicPlaylistEndBehavior);
	void SMusicTransitionProperty(const char* propertyName, SMusicTransition& musicTransition);
	void SMusicTrackExposingGroupDataProperty(const char* propertyName, SMusicTrackExposingGroupData& musicTrackExposingGroupData);
	void SMusicTrackDataProperty(const char* propertyName, SMusicTrackData& musicTrackData);
	void SMusicTrackEntryDataProperty(const char* propertyName, SMusicTrackEntryData& musicTrackEntryData);
	void SMusicTrackVariationDataProperty(const char* propertyName, SMusicTrackVariationData& musicTrackVariationData);
	void ZCurveProperty(const char* propertyName, ZCurve& curve);

	template <typename T>
	void EnumProperty(const char* propertyName, const char* propertyTypeName, T& value)
	{
		const std::map<int, std::string>& enumItems = EnumRegistry::GetInstance().GetEnum(propertyTypeName);

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

	std::shared_ptr<CompositionBlueprint> compositionBlueprintResource;
	std::vector<UI::TableColumn> tableColumns;
};
