#pragma once

enum class ConstantBufferRendererBindings
{
    Uber,
    Light
};

enum class ShaderResourceViewRendererBindings
{
    DiffuseTexture,
    NormalTexture,
    SpecularTexture,
    Texture
};

enum class UnorderedAccessViewRendererBindings
{
    Texture
};

enum class SamplerRendererBindings
{
    BilinearClamp,
    AnisotropicWrap,
    PointWrap
};
