#pragma once

class Object
{
public:
    const unsigned long long GetGPUSize() const;

protected:
    unsigned long long cpuSize = 0;
    unsigned long long gpuSize = 0;
};
