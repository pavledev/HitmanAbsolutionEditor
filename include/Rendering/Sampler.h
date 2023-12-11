#pragma once

#include <d3d11.h>

class Sampler
{
public:
    enum class Filter
    {
        Point,
        Bilinear,
        Trilinear,
        AnisotropicPoint,
        AnisotropicLinear
    };

    enum class AddressMode
    {
        Wrap,
        Clamp,
        Mirror,
        Border
    };

    enum class CompareFunction
    {
        Never,
        Less
    };

    Sampler(
        const Filter filter = Sampler::Filter::Point,
        const AddressMode addressU = AddressMode::Clamp,
        const AddressMode addressV = AddressMode::Clamp,
        const AddressMode addressW = AddressMode::Clamp,
        const float mipLodBias = 0.f,
        const unsigned int maxAnisotropy = D3D11_MAX_MAXANISOTROPY,
        const unsigned int borderColor = 0,
        const CompareFunction compareFunction = CompareFunction::Never
    );
    ~Sampler();
    ID3D11SamplerState* GetSamplerState() const;
    static D3D11_TEXTURE_ADDRESS_MODE ConvertAddressMode(const AddressMode addressMode);
    static D3D11_COMPARISON_FUNC ConvertCompareFunction(const CompareFunction compareFunction);

private:
    void CreateResource();

    Filter filter;
    D3D11_TEXTURE_ADDRESS_MODE addressU;
    D3D11_TEXTURE_ADDRESS_MODE addressV;
    D3D11_TEXTURE_ADDRESS_MODE addressW;
    float mipLodBias;
    unsigned int maxAnisotropy;
    unsigned int borderColor;
    bool isComparisonEnabled;
    D3D11_COMPARISON_FUNC comparisonFunction;
    ID3D11SamplerState* samplerState = nullptr;
};
