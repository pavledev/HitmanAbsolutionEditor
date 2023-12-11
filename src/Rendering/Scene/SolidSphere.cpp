#include "Rendering/Scene/SolidSphere.h"

SolidSphere::SolidSphere(float radius)
{
	CreateTesselated(12, 24);
	//Transform(DirectX::XMMatrixScaling(radius, radius, radius));

	CreateGpuBuffers();
	CreateShaders();
}

const VertexBuffer* SolidSphere::GetVertexBuffer() const
{
	return vertexBuffer.get();
}

const IndexBuffer* SolidSphere::GetIndexBuffer() const
{
	return indexBuffer.get();
}

Shader* SolidSphere::GetVertexShader() const
{
	return vertexShader.get();
}

Shader* SolidSphere::GetPixelShader() const
{
	return pixelShader.get();
}

void SolidSphere::CreateTesselated(const int latDiv, const int longDiv)
{
	assert(latDiv >= 3);
	assert(longDiv >= 3);

	constexpr float radius = 1.0f;
	const auto base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	const float lattitudeAngle = DirectX::XM_PI / latDiv;
	const float longitudeAngle = 2.0f * DirectX::XM_PI / longDiv;

	for (int iLat = 1; iLat < latDiv; iLat++)
	{
		const auto latBase = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationX(lattitudeAngle * iLat));

		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			DirectX::XMFLOAT3 calculatedPos;
			auto v = DirectX::XMVector3Transform(latBase, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));

			DirectX::XMStoreFloat3(&calculatedPos, v);
			vertexPositions.push_back(calculatedPos);
		}
	}

	// add the cap vertices
	const auto iNorthPole = static_cast<unsigned short>(vertexPositions.size());
	DirectX::XMFLOAT3 northPos;
	DirectX::XMStoreFloat3(&northPos, base);
	vertexPositions.push_back(northPos);

	const auto iSouthPole = static_cast<unsigned short>(vertexPositions.size());
	DirectX::XMFLOAT3 southPos;
	DirectX::XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base));
	vertexPositions.push_back(southPos);

	const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
	{
		return iLat * longDiv + iLong;
	};

	for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
	{
		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			indices.push_back(calcIdx(iLat, iLong));
			indices.push_back(calcIdx(iLat + 1, iLong));
			indices.push_back(calcIdx(iLat, iLong + 1));
			indices.push_back(calcIdx(iLat, iLong + 1));
			indices.push_back(calcIdx(iLat + 1, iLong));
			indices.push_back(calcIdx(iLat + 1, iLong + 1));
		}

		// wrap band
		indices.push_back(calcIdx(iLat, longDiv - 1));
		indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		indices.push_back(calcIdx(iLat, 0));
		indices.push_back(calcIdx(iLat, 0));
		indices.push_back(calcIdx(iLat + 1, longDiv - 1));
		indices.push_back(calcIdx(iLat + 1, 0));
	}

	// cap fans
	for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
	{
		// north
		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, iLong));
		indices.push_back(calcIdx(0, iLong + 1));
		// south
		indices.push_back(calcIdx(latDiv - 2, iLong + 1));
		indices.push_back(calcIdx(latDiv - 2, iLong));
		indices.push_back(iSouthPole);
	}

	// wrap triangles
	// north
	indices.push_back(iNorthPole);
	indices.push_back(calcIdx(0, longDiv - 1));
	indices.push_back(calcIdx(0, 0));
	// south
	indices.push_back(calcIdx(latDiv - 2, 0));
	indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
	indices.push_back(iSouthPole);
}

void SolidSphere::CreateGpuBuffers()
{
	vertexBuffer = std::make_shared<VertexBuffer>();
	vertexBuffer->Create(vertexPositions);

	indexBuffer = std::make_shared<IndexBuffer>();
	indexBuffer->Create(indices);
}

void SolidSphere::CreateShaders()
{
	std::string vertexShaderFilePath = "assets/shaders/Solid_VS.hlsl";
	std::string pixelShaderFilePath = "assets/shaders/Solid_PS.hlsl";

	vertexShader = std::make_shared<Shader>();
	vertexShader->Compile(Shader::Type::Vertex, vertexShaderFilePath, VertexType::Pos);

	pixelShader = std::make_shared<Shader>();
	pixelShader->Compile(Shader::Type::Pixel, pixelShaderFilePath);
}

void SolidSphere::Transform(DirectX::FXMMATRIX matrix)
{
	for (size_t i = 0; i < vertexPositions.size(); i++)
	{
		auto& pos = vertexPositions[i];

		DirectX::XMStoreFloat3(&pos, DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix));
	}
}
