#include "UI/Panels/CompositionBlueprintPanel.h"

CompositionBlueprintPanel::CompositionBlueprintPanel(const char* name, const char* icon, std::shared_ptr<CompositionBlueprint> compositionBlueprintResource) : BasePanel(name, icon)
{
	this->compositionBlueprintResource = compositionBlueprintResource;

	tableColumns.push_back({ "Name" , 0, 1.f });
	tableColumns.push_back({ "Value" , ImGuiTableColumnFlags_WidthStretch, 0.f });
}

void CompositionBlueprintPanel::Render()
{
	if (!Begin())
	{
		return;
	}

	if (!compositionBlueprintResource->IsResourceDeserialized())
	{
		ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionAvail().x / 2, ImGui::GetContentRegionAvail().y / 2));
		ImGui::Text("Loading composition blueprint...");
		End();

		return;
	}

	if (!UI::BeginProperties("CompositionBlueprintTable", tableColumns))
	{
		End();

		return;
	}

	std::shared_ptr<SMusicCompositionData> musicCompositionData = compositionBlueprintResource->GetMusicCompositionData();

	TArrayProperty("m_aGameCues", "TArray<SMusicGameCueData>", &musicCompositionData->m_aGameCues);
	TArrayProperty("m_aPlaylists", "TArray<SMusicPlaylistData>", &musicCompositionData->m_aPlaylists);
	TArrayProperty("m_aStingers", "TArray<SMusicStingerData>", &musicCompositionData->m_aStingers);
	TArrayProperty("m_aSegments", "TArray<SMusicSegmentData>", &musicCompositionData->m_aSegments);
	TSparseShortArrayProperty("m_aEndBehaviors", "TSparseShortArray<SMusicPlaylistEndBehavior>", &musicCompositionData->m_aEndBehaviors);
	TSparseShortArrayProperty("m_transitionMap", "TSparseShortArray<TSparseShortArray<SMusicTransition>>", &musicCompositionData->m_transitionMap);
	UI::Property("m_nJumpCue", musicCompositionData->m_nJumpCue);
	TArrayProperty("m_aTrackExposingGroups", "TArray<SMusicTrackExposingGroupData>", &musicCompositionData->m_aTrackExposingGroups);

	UI::EndProperties();

	End();
}

void CompositionBlueprintPanel::OnResourceLoaded()
{
	compositionBlueprintResource->Deserialize();
}

