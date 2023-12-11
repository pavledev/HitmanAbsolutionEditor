#pragma once

#include <d3d11.h>

class RasterizerState
{
public:
    enum class FillMode
    {
        Point,
        Wireframe,
        Solid
    };

    enum class CullMode
    {
        None,
        Clockwise,
        CounterClockwise,
    };

    enum class DepthClipMode : unsigned char
    {
        DepthClip,
        DepthClamp
    };

    RasterizerState() = default;
    RasterizerState(
        const FillMode fillMode = FillMode::Solid,
        const CullMode cullMode = CullMode::None,
        const bool enableLineAA = false,
        const bool enableMSAA = false,
        const RasterizerState::DepthClipMode depthClipMode = RasterizerState::DepthClipMode::DepthClip,
        const float depthBias = 0.0f,
        const float depthBiasClamp = 0.0f,
        const float depthBiasSlopeScaled = 0.0f
    );
    ~RasterizerState();
    ID3D11RasterizerState* GetRasterizerState() const;
    static D3D11_FILL_MODE ConvertFillMode(const FillMode fillMode);
    static D3D11_CULL_MODE ConvertCullMode(const CullMode cullMode);

private:
    void CreateResource();

    D3D11_FILL_MODE fillMode;
    D3D11_CULL_MODE cullMode;
    bool depthClipEnabled;
    bool scissorEnabled;
    bool enableLineAA;
    bool enableMSAA;
    float depthBias;
    float depthBiasClamp;
    float depthBiasSlopeScaled;
    ID3D11RasterizerState* rasterizerState;
};
