#pragma once

#include <d3d11.h>
#include <vector>

#include "Object.h"

class VertexBuffer : public Object
{
public:
    VertexBuffer() = default;
    ~VertexBuffer();

    template <typename T>
    void Create(const std::vector<T>& vertices)
    {
        stride = static_cast<unsigned int>(sizeof(T));
        vertexCount = static_cast<unsigned int>(vertices.size());
        gpuSize = static_cast<unsigned long long>(stride * vertexCount);

        Create(static_cast<const void*>(vertices.data()));
    }

    template <typename T>
    void Create(const T* vertices, const unsigned int vertexCount)
    {
        stride = static_cast<unsigned int>(sizeof(T));
        this->vertexCount = vertexCount;
        gpuSize = static_cast<unsigned long long>(stride * vertexCount);

        Create(static_cast<const void*>(vertices));
    }

    template <typename T>
    void CreateDynamic(const unsigned int vertexCount)
    {
        stride = static_cast<unsigned int>(sizeof(T));
        this->vertexCount = vertexCount;
        gpuSize = static_cast<unsigned long long>(stride * vertexCount);

        Create(nullptr);
    }

    void Create(const void* vertices, const unsigned int vertexCount, const unsigned int stride);
    void* Map();
    void Unmap();
    ID3D11Buffer* GetBuffer() const;
    unsigned int GetStride() const;
    unsigned int GetVertexCount() const;

private:
    void Create(const void* vertices);

    unsigned int stride = 0;
    unsigned int vertexCount = 0;
    ID3D11Buffer* buffer = nullptr;
};
