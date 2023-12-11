#pragma once

#include <cstddef>
#include <memory>

size_t allocationSize = 0;

namespace Allocation
{
    size_t GetAllocatedMemorySize()
    {
        return allocationSize;
    }
}

void* operator new(std::size_t size)
{
    if (size == 0)
    {
        size = 1;
    }

    allocationSize += size;

    return std::malloc(size);
}

void operator delete(void* ptr, size_t size) noexcept
{
    allocationSize -= size;

    std::free(ptr);
}
