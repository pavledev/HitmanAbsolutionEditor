#pragma once

#include <string>
#include <vector>
#include <functional>

#include "Glacier/Resource/SResourceHeaderHeader.h"
#include "Glacier/Resource/ZRuntimeResourceID.h"
#include "Glacier/Resource/EResourceReferenceFlags.h"
#include "IO/BinaryReader.h"

class Resource
{
public:
    struct HeaderLibrary
    {
        std::string resourceID;
        unsigned int chunkIndex;
        unsigned int indexInLibrary;
    };

    using ResourceLoadedCallback = std::function<void()>;

    Resource();
    virtual ~Resource();
    const std::string GetHeaderLibraryFilePath() const;
    const std::string GetResourceLibraryFilePath() const;
    const std::string GetName() const;
    const SResourceHeaderHeader& GetResourceHeaderHeader() const;
    const ZRuntimeResourceID& GetRuntimeResourceID() const;
    const unsigned long long GetHash() const;
    std::string GetResourceID() const;
    std::string GetResourceID();
    std::string GetHeaderLibraryChunkResourceID() const;
    unsigned int GetIndexInLibrary() const;
    const EResourceReferenceFlags GetResourceReferenceFlags() const;
    const unsigned int GetOffsetInHeaderLibrary() const;
    const unsigned int GetOffsetInResourceLibrary() const;
    const void* GetHeaderData() const;
    const unsigned int GetHeaderDataSize() const;
    const void* GetResourceData() const;
    void* GetResourceData();
    const unsigned int GetResourceDataSize() const;
    const std::vector<std::shared_ptr<Resource>>& GetReferences() const;
    std::vector<std::shared_ptr<Resource>>& GetReferences();
    const std::vector<std::shared_ptr<Resource>>& GetBackReferences() const;
    std::vector<std::shared_ptr<Resource>>& GetBackReferences();
    void SetHeaderLibraryFilePath(std::string headerLibraryFilePath);
    void SetResourceLibraryFilePath(std::string resourceLibraryFilePath);
    void SetName(std::string name);
    void SetResourceHeaderHeader(const SResourceHeaderHeader& resourceHeaderHeader);
    void SetHash(const unsigned long long hash);
    void SetRuntimeResourceID(const ZRuntimeResourceID& runtimeResourceID);
    void SetResourceID(std::string resourceID);
    void SetResourceReferenceFlags(const EResourceReferenceFlags resourceReferenceFlags);
    void SetHeaderData(void* headerData);
    void SetHeaderDataSize(const unsigned int headerDataSize);
    void SetResourceData(void* resourceData);
    void SetResourceDataSize(const unsigned int resourceDataSize);
    void LoadResourceData();
    void LoadResourceData(BinaryReader& headerLibraryBinaryReader, const unsigned int resourceHeadersStartOffset, unsigned int resourceHeaderOffset);
    void DeleteResourceData();
    const bool HasReferences() const;
    void LoadResource(const unsigned int headerLibraryIndex, const unsigned int chunkIndex, const unsigned int indexInLibrary, const bool loadReferences, const bool loadBackReferences, const bool loadResourceData);
    void LoadReferences();
    void LoadReferences(const BinaryReader& headerDataBinaryReader);
    void LoadBackReferences(BinaryReader& headerLibraryBinaryReader, const unsigned int referencesChunkSize);
    bool IsResourceLoaded() const;
    const std::vector<HeaderLibrary>* GetHeaderLibraries() const;
    void SetHeaderLibraries(const std::vector<HeaderLibrary>* headerLibraries);
    void AddReference(const ZRuntimeResourceID& runtimeResourceID, const EResourceReferenceFlags resourceReferenceFlags);
    void SetResourceLoadedCallback(ResourceLoadedCallback resourceLoadedCallback);
    const bool IsResourceDeserialized() const;

    virtual void Deserialize()
    {
        isResourceDeserialized = false;
    }

protected:
    std::string headerLibraryFilePath;
    std::string resourceLibraryFilePath;
    std::string name;
    SResourceHeaderHeader resourceHeaderHeader;
    ZRuntimeResourceID runtimeResourceID;
    unsigned long long hash;
    std::string resourceID;
    std::string headerLibraryChunkResourceID;
    unsigned int indexInLibrary;
    EResourceReferenceFlags resourceReferenceFlags;
    std::string fileFormat;
    unsigned int offsetInHeaderLibrary;
    unsigned int offsetInResourceLibrary;
    void* headerData;
    unsigned int headerDataSize;
    void* resourceData;
    unsigned int resourceDataSize;
    std::vector<std::shared_ptr<Resource>> references;
    std::vector<std::shared_ptr<Resource>> backReferences;
    const std::vector<HeaderLibrary>* headerLibraries;
    bool isResourceLoaded;
    bool isResourceDeserialized;
    ResourceLoadedCallback resourceLoadedCallback;
};
