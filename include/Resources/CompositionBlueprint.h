#pragma once

#include <Glacier/Sound/SMusicCompositionData.h>

#include "Resource.h"

class CompositionBlueprint : public Resource
{
public:
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void Parse(void* musicCompositionData);
	void ParseGameCues(void* musicCompositionData);
	void ParsePlayLists(void* musicCompositionData);
	void ParseStingers(void* musicCompositionData);
	void ParseSegments(void* musicCompositionData);
	void ParseEndBehaviors(void* musicCompositionData);
	void ParseTransitionMap(void* musicCompositionData);
	void ParseTrackExposingGroups(void* musicCompositionData);
	std::shared_ptr<SMusicCompositionData> GetMusicCompositionData();

private:
	std::shared_ptr<SMusicCompositionData> musicCompositionData;
};
