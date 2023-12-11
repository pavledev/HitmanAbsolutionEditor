#include "Rendering/ConstantBuffer.h"
#include "Rendering/DirectXRenderer.h"
#include "Utility/D3D11Utility.h"
#include "Editor.h"

ConstantBuffer::~ConstantBuffer()
{
    D3D11Utility::Release(buffer);
}

unsigned int ConstantBuffer::GetStride() const
{
    return stride;
}
unsigned int ConstantBuffer::GetOffset() const
{
    return offset;
}

unsigned int ConstantBuffer::GetStrideCount() const
{
    return elementCount;
}

ID3D11Buffer* ConstantBuffer::GetBuffer() const
{
    return buffer;
}

void ConstantBuffer::Update(void* dataCpu)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    HRESULT result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to map constant buffer!");

        return;
    }

    memcpy(reinterpret_cast<unsigned char*>(mappedResource.pData), reinterpret_cast<unsigned char*>(dataCpu), stride);

    Editor::GetInstance().GetDirectXRenderer()->GetD3D11DeviceContext()->Unmap(buffer, 0);
}

void ConstantBuffer::ResetOffset()
{
    offset = 0;
    hasUpdated = false;
}

void ConstantBuffer::CreateResource()
{
    // Destroy previous buffer
    D3D11Utility::Release(buffer);

    D3D11_BUFFER_DESC bufferDesc = {};

    bufferDesc.ByteWidth = stride;
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.StructureByteStride = 0;

    HRESULT result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreateBuffer(&bufferDesc, nullptr, &buffer);

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create constant buffer!");
    }
}
