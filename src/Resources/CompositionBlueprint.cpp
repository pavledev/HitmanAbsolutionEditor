#include "Glacier/Serializer/ZBinaryDeserializer.h"

#include "Resources/CompositionBlueprint.h"
#include "Utility/ResourceUtility.h"

void CompositionBlueprint::Deserialize()
{
	ZBinaryDeserializer binaryDeserializer;
	void* musicCompositionData = binaryDeserializer.Deserialize(GetResourceData(), GetResourceDataSize());

	Parse(musicCompositionData);

	operator delete(musicCompositionData, std::align_val_t(binaryDeserializer.GetAlignment()));

	isResourceDeserialized = true;
}

void CompositionBlueprint::Parse(void* musicCompositionData)
{
	this->musicCompositionData = std::make_shared<SMusicCompositionData>();

	ParseGameCues(musicCompositionData);
	ParsePlayLists(musicCompositionData);
	ParseStingers(musicCompositionData);
	ParseSegments(musicCompositionData);
	ParseEndBehaviors(musicCompositionData);
	ParseTransitionMap(musicCompositionData);
	ParseTrackExposingGroups(musicCompositionData);

	this->musicCompositionData->m_nJumpCue = *(reinterpret_cast<int*>(musicCompositionData) + 18);
}

void CompositionBlueprint::ParseGameCues(void* musicCompositionData)
{
	const unsigned int gameCuesStartAddress = *reinterpret_cast<unsigned int*>(musicCompositionData);
	const unsigned int gameCuesEndAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 1);
	const unsigned int gameCueCount = ResourceUtility::CalculateArrayElementsCount(gameCuesStartAddress, gameCuesEndAddress, 0x10); //0x10 is size of SMusicGameCueData

	this->musicCompositionData->m_aGameCues.Resize(gameCueCount);

	for (unsigned int i = 0; i < gameCueCount; i++)
	{
		const unsigned int gameCueAddress = gameCuesStartAddress + 0x10 * i;
		SMusicGameCueData* musicGameCueData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicGameCueData>(musicCompositionData, gameCueAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(musicGameCueData) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress);

		const bool isStinger = *reinterpret_cast<bool*>(reinterpret_cast<unsigned int*>(musicGameCueData) + 2);
		const int targetIndex = *(reinterpret_cast<int*>(musicGameCueData) + 3);

		this->musicCompositionData->m_aGameCues[i].m_sName = ZString(chars);
		this->musicCompositionData->m_aGameCues[i].m_bIsStinger = isStinger;
		this->musicCompositionData->m_aGameCues[i].m_nTargetIndex = targetIndex;
	}
}

