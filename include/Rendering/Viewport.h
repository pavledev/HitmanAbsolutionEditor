#pragma once

#include <d3d11.h>

#include "Math/Vector2.h"

class Viewport
{
public:
	Viewport() = default;
	Viewport(const Vector2& size, const Vector2& positon);
	D3D11_VIEWPORT& GetD3DViewport();
	const Vector2& GetSize() const;
	const Vector2& GetPosition() const;
	void SetSize(const Vector2& size);
	void SetPosition(const Vector2& position);

private:
	D3D11_VIEWPORT d3dViewport;
	Vector2 size;
	Vector2 position;
};
