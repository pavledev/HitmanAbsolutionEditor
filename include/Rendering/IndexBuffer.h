#pragma once

#include <d3d11.h>
#include <vector>

#include "Object.h"

class IndexBuffer : public Object
{
public:
    IndexBuffer() = default;
    ~IndexBuffer();

    template<typename T>
    void Create(const std::vector<T>& indices)
    {
        stride = sizeof(T);
        indexCount = static_cast<unsigned int>(indices.size());
        gpuSize = static_cast<unsigned long long>(stride * indexCount);

        Create(static_cast<const void*>(indices.data()));
    }

    template<typename T>
    void Create(const T* indices, const unsigned int indexCount)
    {
        stride = sizeof(T);
        this->indexCount = indexCount;
        gpuSize = static_cast<unsigned long long>(stride * indexCount);

        Create(static_cast<const void*>(indices));
    }

    template<typename T>
    void CreateDynamic(const unsigned int indexCount)
    {
        stride = sizeof(T);
        this->indexCount = indexCount;
        gpuSize = static_cast<unsigned long long>(stride * indexCount);

        Create(nullptr);
    }

    void* Map();
    void Unmap();
    ID3D11Buffer* GetBuffer() const;
    unsigned int GetStride() const;
    unsigned int GetIndexCount() const;
    bool Is16Bit() const;
    bool Is32Bit() const;

private:
    void Create(const void* indices);

    unsigned int stride = 0;
    unsigned int indexCount = 0;
    ID3D11Buffer* buffer = nullptr;
};