void CompositionBlueprint::ParsePlayLists(void* musicCompositionData)
{
	const unsigned int playlistsStartAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 3);
	const unsigned int playlistsEndAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 4);
	const unsigned int playlistCount = ResourceUtility::CalculateArrayElementsCount(playlistsStartAddress, playlistsEndAddress, 0x2C); //0x2C is size of SMusicPlaylistData

	this->musicCompositionData->m_aPlaylists.Resize(playlistCount);

	for (unsigned int i = 0; i < playlistCount; i++)
	{
		const unsigned int playlistAddress = playlistsStartAddress + 0x2C * i;
		SMusicPlaylistData* musicPlaylistData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicPlaylistData>(musicCompositionData, playlistAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistData) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress);

		this->musicCompositionData->m_aPlaylists[i].m_sName = ZString(chars);

		const unsigned int nodesStartAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistData) + 2);
		const unsigned int nodesEndAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistData) + 3);
		const unsigned int nodeCount = ResourceUtility::CalculateArrayElementsCount(nodesStartAddress, nodesEndAddress, 0x38); //0x38 is size of SMusicPlaylistNodeData

		this->musicCompositionData->m_aPlaylists[i].m_aNodes.Resize(nodeCount);

		for (unsigned int j = 0; j < nodeCount; j++)
		{
			const unsigned int nodeAddress = nodesStartAddress + 0x38 * j;
			SMusicPlaylistNodeData* musicPlaylistNodeData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicPlaylistNodeData>(musicCompositionData, nodeAddress);

			const int segmentIndex = *reinterpret_cast<int*>(musicPlaylistNodeData);

			const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(musicPlaylistNodeData) + 2);
			const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress2);

			const int parentIndex = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 3);

			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nSegmentIndex = segmentIndex;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_sName = ZString(chars2);
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nParentIndex = parentIndex;

			const unsigned int childIndicesStartAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistNodeData) + 4);
			const unsigned int childIndicesEndAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistNodeData) + 5);
			const unsigned int childIndexCount = ResourceUtility::CalculateArrayElementsCount(childIndicesStartAddress, childIndicesEndAddress, sizeof(int));

			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_aChildIndices.Resize(childIndexCount);

			for (unsigned int k = 0; k < childIndexCount; k++)
			{
				const unsigned int childIndexAddress = childIndicesStartAddress + sizeof(int) * k;
				int* childIndex = ResourceUtility::Convert4ByteAddressTo8BytePointer<int>(musicCompositionData, childIndexAddress);

				this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_aChildIndices[k] = *childIndex;
			}

			const EMusicPlaylistPlayType playType = *(reinterpret_cast<EMusicPlaylistPlayType*>(musicPlaylistNodeData) + 7);
			const int weight = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 8);
			const int loopCount = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 9);
			const int noRepeatCount = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 10);
			const int selectProbability = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 11);
			const int maxLoopCount = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 12);
			const int realLoopCount = *(reinterpret_cast<int*>(musicPlaylistNodeData) + 13);

			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_ePlayType = playType;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nWeight = weight;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nLoopCount = loopCount;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_NoRepeatCount = noRepeatCount;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nSelectProbability = selectProbability;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nMaxLoopCount = maxLoopCount;
			this->musicCompositionData->m_aPlaylists[i].m_aNodes[j].m_nRealLoopCount = realLoopCount;
		}

		const unsigned int burstsStartAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistData) + 5);
		const unsigned int burstsEndAddress = *(reinterpret_cast<unsigned int*>(musicPlaylistData) + 6);
		const unsigned int burstCount = ResourceUtility::CalculateArrayElementsCount(burstsStartAddress, burstsEndAddress, 0x18); //0x18 is size of SMusicStingerData

		this->musicCompositionData->m_aPlaylists[i].m_aBursts.Resize(burstCount);

		for (unsigned int j = 0; j < burstCount; j++)
		{
			const unsigned int musicStingerDataAddress = burstsStartAddress + 0x18 * j;
			SMusicStingerData* musicStingerData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicStingerData>(musicCompositionData, musicStingerDataAddress);

			const int segmentIndex = *reinterpret_cast<int*>(musicStingerData);
			const EMusicPlayStartType playStartType = *(reinterpret_cast<EMusicPlayStartType*>(musicStingerData) + 1);
			const bool preload = *reinterpret_cast<bool*>(reinterpret_cast<unsigned int*>(musicStingerData) + 2);
			ZCurve* duckingCurve = reinterpret_cast<ZCurve*>(reinterpret_cast<unsigned int*>(musicStingerData) + 3);

			this->musicCompositionData->m_aPlaylists[i].m_aBursts[j].m_nSegmentIndex = segmentIndex;
			this->musicCompositionData->m_aPlaylists[i].m_aBursts[j].m_ePlayStartType = playStartType;
			this->musicCompositionData->m_aPlaylists[i].m_aBursts[j].m_bPreload = preload;

			const unsigned int dataStartAddress = *reinterpret_cast<unsigned int*>(duckingCurve);
			const unsigned int dataEndAddress = *(reinterpret_cast<unsigned int*>(duckingCurve) + 1);
			const unsigned int dataCount = ResourceUtility::CalculateArrayElementsCount(dataStartAddress, dataEndAddress, 0x8 * sizeof(float));
			TArray<TFixedArray<float, 8>>& data = this->musicCompositionData->m_aPlaylists[i].m_aBursts[j].m_duckingCurve.GetData();

			data.Resize(dataCount);

			for (unsigned int k = 0; k < dataCount; k++)
			{
				const unsigned int fixedArrayAddress = dataStartAddress + 0x8 * sizeof(float) * k;
				TFixedArray<float, 8>* fixedArray = ResourceUtility::Convert4ByteAddressTo8BytePointer<TFixedArray<float, 8>>(musicCompositionData, fixedArrayAddress);

				for (unsigned int l = 0; l < fixedArray->Size(); ++l)
				{
					data[k].m_pStart[l] = (*fixedArray)[l];
				}
			}
		}

		const bool playBursts = *reinterpret_cast<bool*>(reinterpret_cast<unsigned int*>(musicPlaylistData) + 8);
		const float minBurstDelay = *reinterpret_cast<float*>(reinterpret_cast<unsigned int*>(musicPlaylistData) + 9);
		const float maxBurstDelay = *reinterpret_cast<float*>(reinterpret_cast<unsigned int*>(musicPlaylistData) + 10);

		this->musicCompositionData->m_aPlaylists[i].m_bPlayBursts = playBursts;
		this->musicCompositionData->m_aPlaylists[i].m_fMinBurstDelay = minBurstDelay;
		this->musicCompositionData->m_aPlaylists[i].m_fMaxBurstDelay = maxBurstDelay;
	}
}

