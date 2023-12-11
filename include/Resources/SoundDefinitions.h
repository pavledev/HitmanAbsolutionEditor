#pragma once

#include <string>
#include <vector>

#include "Glacier/Sound/SActorSoundDefs.h"
#include "Glacier/Sound/ESoundPlayParameters.h"

#include "IO/BinaryReader.h"
#include "Resources/Resource.h"

class SoundDefinitions
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
        unsigned int numSubSoundRepeatCounts;
        std::vector<unsigned char> subSoundRepeatCounts;
    };

    void Deserialize(const std::string& filePath);
    void Deserialize(void* buffer, const unsigned int dataSize);
    void Deserialize(BinaryReader& binaryReader);
    void SerializeToJson(const Resource* resource, std::string& jsonOutput);

private:
    std::vector<Entry> entries;
};
