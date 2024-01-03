#include "Utility/ResourceUtility.h"
#include "Settings.h"
#include "Resources/CppEntity.h"
#include "Resources/CppEntityBlueprint.h"
#include "Resources/TemplateEntity.h"
#include "Resources/TemplateEntityBlueprint.h"
#include "Resources/Texture.h"
#include "Resources/TextList.h"
#include "Resources/Localization.h"
#include "Resources/MultiLanguage.h"
#include "Resources/RenderMaterialEntityType.h"
#include "Resources/RenderMaterialEntityBlueprint.h"
#include "Resources/RenderMaterialInstance.h"
#include "Resources/RenderMaterialEffect.h"
#include "Resources/SoundBlendContainerExternalParametersType.h"
#include "Resources/SoundBlendContainerExternalParametersBlueprint.h"
#include "Resources/WaveBankFSBF.h"
#include "Resources/WaveBankFSBM.h"
#include "Resources/WaveBankFSBS.h"
#include "Resources/WaveBank.h"
#include "Resources/WaveBankFX.h"
#include "Resources/FlashMovie.h"
#include "Resources/GFXMovie.h"
#include "Resources/AnimationDatabase.h"
#include "Resources/SoundDefinitions.h"
#include "Resources/GlobalResourceIndex.h"
#include "Resources/BehaviorTreeEntityBlueprint.h"
#include "Resources/CompositionBlueprint.h"
#include "Resources/PackageList.h"
#include "Resources/ScatterData.h"
#include "Resources/FabricResourceEntity.h"
#include "Resources/FabricResourceEntityBlueprint.h"
#include "Registry/ResourceInfoRegistry.h"

std::string ResourceUtility::ConvertResourceIDToFilePath(const std::string& resourceID)
{
    std::string resourceID2 = StringUtility::ToLowerCase(resourceID);
    std::string extension = resourceID2.substr(resourceID2.find_last_of('.') + 1);
    int index = static_cast<int>(resourceID2.find(':'));
    int index2 = static_cast<int>(resourceID2.find('?'));
    int index3 = static_cast<int>(resourceID2.find(']'));
    std::string folderPath = Settings::GetInstance().GetRuntimeFolderPath();

    if (index2 != -1 && index < index2)
    {
        folderPath += resourceID2.substr(index + 1, index2 - index - 1);
    }
    else
    {
        folderPath += resourceID2.substr(index + 1, index3 - index - 1);
    }

    resourceID2 = resourceID2.substr(0, resourceID2.find_last_of('.') + 1);

    Hash::MD5Hash md5Hash = Hash::MD5(resourceID2);
    std::string fileName = Hash::ConvertMD5ToString(md5Hash);

    return std::format("{}/{}.{}", folderPath, fileName, extension);
}

std::string ResourceUtility::ConvertResourceReferenceFlagsToString(const EResourceReferenceFlags resourceReferenceFlags)
{
    std::string flags;

    if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_TYPE_OF_STREAMING_ENTITY) == EResourceReferenceFlags::RESOURCE_REFERENCE_TYPE_OF_STREAMING_ENTITY)
    {
        flags += "Type Of Streaming Entity, ";
    }

    if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_STATE_STREAMED) == EResourceReferenceFlags::RESOURCE_REFERENCE_STATE_STREAMED)
    {
        flags += "State Streamed, ";
    }

    if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_MEDIA_STREAMED) == EResourceReferenceFlags::RESOURCE_REFERENCE_MEDIA_STREAMED)
    {
        flags += "Media Streamed, ";
    }

    if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY) == EResourceReferenceFlags::RESOURCE_REFERENCE_INSTALL_DEPENDENCY)
    {
        flags += "Install Dependency, ";
    }

    if ((resourceReferenceFlags & EResourceReferenceFlags::RESOURCE_REFERENCE_CYCLIC) == EResourceReferenceFlags::RESOURCE_REFERENCE_CYCLIC)
    {
        flags += "Cyclic";
    }

    if (flags.empty())
    {
        flags = "No Flags";
    }
    else if (flags.ends_with(", "))
    {
        flags = flags.erase(flags.length() - 2, 2);
    }

    return flags;
}

std::string ResourceUtility::ConvertHeaderLibraryChunkFlagsToString(const HeaderLibraryChunkFlags headerLibraryChunkFlags)
{
    std::string flags;

    if ((headerLibraryChunkFlags & HeaderLibraryChunkFlags::GLOBAL) == HeaderLibraryChunkFlags::GLOBAL)
    {
        flags += "Global, ";
    }

    if ((headerLibraryChunkFlags & HeaderLibraryChunkFlags::NON_STREAMED) == HeaderLibraryChunkFlags::NON_STREAMED)
    {
        flags += "Non-streamed, ";
    }

    if ((headerLibraryChunkFlags & HeaderLibraryChunkFlags::MEDIA_STREAMED) == HeaderLibraryChunkFlags::MEDIA_STREAMED)
    {
        flags += "Media-streamed, ";
    }

    if ((headerLibraryChunkFlags & HeaderLibraryChunkFlags::INIT_LIBRARY) == HeaderLibraryChunkFlags::INIT_LIBRARY)
    {
        flags += "Init library, ";
    }

    if ((headerLibraryChunkFlags & HeaderLibraryChunkFlags::DYNAMIC) == HeaderLibraryChunkFlags::DYNAMIC)
    {
        flags += "Dynamic";
    }

    if (flags.length() == 0)
    {
        flags = "None";
    }
    else if (flags.ends_with(", "))
    {
        flags = flags.erase(flags.length() - 2, 2);
    }

    return flags;
}