void CompositionBlueprint::ParseStingers(void* musicCompositionData)
{
	const unsigned int stingersStartAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 6);
	const unsigned int stingersEndAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 7);
	const unsigned int stingerCount = ResourceUtility::CalculateArrayElementsCount(stingersStartAddress, stingersEndAddress, 0x18); //0x18 is size of SMusicStingerData

	this->musicCompositionData->m_aStingers.Resize(stingerCount);

	for (unsigned int i = 0; i < stingerCount; i++)
	{
		const unsigned int musicStingerDataAddress = stingersStartAddress + 0x18 * i;
		SMusicStingerData* musicStingerData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicStingerData>(musicCompositionData, musicStingerDataAddress);

		const int segmentIndex = *reinterpret_cast<int*>(musicStingerData);
		const EMusicPlayStartType playStartType = *(reinterpret_cast<EMusicPlayStartType*>(musicStingerData) + 1);
		const bool preload = *(reinterpret_cast<bool*>(musicStingerData) + 2);
		ZCurve* duckingCurve = reinterpret_cast<ZCurve*>(reinterpret_cast<unsigned int*>(musicStingerData) + 3);

		this->musicCompositionData->m_aStingers[i].m_nSegmentIndex = segmentIndex;
		this->musicCompositionData->m_aStingers[i].m_ePlayStartType = playStartType;
		this->musicCompositionData->m_aStingers[i].m_bPreload = preload;

		const unsigned int dataStartAddress = *reinterpret_cast<unsigned int*>(duckingCurve);
		const unsigned int dataEndAddress = *(reinterpret_cast<unsigned int*>(duckingCurve) + 1);
		const unsigned int dataCount = ResourceUtility::CalculateArrayElementsCount(dataStartAddress, dataEndAddress, 0x8 * sizeof(float));
		TArray<TFixedArray<float, 8>>& data = this->musicCompositionData->m_aStingers[i].m_duckingCurve.GetData();

		data.Resize(dataCount);

		for (unsigned int k = 0; k < dataCount; k++)
		{
			const unsigned int fixedArrayAddress = dataStartAddress + 0x8 * sizeof(float) * k;
			TFixedArray<float, 8>* fixedArray = ResourceUtility::Convert4ByteAddressTo8BytePointer<TFixedArray<float, 8>>(musicCompositionData, fixedArrayAddress);

			for (unsigned int l = 0; l < fixedArray->Size(); ++l)
			{
				data[k].m_pStart[l] = (*fixedArray)[l];
			}
		}
	}
}

