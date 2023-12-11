#include "Rendering/Viewport.h"

Viewport::Viewport(const Vector2& size, const Vector2& positon) : size(size), position(position)
{
    d3dViewport.TopLeftX = 0;
    d3dViewport.TopLeftY = 0;
    d3dViewport.Width = size.x;
    d3dViewport.Height = size.y;
    d3dViewport.MinDepth = 0.f;
    d3dViewport.MaxDepth = 1.f;
}

D3D11_VIEWPORT& Viewport::GetD3DViewport()
{
    return d3dViewport;
}

const Vector2& Viewport::GetSize() const
{
    return size;
}

const Vector2& Viewport::GetPosition() const
{
    return position;
}

void Viewport::SetSize(const Vector2& size)
{
    this->size = size;
    d3dViewport.Width = size.x;
    d3dViewport.Height = size.y;
}

void Viewport::SetPosition(const Vector2& position)
{
    this->position = position;
}
