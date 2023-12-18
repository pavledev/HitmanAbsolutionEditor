#include "Utility/ResourceUtility.h"
#include "Settings.h"
#include "Resources/CppEntity.h"
#include "Resources/CppEntityBlueprint.h"
#include "Resources/TemplateEntity.h"
#include "Resources/TemplateEntityBlueprint.h"

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
    else
    {
        resource = std::make_shared<Resource>();
    }

    return resource;
}