void CompositionBlueprint::ParseSegments(void* musicCompositionData)
{
	const unsigned int segmentsStartAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 9);
	const unsigned int segmentsEndAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 10);
	const unsigned int segmentCount = ResourceUtility::CalculateArrayElementsCount(segmentsStartAddress, segmentsEndAddress, 0x30); //0x30 is size of SMusicStingerData

	this->musicCompositionData->m_aSegments.Resize(segmentCount);

	for (unsigned int i = 0; i < segmentCount; i++)
	{
		const unsigned int musicSegmentDataDataAddress = segmentsStartAddress + 0x30 * i;
		SMusicSegmentData* musicSegmentData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicSegmentData>(musicCompositionData, musicSegmentDataDataAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(musicSegmentData) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress);

		const int barCount = *(reinterpret_cast<int*>(musicSegmentData) + 2);

		this->musicCompositionData->m_aSegments[i].m_sName = ZString(chars);
		this->musicCompositionData->m_aSegments[i].m_nBarCount = barCount;

		const unsigned int tracksStartAddress = *(reinterpret_cast<unsigned int*>(musicSegmentData) + 3);
		const unsigned int tracksEndAddress = *(reinterpret_cast<unsigned int*>(musicSegmentData) + 4);
		const unsigned int trackCount = ResourceUtility::CalculateArrayElementsCount(tracksStartAddress, tracksEndAddress, 0x28); //0x28 is size of SMusicTrackData

		this->musicCompositionData->m_aSegments[i].m_aTracks.Resize(trackCount);

		for (unsigned int j = 0; j < trackCount; j++)
		{
			const unsigned int musicTrackDataAddress = tracksStartAddress + 0x28 * j;
			SMusicTrackData* musicTrackData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicTrackData>(musicCompositionData, musicTrackDataAddress);

			const unsigned int entriesStartAddress = *reinterpret_cast<unsigned int*>(musicTrackData);
			const unsigned int entriesEndAddress = *(reinterpret_cast<unsigned int*>(musicTrackData) + 1);
			const unsigned int entryCount = ResourceUtility::CalculateArrayElementsCount(entriesStartAddress, entriesEndAddress, 0x18); //0x18 is size of SMusicTrackEntryData

			this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries.Resize(entryCount);

			for (unsigned int k = 0; k < entryCount; k++)
			{
				const unsigned int musicTrackEntryDataAddress = entriesStartAddress + 0x18 * k;
				SMusicTrackEntryData* musicTrackEntryData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicTrackEntryData>(musicCompositionData, musicTrackEntryDataAddress);

				const unsigned int variationsStartAddress = *reinterpret_cast<unsigned int*>(musicTrackEntryData);
				const unsigned int variationsEndAddress = *(reinterpret_cast<unsigned int*>(musicTrackEntryData) + 1);
				const unsigned int variationCount = ResourceUtility::CalculateArrayElementsCount(variationsStartAddress, variationsEndAddress, 0x8); //0x8 is size of SMusicTrackVariationData

				this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries[k].m_aVariations.Resize(variationCount);

				for (unsigned int l = 0; l < variationCount; l++)
				{
					const unsigned int musicTrackVariationDataAddress = variationsStartAddress + 0x8 * l;
					SMusicTrackVariationData* musicTrackVariationData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicTrackVariationData>(musicCompositionData, musicTrackVariationDataAddress);

					const int weight = *reinterpret_cast<int*>(musicTrackVariationData);
					const int waveIndex = *(reinterpret_cast<int*>(musicTrackVariationData) + 1);

					this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries[k].m_aVariations[l].m_nWeight = weight;
					this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries[k].m_aVariations[l].m_nWaveIndex = waveIndex;
				}

				const int startBeat = *(reinterpret_cast<int*>(musicTrackEntryData) + 3);
				const bool playOnce = *reinterpret_cast<bool*>(reinterpret_cast<unsigned int*>(musicTrackEntryData) + 4);
				const int selectedVariation = *(reinterpret_cast<int*>(musicTrackEntryData) + 5);

				this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries[k].m_nStartBeat = startBeat;
				this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries[k].m_bPlayOnce = playOnce;
				this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_aEntries[k].m_nSelectedVariation = playOnce;
			}

			const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(musicTrackData) + 4);
			const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress2);

			const float attenuation = *reinterpret_cast<float*>(reinterpret_cast<unsigned int*>(musicTrackData) + 5);
			const int lowpassCutoff = *(reinterpret_cast<int*>(musicTrackData) + 6);
			const float probability = *reinterpret_cast<float*>(reinterpret_cast<unsigned int*>(musicTrackData) + 7);

			const unsigned int charsAddress3 = *(reinterpret_cast<unsigned int*>(musicTrackData) + 9);
			const char* chars3 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress3);

			this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_sName = ZString(chars2);
			this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_fAttenuation = attenuation;
			this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_nLowpassCutoff = lowpassCutoff;
			this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_fProbability = probability;
			this->musicCompositionData->m_aSegments[i].m_aTracks[j].m_sTrackID = ZString(chars3);
		}

		const SMusicTimeSignature signature = *reinterpret_cast<SMusicTimeSignature*>(reinterpret_cast<unsigned int*>(musicSegmentData) + 6);
		const int choppedSampleRate = *(reinterpret_cast<int*>(musicSegmentData) + 8);
		const int choppedSampleCount = *(reinterpret_cast<int*>(musicSegmentData) + 9);
		const int leadInSamples = *(reinterpret_cast<int*>(musicSegmentData) + 10);
		const int leadOutSamples = *(reinterpret_cast<int*>(musicSegmentData) + 11);

		this->musicCompositionData->m_aSegments[i].m_signature = signature;
		this->musicCompositionData->m_aSegments[i].m_nChoppedSampleRate = choppedSampleRate;
		this->musicCompositionData->m_aSegments[i].m_nChoppedSampleCount = choppedSampleCount;
		this->musicCompositionData->m_aSegments[i].m_nLeadInSamples = leadInSamples;
		this->musicCompositionData->m_aSegments[i].m_nLeadOutSamples = leadOutSamples;
	}
}

