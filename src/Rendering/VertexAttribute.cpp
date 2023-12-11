#include "Rendering/VertexAttribute.h"

VertexAttribute::VertexAttribute(const std::string& name, const unsigned int location, const uint32_t binding, const DXGI_FORMAT format, const unsigned int offset)
{
    this->name = name;
    this->location = location;
    this->binding = binding;
    this->format = format;
    this->offset = offset;
}
