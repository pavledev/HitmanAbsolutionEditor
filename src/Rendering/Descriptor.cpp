#include "Rendering/Descriptor.h"

Descriptor::Descriptor(const Descriptor& descriptor)
{
    type = descriptor.type;
    layout = descriptor.layout;
    slot = descriptor.slot;
    stage = descriptor.stage;
    name = descriptor.name;
    mip = descriptor.mip;
    arraySize = descriptor.arraySize;
}

Descriptor::Descriptor(const std::string& name, const Type type, const ImageLayout layout, const unsigned int slot, const unsigned int arraySize, const unsigned int stage)
{
    this->type = type;
    this->layout = layout;
    this->slot = slot;
    this->stage = stage;
    this->name = name;
    this->arraySize = arraySize;
}

bool Descriptor::IsStorage() const
{
    return type == Type::TextureStorage;
}

bool Descriptor::IsArray() const
{
    return arraySize > 0;
}
