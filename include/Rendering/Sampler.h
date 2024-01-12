#pragma once

#include <d3d11.h>

class Sampler
{
public:
    //
    // Summary:
    //     Filtering options during texture sampling.
    //
    // Remarks:
    //     Note??If you use different filter types for min versus mag filter, undefined
    //     behavior occurs in certain cases where the choice between whether magnification
    //     or minification happens is ambiguous. To prevent this undefined behavior, use
    //     filter modes that use similar filter operations for both min and mag (or use
    //     anisotropic filtering, which avoids the issue as well).?During texture sampling,
    //     one or more texels are read and combined (this is calling filtering) to produce
    //     a single value. Point sampling reads a single texel while linear sampling reads
    //     two texels (endpoints) and linearly interpolates a third value between the endpoints.HLSL
    //     texture-sampling functions also support comparison filtering during texture sampling.
    //     Comparison filtering compares each sampled texel against a comparison value.
    //     The boolean result is blended the same way that normal texture filtering is blended.You
    //     can use HLSL intrinsic texture-sampling functions that implement texture filtering
    //     only or companion functions that use texture filtering with comparison filtering.
    //     Texture Sampling FunctionTexture Sampling Function with Comparison Filtering
    //     samplesamplecmp or samplecmplevelzero ?Comparison filters only work with textures
    //     that have the following DXGI formats: R32_FLOAT_X8X24_TYPELESS, R32_FLOAT, R24_UNORM_X8_TYPELESS,
    //     R16_UNORM.
    enum class Filter
    {
        //
        // Summary:
        //     Use point sampling for minification, magnification, and mip-level sampling.
        MinMagMipPoint = 0,
        //
        // Summary:
        //     Use point sampling for minification and magnification; use linear interpolation
        //     for mip-level sampling.
        MinMagPointMipLinear = 1,
        //
        // Summary:
        //     Use point sampling for minification; use linear interpolation for magnification;
        //     use point sampling for mip-level sampling.
        MinPointMagLinearMipPoint = 4,
        //
        // Summary:
        //     Use point sampling for minification; use linear interpolation for magnification
        //     and mip-level sampling.
        MinPointMagMipLinear = 5,
        //
        // Summary:
        //     Use linear interpolation for minification; use point sampling for magnification
        //     and mip-level sampling.
        MinLinearMagMipPoint = 16,
        //
        // Summary:
        //     Use linear interpolation for minification; use point sampling for magnification;
        //     use linear interpolation for mip-level sampling.
        MinLinearMagPointMipLinear = 17,
        //
        // Summary:
        //     Use linear interpolation for minification and magnification; use point sampling
        //     for mip-level sampling.
        MinMagLinearMipPoint = 20,
        //
        // Summary:
        //     Use linear interpolation for minification, magnification, and mip-level sampling.
        MinMagMipLinear = 21,
        //
        // Summary:
        //     Use anisotropic interpolation for minification, magnification, and mip-level
        //     sampling.
        Anisotropic = 85,
        //
        // Summary:
        //     Use point sampling for minification, magnification, and mip-level sampling. Compare
        //     the result to the comparison value.
        ComparisonMinMagMipPoint = 128,
        //
        // Summary:
        //     Use point sampling for minification and magnification; use linear interpolation
        //     for mip-level sampling. Compare the result to the comparison value.
        ComparisonMinMagPointMipLinear = 129,
        //
        // Summary:
        //     Use point sampling for minification; use linear interpolation for magnification;
        //     use point sampling for mip-level sampling. Compare the result to the comparison
        //     value.
        ComparisonMinPointMagLinearMipPoint = 132,
        //
        // Summary:
        //     Use point sampling for minification; use linear interpolation for magnification
        //     and mip-level sampling. Compare the result to the comparison value.
        ComparisonMinPointMagMipLinear = 133,
        //
        // Summary:
        //     Use linear interpolation for minification; use point sampling for magnification
        //     and mip-level sampling. Compare the result to the comparison value.
        ComparisonMinLinearMagMipPoint = 144,
        //
        // Summary:
        //     Use linear interpolation for minification; use point sampling for magnification;
        //     use linear interpolation for mip-level sampling. Compare the result to the comparison
        //     value.
        ComparisonMinLinearMagPointMipLinear = 145,
        //
        // Summary:
        //     Use linear interpolation for minification and magnification; use point sampling
        //     for mip-level sampling. Compare the result to the comparison value.
        ComparisonMinMagLinearMipPoint = 148,
        //
        // Summary:
        //     Use linear interpolation for minification, magnification, and mip-level sampling.
        //     Compare the result to the comparison value.
        ComparisonMinMagMipLinear = 149,
        //
        // Summary:
        //     Use anisotropic interpolation for minification, magnification, and mip-level
        //     sampling. Compare the result to the comparison value.
        ComparisonAnisotropic = 213,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_MIP_POINT and instead of
        //     filtering them return the minimum of the texels. Texels that are weighted 0 during
        //     filtering aren't counted towards the minimum. You can query support for this
        //     filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinMagMipPoint = 256,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR and instead
        //     of filtering them return the minimum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the minimum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinMagPointMipLinear = 257,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT and
        //     instead of filtering them return the minimum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the minimum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinPointMagLinearMipPoint = 260,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR and instead
        //     of filtering them return the minimum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the minimum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinPointMagMipLinear = 261,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT and instead
        //     of filtering them return the minimum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the minimum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinLinearMagMipPoint = 272,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
        //     and instead of filtering them return the minimum of the texels. Texels that are
        //     weighted 0 during filtering aren't counted towards the minimum. You can query
        //     support for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinLinearMagPointMipLinear = 273,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT and instead
        //     of filtering them return the minimum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the minimum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinMagLinearMipPoint = 276,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_MIP_LINEAR and instead of
        //     filtering them return the minimum of the texels. Texels that are weighted 0 during
        //     filtering aren't counted towards the minimum. You can query support for this
        //     filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumMinMagMipLinear = 277,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_ANISOTROPIC and instead of filtering
        //     them return the minimum of the texels. Texels that are weighted 0 during filtering
        //     aren't counted towards the minimum. You can query support for this filter type
        //     from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MinimumAnisotropic = 341,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_MIP_POINT and instead of
        //     filtering them return the maximum of the texels. Texels that are weighted 0 during
        //     filtering aren't counted towards the maximum. You can query support for this
        //     filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinMagMipPoint = 384,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR and instead
        //     of filtering them return the maximum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the maximum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinMagPointMipLinear = 385,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT and
        //     instead of filtering them return the maximum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the maximum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinPointMagLinearMipPoint = 388,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR and instead
        //     of filtering them return the maximum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the maximum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinPointMagMipLinear = 389,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT and instead
        //     of filtering them return the maximum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the maximum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinLinearMagMipPoint = 400,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR
        //     and instead of filtering them return the maximum of the texels. Texels that are
        //     weighted 0 during filtering aren't counted towards the maximum. You can query
        //     support for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinLinearMagPointMipLinear = 401,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT and instead
        //     of filtering them return the maximum of the texels. Texels that are weighted
        //     0 during filtering aren't counted towards the maximum. You can query support
        //     for this filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinMagLinearMipPoint = 404,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_MIN_MAG_MIP_LINEAR and instead of
        //     filtering them return the maximum of the texels. Texels that are weighted 0 during
        //     filtering aren't counted towards the maximum. You can query support for this
        //     filter type from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumMinMagMipLinear = 405,
        //
        // Summary:
        //     Fetch the same set of texels as D3D11_FILTER_ANISOTROPIC and instead of filtering
        //     them return the maximum of the texels. Texels that are weighted 0 during filtering
        //     aren't counted towards the maximum. You can query support for this filter type
        //     from the MinMaxFiltering member in the SharpDX.Direct3D11.FeatureDataD3D11Options1
        //     structure.
        MaximumAnisotropic = 469
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
        const Filter filter = Filter::MinMagMipLinear,
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
