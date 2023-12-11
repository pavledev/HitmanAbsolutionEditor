#pragma once

#include <d3d11.h>
#include <cassert>
#include <type_traits>

#include "Object.h"

class ConstantBuffer : public Object
{
public:
    ConstantBuffer() = default;
    ~ConstantBuffer();
    unsigned int GetStride() const;
    unsigned int GetOffset() const;
    unsigned int GetStrideCount() const;
    ID3D11Buffer* GetBuffer() const;

    template <typename T>
    void Create(const unsigned int elementCount)
    {
        static_assert(std::is_trivially_copyable_v<T>, "Type is not trivially copyable");
        assert(sizeof(T) % 16 == 0 && "The size is not a multiple of 16");
        assert(elementCount != 0 && "Element count can't be zero");

        this->elementCount = elementCount;
        stride = static_cast<unsigned int>(sizeof(T));
        gpuSize = static_cast<unsigned long long>(stride * elementCount);

        CreateResource();
    }

    void Update(void* dataCpu);
    void ResetOffset();

private:
    void CreateResource();

    unsigned int stride = 0;
    unsigned int offset = 0;
    unsigned int elementCount = 0;
    bool hasUpdated = true;
    void* mappedData = nullptr;
    ID3D11Buffer* buffer = nullptr;
};