void CompositionBlueprint::ParseEndBehaviors(void* musicCompositionData)
{
	int size = *(reinterpret_cast<int*>(musicCompositionData) + 12);
	unsigned int indicesAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 13);
	unsigned int itemsAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 14);
	char* indices = ResourceUtility::Convert4ByteAddressTo8BytePointer<char>(musicCompositionData, indicesAddress);
	SMusicPlaylistEndBehavior* items = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicPlaylistEndBehavior>(musicCompositionData, itemsAddress);

	this->musicCompositionData->m_aEndBehaviors = TSparseShortArray<SMusicPlaylistEndBehavior>(size);

	memcpy(*this->musicCompositionData->m_aEndBehaviors.GetIndices(), indices, size);

	for (int i = 0; i < size; ++i)
	{
		if (!this->musicCompositionData->m_aEndBehaviors.Contains(i))
		{
			continue;
		}

		const unsigned int itemIndex = (*this->musicCompositionData->m_aEndBehaviors.GetIndices())[i];
		SMusicPlaylistEndBehavior* musicPlaylistEndBehavior = reinterpret_cast<SMusicPlaylistEndBehavior*>(reinterpret_cast<unsigned int*>(items) + 2 * itemIndex);

		this->musicCompositionData->m_aEndBehaviors[i].m_bGotoPrevious = *reinterpret_cast<bool*>(reinterpret_cast<unsigned int*>(musicPlaylistEndBehavior));
		this->musicCompositionData->m_aEndBehaviors[i].m_nAttenuationOffset = *(reinterpret_cast<int*>(musicPlaylistEndBehavior) + 1);
	}
}