std::string ResourceUtility::GetResourceName(const std::string& resourceID)
{
    std::string name;

    if (resourceID.contains("("))
    {
        size_t index = resourceID.substr(0, resourceID.find("(")).find_last_of('/');

        name = resourceID.substr(index + 1, resourceID.find("(") - 2 - index);
    }
    else
    {
        size_t index = resourceID.find("?");

        if (index != -1 && resourceID[index + 1] != '/')
        {
            name = resourceID.substr(resourceID.find("?") + 1, resourceID.find("]") - resourceID.find("?") - 1);
        }
        else
        {
            name = resourceID.substr(resourceID.find_last_of("/") + 1, resourceID.find("]") - resourceID.find_last_of("/") - 1);
        }
    }

    return name;
}

std::string ResourceUtility::ConvertResourceTypeToString(const unsigned int type)
{
    std::stringstream stream;

    stream << std::hex << type;

    std::string hex(stream.str());

    int length = hex.length();
    std::string result;

    for (int i = 0; i < length; i += 2)
    {
        std::string byte = hex.substr(i, 2);
        char chr = static_cast<char>(static_cast<int>(strtol(byte.c_str(), nullptr, 16)));

        result.push_back(chr);
    }

    return result;
}

std::shared_ptr<Resource> ResourceUtility::CreateResource(const std::string& type)
{
    std::shared_ptr<Resource> resource;

    if (type == "CPPT")
    {
        resource = std::make_shared<CppEntity>();
    }
    else if (type == "CBLU")
    {
        resource = std::make_shared<CppEntityBlueprint>();
    }
    else if (type == "TEMP")
    {
        resource = std::make_shared<TemplateEntity>();
    }
    else if (type == "TBLU")
    {
        resource = std::make_shared<TemplateEntityBlueprint>();
    }
    else if (type == "TEXT")
    {
        resource = std::make_shared<Texture>();
    }
    else if (type == "TELI")
    {
        resource = std::make_shared<TextList>();
    }
    else if (type == "LOCR")
    {
        resource = std::make_shared<Localization>();
    }
    else if (type == "LOCM")
    {
        resource = std::make_shared<MultiLanguage>();
    }
    else if (type == "MATT")
    {
        resource = std::make_shared<RenderMaterialEntityType>();
    }
    else if (type == "MATB")
    {
        resource = std::make_shared<RenderMaterialEntityBlueprint>();
    }
    else if (type == "MATI")
    {
        resource = std::make_shared<RenderMaterialInstance>();
    }
    else if (type == "MATE")
    {
        resource = std::make_shared<RenderMaterialEffect>();
    }
    else if (type == "SBPD")
    {
        resource = std::make_shared<SoundBlendContainerExternalParametersType>();
    }
    else if (type == "SBPB")
    {
        resource = std::make_shared<SoundBlendContainerExternalParametersBlueprint>();
    }
    else if (type == "FSBF")
    {
        resource = std::make_shared<WaveBankFSBF>();
    }
    else if (type == "FSBM")
    {
        resource = std::make_shared<WaveBankFSBM>();
    }
    else if (type == "FSBS")
    {
        resource = std::make_shared<WaveBankFSBS>();
    }
    else if (type == "WAVB")
    {
        resource = std::make_shared<WaveBank>();
    }
    else if (type == "WBFX")
    {
        resource = std::make_shared<WaveBankFX>();
    }
    else if (type == "SWFF")
    {
        resource = std::make_shared<FlashMovie>();
    }
    else if (type == "GFXF")
    {
        resource = std::make_shared<GFXMovie>();
    }
    else if (type == "ChrT")
    {
        resource = std::make_shared<AnimationDatabase>();
    }
    else if (type == "SDEF")
    {
        resource = std::make_shared<SoundDefinitions>();
    }
    else if (type == "GIDX")
    {
        resource = std::make_shared<GlobalResourceIndex>();
    }
    else if (type == "AIBB")
    {
        resource = std::make_shared<BehaviorTreeEntityBlueprint>();
    }
    else if (type == "MUCB")
    {
        resource = std::make_shared<CompositionBlueprint>();
    }
    else if (type == "PKGL")
    {
        resource = std::make_shared<PackageList>();
    }
    else if (type == "SCDA")
    {
        resource = std::make_shared<ScatterData>();
    }
    else if (type == "CLOT")
    {
        resource = std::make_shared<FabricResourceEntity>();
    }
    else if (type == "CLOB")
    {
        resource = std::make_shared<FabricResourceEntityBlueprint>();
    }
    else
    {
        resource = std::make_shared<Resource>();
    }

    return resource;
}

void ResourceUtility::LoadResource(std::shared_ptr<Resource> resource)
{
    const ResourceInfoRegistry::ResourceInfo& resourceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(resource->GetHash());

    if (resourceInfo.headerLibraries.size() > 0)
    {
        resource->LoadResource(0, resourceInfo.headerLibraries[0].chunkIndex, resourceInfo.headerLibraries[0].indexInLibrary, true, true, true);
    }
    else
    {
        resource->LoadResource(0, -1, -1, false, false, true);
    }
}

std::string ResourceUtility::GenerateFileName(std::shared_ptr<Resource> resource)
{
    return std::format("{}_{}", resource->GetName(), StringUtility::ConvertValueToHexString(resource->GetRuntimeResourceID()));
}
