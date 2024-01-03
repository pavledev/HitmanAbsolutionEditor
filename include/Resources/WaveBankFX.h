#pragma once

#include "Resource.h"

class WaveBankFX : public Resource
{
public:
	struct FaceFXAnimationReference
	{
		unsigned int animationSetResourceIndex;
		std::string animationName;
	};

	struct SubtitleReference
	{
		unsigned int textlistResourceIndex;
		unsigned int hashedLoganID;
		std::string textListEntryName;
	};

	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	std::vector<FaceFXAnimationReference>& GetFaceFXAnimationReferences();
	std::vector<SubtitleReference>& GetSubtitleReferences();
	void SerializeToJson(const std::string& outputFilePath);

private:
	std::vector<FaceFXAnimationReference> faceFXAnimationReferences;
	std::vector<SubtitleReference> subtitleReferences;
};
