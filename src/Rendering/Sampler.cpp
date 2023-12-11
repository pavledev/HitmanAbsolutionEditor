#include <cfloat>

#include "Rendering/Sampler.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/D3D11Utility.h"
#include "Math/LinearColor.h"
#include "Editor.h"

Sampler::Sampler(const Filter filter,
    const AddressMode addressU,
    const AddressMode addressV,
    const AddressMode addressW,
    const float mipLodBias,
    const unsigned int maxAnisotropy,
    const unsigned int borderColor,
    const CompareFunction compareFunction
)
{
    samplerState = nullptr;
    this->filter = filter;
    this->addressU = ConvertAddressMode(addressU);
    this->addressV = ConvertAddressMode(addressV);
    this->addressW = ConvertAddressMode(addressW);
    this->mipLodBias = mipLodBias;
    this->maxAnisotropy = maxAnisotropy;
    this->borderColor = borderColor;
    this->comparisonFunction = ConvertCompareFunction(compareFunction);

    CreateResource();
}

Sampler::~Sampler()
{
    D3D11Utility::Release(samplerState);
}

ID3D11SamplerState* Sampler::GetSamplerState() const
{
    return samplerState;
}

D3D11_TEXTURE_ADDRESS_MODE Sampler::ConvertAddressMode(const AddressMode addressMode)
{
    switch (addressMode)
    {
        case AddressMode::Clamp:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        case AddressMode::Mirror:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        case AddressMode::Border:
            return D3D11_TEXTURE_ADDRESS_BORDER;
        default:
            return D3D11_TEXTURE_ADDRESS_WRAP;
    };
}

D3D11_COMPARISON_FUNC Sampler::ConvertCompareFunction(const CompareFunction compareFunction)
{
    switch (compareFunction)
    {
        case CompareFunction::Less:
            return D3D11_COMPARISON_LESS;
        case CompareFunction::Never:
        default:
            return D3D11_COMPARISON_NEVER;
    };
}

void Sampler::CreateResource()
{
    D3D11_SAMPLER_DESC samplerDesc = {};

    samplerDesc.AddressU = addressU;
    samplerDesc.AddressV = addressV;
    samplerDesc.AddressW = addressW;
    samplerDesc.MipLODBias = mipLodBias;
    samplerDesc.MaxAnisotropy = maxAnisotropy;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = FLT_MAX;

    // Determine whether we should use one of the comparison modes
    isComparisonEnabled = comparisonFunction != D3D11_COMPARISON_NEVER;

    switch (filter)
    {
        case Filter::AnisotropicLinear:
        case Filter::AnisotropicPoint:
            if (maxAnisotropy == 1)
            {
                samplerDesc.Filter = isComparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            }
            else
            {
                // D3D11 doesn't allow using point filtering for mip filter when using anisotropic filtering
                samplerDesc.Filter = isComparisonEnabled ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;
            }

            break;
        case Filter::Trilinear:
            samplerDesc.Filter = isComparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            break;
        case Filter::Bilinear:
            samplerDesc.Filter = isComparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
            break;
        case Filter::Point:
            samplerDesc.Filter = isComparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;
            break;
    }

    const LinearColor LinearBorderColor = Color(borderColor);
    samplerDesc.BorderColor[0] = LinearBorderColor.r;
    samplerDesc.BorderColor[1] = LinearBorderColor.g;
    samplerDesc.BorderColor[2] = LinearBorderColor.b;
    samplerDesc.BorderColor[3] = LinearBorderColor.a;
    samplerDesc.ComparisonFunc = comparisonFunction;

    ID3D11Device* device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    const HRESULT result = device->CreateSamplerState(&samplerDesc, &samplerState);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create sampler state!");
    }
}
