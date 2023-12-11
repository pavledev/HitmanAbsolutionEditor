#pragma once

#include <vector>
#include <d3d11.h>

#include "VertexType.h"
#include "VertexAttribute.h"
#include "Vertex.h"

class InputLayout
{
public:
    InputLayout() = default;
    ~InputLayout();
    VertexType GetVertexType() const;
    const unsigned int GetVertexSize() const;
    const std::vector<VertexAttribute>& GetAttributeDescriptions() const;
    unsigned int GetAttributeCount() const;
    ID3D11InputLayout* GetInputLayout() const;
    bool operator==(const InputLayout& rhs) const;
    void Create(const VertexType vertexType, void* vertexShaderBlob = nullptr);

private:
    bool CreateResource(void* vertexShaderBlob);

    VertexType vertexType;
    unsigned int vertexSize;
    std::vector<VertexAttribute> vertexAttributes;
    ID3D11InputLayout* inputLayout = nullptr;
};
