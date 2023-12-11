#include "Rendering/InputLayout.h"
#include "Rendering/DirectXRenderer.h"
#include "Logger.h"
#include "Utility/D3D11Utility.h"
#include "Editor.h"

InputLayout::~InputLayout()
{
    D3D11Utility::Release(inputLayout);
}

VertexType InputLayout::GetVertexType() const
{
	return vertexType;
}

const unsigned int InputLayout::GetVertexSize() const
{
	return vertexSize;
}

const std::vector<VertexAttribute>& InputLayout::GetAttributeDescriptions() const
{
	return vertexAttributes;
}

unsigned int InputLayout::GetAttributeCount() const
{
	return static_cast<unsigned int>(vertexAttributes.size());
}

ID3D11InputLayout* InputLayout::GetInputLayout() const
{
    return inputLayout;
}

bool InputLayout::operator==(const InputLayout& rhs) const
{
	return vertexType == rhs.GetVertexType();
}

void InputLayout::Create(const VertexType vertexType, void* vertexShaderBlob)
{
    const unsigned int binding = 0;

    switch (vertexType)
    {
        case VertexType::Pos:
        {
            vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPosition, position) }
            };

            vertexSize = sizeof(VertexPosition);
            break;
        }
        case VertexType::PosCol:
        {
            vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT,    offsetof(VertexPositionColor, position) },
                { "COLOR",    1, binding, DXGI_FORMAT_R32G32B32A32_FLOAT, offsetof(VertexPositionColor, color) }
            };

            vertexSize = sizeof(VertexPositionColor);
            break;
        }
        case VertexType::PosUv:
        {
            vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPositionTexture, position) },
                { "TEXCOORD", 1, binding, DXGI_FORMAT_R32G32_FLOAT,    offsetof(VertexPositionTexture, textureCoordinates) }
            };

            vertexSize = sizeof(VertexPositionTexture);
            break;
        }
        case VertexType::PosUvCol:
        {
            vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPositionTextureColor, position) },
                { "TEXCOORD", 1, binding, DXGI_FORMAT_R32G32_FLOAT,    offsetof(VertexPositionTextureColor, textureCoordinates) },
                { "COLOR",    2, binding, DXGI_FORMAT_R32G32B32A32_FLOAT, offsetof(VertexPositionTextureColor, color) }
            };

            vertexSize = sizeof(VertexPositionTextureColor);
            break;
        }
        case VertexType::PosNor:
        {
            vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPositionNormal, position) },
                { "NORMAL", 1, binding, DXGI_FORMAT_R32G32B32_FLOAT,    offsetof(VertexPositionNormal, normal) }
            };

            vertexSize = sizeof(VertexPositionNormal);
            break;
        }
        case VertexType::PosUvNorTan:
        {
            /*vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPositionTextureNormalTangent, position) },
                { "TEXCOORD", 1, binding, DXGI_FORMAT_R32G32_FLOAT,    offsetof(VertexPositionTextureNormalTangent, textureCoordinates) },
                { "NORMAL",   2, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPositionTextureNormalTangent, nor) },
                { "TANGENT",  3, binding, DXGI_FORMAT_R32G32B32_FLOAT, offsetof(VertexPositionTextureNormalTangent, tan) }
            };*/
            /*vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32A32_FLOAT, 0 },
                { "NORMAL",   2, binding, DXGI_FORMAT_R32G32B32A32_FLOAT, 16 },
                { "TANGENT",  3, binding, DXGI_FORMAT_R32G32B32A32_FLOAT, 32 },
                { "BINORMAL",  3, binding, DXGI_FORMAT_R32G32B32A32_FLOAT, 48 },
                { "TEXCOORD", 1, binding, DXGI_FORMAT_R32G32_FLOAT, 64 },
            };*/
            vertexAttributes =
            {
                { "POSITION", 0, binding, DXGI_FORMAT_R32G32B32_FLOAT, 0 },
                { "NORMAL",   2, binding, DXGI_FORMAT_R32G32B32_FLOAT, 12 },
                { "TANGENT",  3, binding, DXGI_FORMAT_R32G32B32_FLOAT, 24 },
                { "BINORMAL",  3, binding, DXGI_FORMAT_R32G32B32_FLOAT, 36 },
                { "TEXCOORD", 1, binding, DXGI_FORMAT_R32G32_FLOAT, 48 },
            };

            //vertexSize = sizeof(VertexPositionTextureNormalTangent);
            //vertexSize = 72;
            vertexSize = sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector3) + sizeof(Vector2);
            break;
        }
        default:
            vertexSize = 0;
            break;
    }

    if (vertexShaderBlob && !vertexAttributes.empty())
    {
        if (!CreateResource(vertexShaderBlob))
        {
            Logger::GetInstance().Log(Logger::Level::Error, "Failed to create input layout!");
        }
    }
}

bool InputLayout::CreateResource(void* vertexShaderBlob)
{
    assert(vertexShaderBlob != nullptr);
    assert(!this->vertexAttributes.empty());

    std::vector<D3D11_INPUT_ELEMENT_DESC> vertexAttributes;

    for (const auto& vertexAttribute : this->vertexAttributes)
    {
        vertexAttributes.emplace_back(D3D11_INPUT_ELEMENT_DESC
            {
                vertexAttribute.name.c_str(),                              // SemanticName
                0,                                                          // SemanticIndex
                vertexAttribute.format,                                     // Format
                0,                                                          // InputSlot
                vertexAttribute.offset,                                    // AlignedByteOffset
                D3D11_INPUT_PER_VERTEX_DATA,                                // InputSlotClass
                0                                                           // InstanceDataStepRate
            });
    }

    auto d3d_blob = static_cast<ID3D10Blob*>(vertexShaderBlob);
    const auto result = Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device()->CreateInputLayout
    (
        vertexAttributes.data(),
        static_cast<UINT>(vertexAttributes.size()),
        d3d_blob->GetBufferPointer(),
        d3d_blob->GetBufferSize(),
        &inputLayout
    );

    if (FAILED(result))
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to create input layout!");
        return false;
    }

    return true;
}