void CompositionBlueprint::ParseTransitionMap(void* musicCompositionData)
{
	int size = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 15);
	unsigned int indicesAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 16);
	unsigned int itemsAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 17);
	char* indices = ResourceUtility::Convert4ByteAddressTo8BytePointer<char>(musicCompositionData, indicesAddress);
	TSparseShortArray<SMusicTransition>* items = ResourceUtility::Convert4ByteAddressTo8BytePointer<TSparseShortArray<SMusicTransition>>(musicCompositionData, itemsAddress);

	this->musicCompositionData->m_transitionMap = TSparseShortArray<TSparseShortArray<SMusicTransition>>(size);

	memcpy(*this->musicCompositionData->m_transitionMap.GetIndices(), indices, size);

	TSparseShortArray<SMusicTransition>* sparseShortArrays = this->musicCompositionData->m_transitionMap.GetItems();

	for (int i = 0; i < size; ++i)
	{
		if (!this->musicCompositionData->m_transitionMap.Contains(i))
		{
			continue;
		}

		const unsigned int itemIndex = (*this->musicCompositionData->m_transitionMap.GetIndices())[i];
		int size2 = *(reinterpret_cast<int*>(items) + 3 * itemIndex);
		unsigned int indicesAddress2 = *(reinterpret_cast<unsigned int*>(items) + 3 * itemIndex + 1);
		unsigned int itemsAddress2 = *(reinterpret_cast<unsigned int*>(items) + 3 * itemIndex + 2);
		char* indices2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<char>(musicCompositionData, indicesAddress2);
		SMusicTransition* items2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicTransition>(musicCompositionData, itemsAddress2);

		new (sparseShortArrays + itemIndex) TSparseShortArray<SMusicTransition>(size2);

		TSparseShortArray<SMusicTransition>& sparseShortArray = *(sparseShortArrays + itemIndex);

		memcpy(*sparseShortArray.GetIndices(), indices2, size2);

		for (int j = 0; j < size2; ++j)
		{
			if (!sparseShortArray.Contains(j))
			{
				continue;
			}

			const unsigned int itemIndex2 = (*sparseShortArray.GetIndices())[j];
			SMusicTransition* musicTransition = reinterpret_cast<SMusicTransition*>(reinterpret_cast<unsigned int*>(items2) + 0xF * itemIndex2);
			const int source = *reinterpret_cast<int*>(musicTransition);
			const int destination = *(reinterpret_cast<int*>(musicTransition) + 1);
			const EMusicPlayStartType startType = *(reinterpret_cast<EMusicPlayStartType*>(musicTransition) + 2);
			const EMusicTransitionType transType = *(reinterpret_cast<EMusicTransitionType*>(musicTransition) + 3);
			const EMusicTransitionFadeType fadeType = *(reinterpret_cast<EMusicTransitionFadeType*>(musicTransition) + 4);
			const float fadeTime = *reinterpret_cast<float*>(reinterpret_cast<int*>(musicTransition) + 5);
			const int segmentIndex = *(reinterpret_cast<int*>(musicTransition) + 6);
			const bool skipPreEntries = *reinterpret_cast<bool*>(reinterpret_cast<unsigned int*>(musicTransition) + 7);
			const float transDelay = *reinterpret_cast<float*>(reinterpret_cast<unsigned int*>(musicTransition) + 8);
			ZCurve* customFadeInCurve = reinterpret_cast<ZCurve*>(reinterpret_cast<unsigned int*>(musicTransition) + 9);
			ZCurve* customFadeOutCurve = reinterpret_cast<ZCurve*>(reinterpret_cast<unsigned int*>(musicTransition) + 12);

			sparseShortArray[j].m_nSource = source;
			sparseShortArray[j].m_nDestination = destination;
			sparseShortArray[j].m_eStartType = startType;
			sparseShortArray[j].m_eTransType = transType;
			sparseShortArray[j].m_eFadeType = fadeType;
			sparseShortArray[j].m_fFadeTime = fadeTime;
			sparseShortArray[j].m_nSegmentIndex = segmentIndex;
			sparseShortArray[j].m_bSkipPreEntries = skipPreEntries;
			sparseShortArray[j].m_fTransDelay = transDelay;

			const unsigned int dataStartAddress = *reinterpret_cast<unsigned int*>(customFadeInCurve);
			const unsigned int dataEndAddress = *(reinterpret_cast<unsigned int*>(customFadeInCurve) + 1);
			const unsigned int dataCount = ResourceUtility::CalculateArrayElementsCount(dataStartAddress, dataEndAddress, 0x8 * sizeof(float));
			TArray<TFixedArray<float, 8>>& data = sparseShortArray[j].m_customFadeInCurve.GetData();

			data.Resize(dataCount);

			for (unsigned int k = 0; k < dataCount; k++)
			{
				const unsigned int fixedArrayAddress = dataStartAddress + 0x8 * sizeof(float) * k;
				TFixedArray<float, 8>* fixedArray = ResourceUtility::Convert4ByteAddressTo8BytePointer<TFixedArray<float, 8>>(musicCompositionData, fixedArrayAddress);

				for (unsigned int l = 0; l < fixedArray->Size(); ++l)
				{
					data[k].m_pStart[l] = (*fixedArray)[l];
				}
			}

			const unsigned int dataStartAddress2 = *reinterpret_cast<unsigned int*>(customFadeOutCurve);
			const unsigned int dataEndAddress2 = *(reinterpret_cast<unsigned int*>(customFadeOutCurve) + 1);
			const unsigned int dataCount2 = ResourceUtility::CalculateArrayElementsCount(dataStartAddress2, dataEndAddress2, 0x8 * sizeof(float));
			TArray<TFixedArray<float, 8>>& data2 = sparseShortArray[j].m_customFadeOutCurve.GetData();

			data2.Resize(dataCount2);

			for (unsigned int k = 0; k < dataCount2; k++)
			{
				const unsigned int fixedArrayAddress2 = dataStartAddress2 + 0x8 * sizeof(float) * k;
				TFixedArray<float, 8>* fixedArray2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<TFixedArray<float, 8>>(musicCompositionData, fixedArrayAddress2);

				for (unsigned int l = 0; l < fixedArray2->Size(); ++l)
				{
					data2[k].m_pStart[l] = (*fixedArray2)[l];
				}
			}
		}
	}
}

