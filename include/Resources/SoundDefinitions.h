#pragma once

#include <string>
#include <vector>

#include "Glacier/Sound/SActorSoundDefs.h"
#include "Glacier/Sound/ESoundPlayParameters.h"

#include "Resource.h"

class SoundDefinitions : public Resource
{
public:
    struct Entry
    {
        void SerializeToJson(const std::vector<std::shared_ptr<Resource>>& references, rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
        static std::string ConvertSoundDefinitionToString(const SActorSoundDefs::EDefinition definition);
        static std::string ConvertSoundPlayParametersToString(const ESoundPlayParameters soundPlayParameters);

        SActorSoundDefs::EDefinition definition;
        unsigned int referenceIndex;
        int attenuationOffset;
        int groupNumber;
        ESoundPlayParameters selectionMode;
        int noRepeatsCount;
        std::vector<unsigned char> subSoundRepeatCounts;
    };

    void Deserialize();
    void SerializeToJson(const Resource* resource, std::string& jsonOutput);
    std::vector<Entry>& GetEntries();

private:
    std::vector<Entry> entries;
};
