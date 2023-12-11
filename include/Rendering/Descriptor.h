#pragma once

#include <string>

#include "ImageLayout.h"

class Descriptor
{
public:
    enum class Type
    {
        Sampler,
        Texture,
        TextureStorage,
        ConstantBuffer,
        StructuredBuffer,
        Undefined
    };

    Descriptor() = default;

    Descriptor(const Descriptor& descriptor);
    Descriptor(const std::string& name, const Type type, const ImageLayout layout, const unsigned int slot, const unsigned int array_size, const unsigned int stage);
    bool IsStorage() const;
    bool IsArray() const;

    // Properties that affect the hash. They are reflected from the shader
    Type type = Type::Undefined;
    unsigned int slot = 0; // the binding slot in the shader
    unsigned int stage = 0; // the pipeline stages from which which the descriptor resources is accessed from
    unsigned int arraySize = 0; // the size of the array in the shader

    // Properties that don't affect the hash. Data that simply needs to be passed around
    unsigned int dynamicOffset = 0; // the offset used for dynamic constant buffers
    uint64_t range = 0; // the size in bytes that is used for a descriptor update
    unsigned int mip = 0;
    unsigned int mipRange = 0;
    void* data = nullptr;
    ImageLayout layout = ImageLayout::Undefined;
    std::string name; // Kept here for debugging purposes
};