void CompositionBlueprint::ParseTrackExposingGroups(void* musicCompositionData)
{
	const unsigned int trackExposingGroupsAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 19);
	const unsigned int trackExposingGroupsEndAddress = *(reinterpret_cast<unsigned int*>(musicCompositionData) + 20);
	const unsigned int trackExposingGroupCount = ResourceUtility::CalculateArrayElementsCount(trackExposingGroupsAddress, trackExposingGroupsEndAddress, 0x20); //0x20 is size of SMusicTrackExposingGroupData

	this->musicCompositionData->m_aTrackExposingGroups.Resize(trackExposingGroupCount);

	for (unsigned int i = 0; i < trackExposingGroupCount; i++)
	{
		const unsigned int musicTrackExposingGroupDataAddress = trackExposingGroupsAddress + 0x20 * i;
		SMusicTrackExposingGroupData* musicTrackExposingGroupData = ResourceUtility::Convert4ByteAddressTo8BytePointer<SMusicTrackExposingGroupData>(musicCompositionData, musicTrackExposingGroupDataAddress);

		const unsigned int charsAddress = *(reinterpret_cast<unsigned int*>(musicTrackExposingGroupData) + 1);
		const char* chars = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress);

		this->musicCompositionData->m_aTrackExposingGroups[i].m_sName = ZString(chars);

		const unsigned int trackNamesAddress = *(reinterpret_cast<unsigned int*>(musicTrackExposingGroupData) + 2);
		const unsigned int trackNamesEndAddress = *(reinterpret_cast<unsigned int*>(musicTrackExposingGroupData) + 3);
		const unsigned int trackNameCount = ResourceUtility::CalculateArrayElementsCount(trackNamesAddress, trackNamesEndAddress, 0x8); //0x8 is size of ZString

		this->musicCompositionData->m_aTrackExposingGroups[i].m_trackNames.Resize(trackNameCount);

		for (unsigned int j = 0; j < trackNameCount; j++)
		{
			const unsigned int trackNameAddress = trackNamesAddress + 0x8 * j;
			ZString* trackName = ResourceUtility::Convert4ByteAddressTo8BytePointer<ZString>(musicCompositionData, trackNameAddress);

			const unsigned int charsAddress2 = *(reinterpret_cast<unsigned int*>(trackName) + 1);
			const char* chars2 = ResourceUtility::Convert4ByteAddressTo8BytePointer<const char>(musicCompositionData, charsAddress2);

			this->musicCompositionData->m_aTrackExposingGroups[i].m_trackNames[j] = ZString(chars2);
		}

		const unsigned int trackTypesAddress = *(reinterpret_cast<unsigned int*>(musicTrackExposingGroupData) + 5);
		const unsigned int trackTypesEndAddress = *(reinterpret_cast<unsigned int*>(musicTrackExposingGroupData) + 6);
		const unsigned int trackTypeCount = ResourceUtility::CalculateArrayElementsCount(trackTypesAddress, trackTypesEndAddress, sizeof(EMusicTrackExposingType));

		this->musicCompositionData->m_aTrackExposingGroups[i].m_trackTypes.Resize(trackTypeCount);

		for (unsigned int j = 0; j < trackTypeCount; j++)
		{
			const unsigned int trackTypeAddress = trackTypesAddress + sizeof(EMusicTrackExposingType) * j;
			EMusicTrackExposingType* trackType = ResourceUtility::Convert4ByteAddressTo8BytePointer<EMusicTrackExposingType>(musicCompositionData, trackTypeAddress);

			this->musicCompositionData->m_aTrackExposingGroups[i].m_trackTypes[j] = *trackType;
		}
	}
}

std::shared_ptr<SMusicCompositionData> CompositionBlueprint::GetMusicCompositionData()
{
	return musicCompositionData;
}
