#include <cassert>

#include "Rendering/VertexBuffer.h"
#include "Utility/D3D11Utility.h"
#include "Rendering/DirectXRenderer.h"
#include "Editor.h"

VertexBuffer::~VertexBuffer()
{
	D3D11Utility::Release(buffer);
}

void VertexBuffer::Create(const void* vertices)
{
    ID3D11Device* device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    const bool isDynamic = vertices == nullptr;

    // Destroy previous buffer
    D3D11Utility::Release(buffer);

    // fill in a buffer description.
    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = static_cast<UINT>(gpuSize);
    bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
    bufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    // fill in the subresource data.
    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    const HRESULT result = device->CreateBuffer(&bufferDesc, isDynamic ? nullptr : &initData, &buffer);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create vertex buffer!");
    }
}

void VertexBuffer::Create(const void* vertices, const unsigned int vertexCount, const unsigned int stride)
{
    this->stride = stride;
    this->vertexCount = vertexCount;
    gpuSize = static_cast<unsigned long long>(stride * vertexCount);

    Create(vertices);
}

void* VertexBuffer::Map()
{
    assert(buffer != nullptr);

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // Disable GPU access to the vertex buffer data.
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    const HRESULT result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to map vertex buffer!");

        return nullptr;
    }

    return mappedResource.pData;
}

void VertexBuffer::Unmap()
{
    assert(buffer != nullptr);

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    // Re-enable GPU access to the vertex buffer data.
    deviceContext->Unmap(buffer, 0);
}

ID3D11Buffer* VertexBuffer::GetBuffer() const
{
    return buffer;
}

unsigned int VertexBuffer::GetStride() const
{
    return stride;
}

unsigned int VertexBuffer::GetVertexCount() const
{
    return vertexCount;
}
