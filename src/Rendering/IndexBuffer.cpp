#include <cassert>

#include "Rendering/IndexBuffer.h"
#include "Utility/D3D11Utility.h"
#include "Rendering/DirectXRenderer.h"
#include "Editor.h"

IndexBuffer::~IndexBuffer()
{
    D3D11Utility::Release(buffer);
}

void IndexBuffer::Create(const void* indices)
{
    ID3D11Device* device = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device();
    const bool isDynamic = indices == nullptr;

    // Destroy previous buffer
    D3D11Utility::Release(buffer);

    D3D11_BUFFER_DESC bufferDesc = {};
    bufferDesc.ByteWidth = stride * indexCount;
    bufferDesc.Usage = isDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_IMMUTABLE;
    bufferDesc.CPUAccessFlags = isDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = indices;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    const HRESULT result = device->CreateBuffer(&bufferDesc, isDynamic ? nullptr : &initData, &buffer);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create vertex buffer!");
    }
}

void* IndexBuffer::Map()
{
    assert(buffer != nullptr);

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    const HRESULT result = deviceContext->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to map index buffer!");

        return nullptr;
    }

    return mappedResource.pData;
}

void IndexBuffer::Unmap()
{
    assert(buffer != nullptr);

    ID3D11DeviceContext* deviceContext = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext();

    deviceContext->Unmap(buffer, 0);
}

ID3D11Buffer* IndexBuffer::GetBuffer() const
{
    return buffer;
}

unsigned int IndexBuffer::GetStride() const
{
    return stride;
}

unsigned int IndexBuffer::GetIndexCount() const
{
    return indexCount;
}

bool IndexBuffer::Is16Bit() const
{
    return sizeof(unsigned short) == stride;
}

bool IndexBuffer::Is32Bit() const
{
    return sizeof(unsigned int) == stride;
}
