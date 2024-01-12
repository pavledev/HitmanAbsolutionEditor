#pragma once

enum class ConstantBufferRendererBindings
{
    Camera,
    Mesh,
    Light
};

enum class ShaderResourceViewRendererBindings
{
    DiffuseTexture,
    NormalTexture,
    SpecularTexture,
    Emissive,
    Alpha,
    Texture
};

enum class UnorderedAccessViewRendererBindings
{
    Texture
};

enum class SamplerRendererBindings
{
    Surface,
    BilinearClamp
};
