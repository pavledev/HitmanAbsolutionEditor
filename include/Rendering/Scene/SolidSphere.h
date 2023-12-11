#pragma once

#include <vector>
#include <DirectXMath.h>

#include "../VertexBuffer.h"
#include "../IndexBuffer.h"
#include "../Shader.h"

class SolidSphere
{
public:
	SolidSphere(float radius);
	const VertexBuffer* GetVertexBuffer() const;
	const IndexBuffer* GetIndexBuffer() const;
	Shader* GetVertexShader() const;
	Shader* GetPixelShader() const;
	void CreateTesselated(const int latDiv, const int longDiv);
	void CreateGpuBuffers();
	void CreateShaders();
	void Transform(DirectX::FXMMATRIX matrix);

private:
	std::vector<DirectX::XMFLOAT3> vertexPositions;
	std::vector<unsigned short> indices;
	std::shared_ptr<VertexBuffer> vertexBuffer;
	std::shared_ptr<IndexBuffer> indexBuffer;
	std::shared_ptr<Shader> vertexShader;
	std::shared_ptr<Shader> pixelShader;
};
