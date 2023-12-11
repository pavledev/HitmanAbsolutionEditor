#pragma once

#include <string>
#include <dxgiformat.h>

struct VertexAttribute
{
    VertexAttribute(const std::string& name, const unsigned int location, const uint32_t binding, const DXGI_FORMAT format, const unsigned int offset);

    std::string name;
    unsigned int location;
    unsigned int binding;
    DXGI_FORMAT format;
    unsigned int offset;
};
