#pragma once

#undef max

#include <cassert>
#include <limits>

#include "Math/LinearColor.h"
#include "Rendering/Sampler.h"
#include "Logger.h"

static const unsigned int allMips = std::numeric_limits<unsigned int>::max();

static const LinearColor colorLoad = LinearColor(std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f);
static const float depthLoad = std::numeric_limits<float>::infinity();
static const unsigned int stencilLoad = std::numeric_limits<unsigned int>::infinity();
static const unsigned char maxRenderTargetCount = 8;

enum class PrimitiveType
{
    // Topology that defines a triangle N with 3 vertex extremities: 3*N+0, 3*N+1, 3*N+2.
    TriangleList,

    // Topology that defines a triangle N with 3 vertex extremities: N+0, N+1, N+2.
    TriangleStrip,

    // Topology that defines a line with 2 vertex extremities: 2*N+0, 2*N+1.
    LineList,

    // Topology that defines a quad N with 4 vertex extremities: 4*N+0, 4*N+1, 4*N+2, 4*N+3.
    // Supported only if GRHISupportsQuadTopology == true.
    QuadList,

    // Topology that defines a point N with a single vertex N.
    PointList,

    // Topology that defines a screen aligned rectangle N with only 3 vertex corners:
    //    3*N + 0 is upper-left corner,
    //    3*N + 1 is upper-right corner,
    //    3*N + 2 is the lower-left corner.
    // Supported only if GRHISupportsRectTopology == true.
    RectList,

    Num,
    NumBits = 3
};

class D3D11Utility
{
public:
    template <typename T>
    static void Release(T* pointer)
    {
        if (pointer)
        {
            pointer->Release();

            pointer = nullptr;
        }
    }

    static unsigned int GetVertexCountForPrimitiveCount(unsigned int primitiveCount, PrimitiveType primitiveType)
    {
        unsigned int factor = (primitiveType == PrimitiveType::TriangleList) ? 3 : (primitiveType == PrimitiveType::LineList) ? 2 : (primitiveType == PrimitiveType::RectList) ? 3 : 1;
        unsigned int offset = (primitiveType == PrimitiveType::TriangleStrip) ? 2 : 0;

        return primitiveCount * factor + offset;
    }

    static D3D11_PRIMITIVE_TOPOLOGY GetD3D11PrimitiveType(PrimitiveType primitiveType)
    {
        switch (primitiveType)
        {
            case PrimitiveType::TriangleList:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
            case PrimitiveType::TriangleStrip:
                return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
            case PrimitiveType::LineList:
                return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
            case PrimitiveType::PointList:
                return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
            default:
                Logger::GetInstance().Log(Logger::Level::Error, "Unknown primitive type: {}", static_cast<unsigned int>(primitiveType));
        };

        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
};