void CompositionBlueprintPanel::TArrayProperty(const char* propertyName, const char* propertyTypeName, void* data)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		const std::string typeName = propertyTypeName;

		if (typeName == "TArray<SMusicGameCueData>")
		{
			TArray<SMusicGameCueData>* gameCues = static_cast<TArray<SMusicGameCueData>*>(data);

			for (size_t i = 0; i < gameCues->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Game Cue Data {}", i);

				SMusicGameCueDataProperty(treeNodeName.c_str(), (*gameCues)[i]);
			}
		}
		else if (typeName == "TArray<SMusicPlaylistData>")
		{
			TArray<SMusicPlaylistData>* playLists = static_cast<TArray<SMusicPlaylistData>*>(data);

			for (size_t i = 0; i < playLists->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Play List Data {}", i);

				SMusicPlaylistDataProperty(treeNodeName.c_str(), (*playLists)[i]);
			}
		}
		else if (typeName == "TArray<SMusicStingerData>")
		{
			TArray<SMusicStingerData>* stingers = static_cast<TArray<SMusicStingerData>*>(data);

			for (size_t i = 0; i < stingers->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Stinger Data {}", i);

				SMusicStingerDataProperty(treeNodeName.c_str(), (*stingers)[i]);
			}
		}
		else if (typeName == "TArray<SMusicSegmentData>")
		{
			TArray<SMusicSegmentData>* segments = static_cast<TArray<SMusicSegmentData>*>(data);

			for (size_t i = 0; i < segments->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Segment Data {}", i);

				SMusicSegmentDataProperty(treeNodeName.c_str(), (*segments)[i]);
			}
		}
		else if (typeName == "TArray<SMusicTrackExposingGroupData>")
		{
			TArray<SMusicTrackExposingGroupData>* trackExposingGroups = static_cast<TArray<SMusicTrackExposingGroupData>*>(data);

			for (size_t i = 0; i < trackExposingGroups->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Track Exposing Group Data {}", i);

				SMusicTrackExposingGroupDataProperty(treeNodeName.c_str(), (*trackExposingGroups)[i]);
			}
		}
		else if (typeName == "TArray<SMusicPlaylistNodeData>")
		{
			TArray<SMusicPlaylistNodeData>* nodes = static_cast<TArray<SMusicPlaylistNodeData>*>(data);

			for (size_t i = 0; i < nodes->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Play List Node Data {}", i);

				SMusicPlaylistNodeDataProperty(treeNodeName.c_str(), (*nodes)[i]);
			}
		}
		else if (typeName == "TArray<SMusicTrackData>")
		{
			TArray<SMusicTrackData>* tracks = static_cast<TArray<SMusicTrackData>*>(data);

			for (size_t i = 0; i < tracks->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Track Data {}", i);

				SMusicTrackDataProperty(treeNodeName.c_str(), (*tracks)[i]);
			}
		}
		else if (typeName == "TArray<int>")
		{
			TArray<int>* indices = static_cast<TArray<int>*>(data);

			for (size_t i = 0; i < indices->Size(); ++i)
			{
				std::string treeNodeName = std::format("Index {}", i);

				UI::Property(treeNodeName.c_str(), (*indices)[i]);
			}
		}
		else if (typeName == "TArray<ZString>")
		{
			TArray<ZString>* strings = static_cast<TArray<ZString>*>(data);

			for (size_t i = 0; i < strings->Size(); ++i)
			{
				std::string treeNodeName = std::to_string(i);
				std::string string = (*strings)[i].ToCString();

				UI::StringProperty(treeNodeName.c_str(), string);
			}
		}
		else if (typeName == "TArray<EMusicTrackExposingType>")
		{
			TArray<EMusicTrackExposingType>* musicTrackExposingTypes = static_cast<TArray<EMusicTrackExposingType>*>(data);

			for (size_t i = 0; i < musicTrackExposingTypes->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Track Exposing Type {}", i);

				EnumProperty(treeNodeName.c_str(), "EMusicTrackExposingType", (*musicTrackExposingTypes)[i]);
			}
		}
		else if (typeName == "TArray<SMusicTrackEntryData>")
		{
			TArray<SMusicTrackEntryData>* entries = static_cast<TArray<SMusicTrackEntryData>*>(data);

			for (size_t i = 0; i < entries->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Track Entry Data {}", i);

				SMusicTrackEntryDataProperty(treeNodeName.c_str(), (*entries)[i]);
			}
		}
		else if (typeName == "TArray<SMusicTrackVariationData>")
		{
			TArray<SMusicTrackVariationData>* variations = static_cast<TArray<SMusicTrackVariationData>*>(data);

			for (size_t i = 0; i < variations->Size(); ++i)
			{
				std::string treeNodeName = std::format("Music Track Variation Data {}", i);

				SMusicTrackVariationDataProperty(treeNodeName.c_str(), (*variations)[i]);
			}
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::TSparseShortArrayProperty(const char* propertyName, const char* propertyTypeName, void* data)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		const std::string typeName = propertyTypeName;

		if (typeName == "TSparseShortArray<SMusicPlaylistEndBehavior>")
		{
			TSparseShortArray<SMusicPlaylistEndBehavior>* endBehaviors = static_cast<TSparseShortArray<SMusicPlaylistEndBehavior>*>(data);

			for (int i = 0; i < endBehaviors->GetSize(); ++i)
			{
				if (endBehaviors->Contains(i))
				{
					std::string treeNodeName = std::format("Music Play List End Behavior {}", i);

					SMusicPlaylistEndBehaviorProperty(treeNodeName.c_str(), (*endBehaviors)[i]);
				}
			}
		}
		else if (typeName == "TSparseShortArray<TSparseShortArray<SMusicTransition>>")
		{
			TSparseShortArray<TSparseShortArray<SMusicTransition>>* transitionMap = static_cast<TSparseShortArray<TSparseShortArray<SMusicTransition>>*>(data);

			for (int i = 0; i < transitionMap->GetSize(); ++i)
			{
				if (transitionMap->Contains(i))
				{
					std::string treeNodeName = std::format("Transition Array {}", i);

					TSparseShortArrayProperty(treeNodeName.c_str(), "TSparseShortArray<SMusicTransition>", &(*transitionMap)[i]);
				}
			}
		}
		else if (typeName == "TSparseShortArray<SMusicTransition>")
		{
			TSparseShortArray<SMusicTransition>* transitions = static_cast<TSparseShortArray<SMusicTransition>*>(data);

			for (int i = 0; i < transitions->GetSize(); ++i)
			{
				if (transitions->Contains(i))
				{
					std::string treeNodeName = std::format("Music Transition {}", i);

					SMusicTransitionProperty(treeNodeName.c_str(), (*transitions)[i]);
				}
			}
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicGameCueDataProperty(const char* propertyName, SMusicGameCueData& musicGameCueData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		std::string nameProperty = musicGameCueData.m_sName.ToCString();

		UI::StringProperty("m_sName", nameProperty);
		UI::Property("m_bIsStinger", musicGameCueData.m_bIsStinger);
		UI::Property("m_nTargetIndex", musicGameCueData.m_nTargetIndex);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicPlaylistDataProperty(const char* propertyName, SMusicPlaylistData& musicPlaylistData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		std::string nameProperty = musicPlaylistData.m_sName.ToCString();

		UI::StringProperty("m_sName", nameProperty);
		TArrayProperty("m_aNodes", "TArray<SMusicPlaylistNodeData>", &musicPlaylistData.m_aNodes);
		TArrayProperty("m_aBursts", "TArray<SMusicStingerData>", &musicPlaylistData.m_aBursts);
		UI::Property("m_bPlayBursts", musicPlaylistData.m_bPlayBursts);
		UI::Property("m_fMinBurstDelay", musicPlaylistData.m_fMinBurstDelay);
		UI::Property("m_fMaxBurstDelay", musicPlaylistData.m_fMaxBurstDelay);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicPlaylistNodeDataProperty(const char* propertyName, SMusicPlaylistNodeData& musicPlaylistNodeData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		std::string nameProperty = musicPlaylistNodeData.m_sName.ToCString();

		UI::Property("m_nSegmentIndex", musicPlaylistNodeData.m_nSegmentIndex);
		UI::StringProperty("m_sName", nameProperty);
		UI::Property("m_nParentIndex", musicPlaylistNodeData.m_nParentIndex);
		TArrayProperty("m_aChildIndices", "TArray<int>", &musicPlaylistNodeData.m_aChildIndices);
		EnumProperty("m_ePlayType", "EMusicPlaylistPlayType", musicPlaylistNodeData.m_ePlayType);
		UI::Property("m_nWeight", musicPlaylistNodeData.m_nParentIndex);
		UI::Property("m_nLoopCount", musicPlaylistNodeData.m_nParentIndex);
		UI::Property("m_NoRepeatCount", musicPlaylistNodeData.m_nParentIndex);
		UI::Property("m_nSelectProbability", musicPlaylistNodeData.m_nParentIndex);
		UI::Property("m_nMaxLoopCount", musicPlaylistNodeData.m_nParentIndex);
		UI::Property("m_nRealLoopCount", musicPlaylistNodeData.m_nParentIndex);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicStingerDataProperty(const char* propertyName, SMusicStingerData& musicStingerData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		UI::Property("m_nSegmentIndex", musicStingerData.m_nSegmentIndex);
		EnumProperty("m_ePlayStartType", "EMusicPlayStartType", musicStingerData.m_ePlayStartType);
		UI::Property("m_bPreload", musicStingerData.m_bPreload);
		ZCurveProperty("m_duckingCurve", musicStingerData.m_duckingCurve);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicSegmentDataProperty(const char* propertyName, SMusicSegmentData& musicSegmentData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		std::string nameProperty = musicSegmentData.m_sName.ToCString();

		UI::StringProperty("m_sName", nameProperty);
		UI::Property("m_nBarCount", musicSegmentData.m_nBarCount);
		TArrayProperty("m_aTracks", "TArray<SMusicTrackData>", &musicSegmentData.m_aTracks);
		SMusicTimeSignatureProperty("m_signature", musicSegmentData.m_signature);
		UI::Property("m_nChoppedSampleRate", musicSegmentData.m_nChoppedSampleRate);
		UI::Property("m_nChoppedSampleCount", musicSegmentData.m_nChoppedSampleCount);
		UI::Property("m_nLeadInSamples", musicSegmentData.m_nLeadInSamples);
		UI::Property("m_nLeadOutSamples", musicSegmentData.m_nLeadOutSamples);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicTimeSignatureProperty(const char* propertyName, SMusicTimeSignature& musicTimeSignature)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		UI::Property("m_nBeatsPerMinute", musicTimeSignature.m_nBeatsPerMinute);
		UI::Property("m_nBeatsPerBar", musicTimeSignature.m_nBeatsPerBar);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicPlaylistEndBehaviorProperty(const char* propertyName, SMusicPlaylistEndBehavior& musicPlaylistEndBehavior)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		UI::Property("m_bGotoPrevious", musicPlaylistEndBehavior.m_bGotoPrevious);
		UI::Property("m_nAttenuationOffset", musicPlaylistEndBehavior.m_nAttenuationOffset);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicTransitionProperty(const char* propertyName, SMusicTransition& musicTransition)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		UI::Property("m_nSource", musicTransition.m_nSource);
		UI::Property("m_nDestination", musicTransition.m_nDestination);
		EnumProperty("m_eStartType", "EMusicPlayStartType", musicTransition.m_eStartType);
		EnumProperty("m_eTransType", "EMusicTransitionType", musicTransition.m_eTransType);
		EnumProperty("m_eFadeType", "EMusicTransitionFadeType", musicTransition.m_eFadeType);
		UI::Property("m_fFadeTime", musicTransition.m_fFadeTime);
		UI::Property("m_nSegmentIndex", musicTransition.m_nSegmentIndex);
		UI::Property("m_bSkipPreEntries", musicTransition.m_bSkipPreEntries);
		UI::Property("m_fTransDelay", musicTransition.m_fTransDelay);
		ZCurveProperty("m_customFadeInCurve", musicTransition.m_customFadeInCurve);
		ZCurveProperty("m_customFadeOutCurve", musicTransition.m_customFadeOutCurve);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicTrackExposingGroupDataProperty(const char* propertyName, SMusicTrackExposingGroupData& musicTrackExposingGroupData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		std::string nameProperty = musicTrackExposingGroupData.m_sName.ToCString();

		UI::StringProperty("m_sName", nameProperty);
		TArrayProperty("m_trackNames", "TArray<ZString>", &musicTrackExposingGroupData.m_trackNames);
		TArrayProperty("m_trackTypes", "TArray<EMusicTrackExposingType>", &musicTrackExposingGroupData.m_trackTypes);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicTrackDataProperty(const char* propertyName, SMusicTrackData& musicTrackData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		std::string nameProperty = musicTrackData.m_sName.ToCString();
		std::string trackIDProperty = musicTrackData.m_sTrackID.ToCString();

		TArrayProperty("m_aEntries", "TArray<SMusicTrackEntryData>", &musicTrackData.m_aEntries);
		UI::StringProperty("m_sName", nameProperty);
		UI::Property("m_fAttenuation", musicTrackData.m_fAttenuation);
		UI::Property("m_nLowpassCutoff", musicTrackData.m_nLowpassCutoff);
		UI::Property("m_fProbability", musicTrackData.m_fProbability);
		UI::StringProperty("m_sTrackID", trackIDProperty);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicTrackEntryDataProperty(const char* propertyName, SMusicTrackEntryData& musicTrackEntryData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		TArrayProperty("m_aVariations", "TArray<SMusicTrackVariationData>", &musicTrackEntryData.m_aVariations);
		UI::Property("m_nStartBeat", musicTrackEntryData.m_nStartBeat);
		UI::Property("m_bPlayOnce", musicTrackEntryData.m_bPlayOnce);
		UI::Property("m_nSelectedVariation", musicTrackEntryData.m_nSelectedVariation);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::SMusicTrackVariationDataProperty(const char* propertyName, SMusicTrackVariationData& musicTrackVariationData)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		UI::Property("m_nWeight", musicTrackVariationData.m_nWeight);
		UI::Property("m_nWaveIndex", musicTrackVariationData.m_nWaveIndex);
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}

void CompositionBlueprintPanel::ZCurveProperty(const char* propertyName, ZCurve& curve)
{
	const bool isTreeNodeOpen = UI::BeginTreeNodeProperty(propertyName, nullptr);

	if (isTreeNodeOpen)
	{
		TArray<TFixedArray<float, 8>>& data = curve.GetData();

		for (size_t i = 0; i < data.Size(); ++i)
		{
			std::string partIndex = std::to_string(i);
			const bool isChildTreeNodeOpen = UI::BeginTreeNodeProperty(partIndex.c_str(), nullptr);

			if (isChildTreeNodeOpen)
			{
				for (size_t j = 0; j < data[i].Size(); ++j)
				{
					std::string valueIndex = std::to_string(j);

					UI::Property(valueIndex.c_str(), data[i][j]);
				}
			}

			UI::EndTreeNodeProperty(isChildTreeNodeOpen);
		}
	}

	UI::EndTreeNodeProperty(isTreeNodeOpen);
}
