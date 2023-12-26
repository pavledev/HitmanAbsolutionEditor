#include "Resources/RenderMaterialEffect.h"
#include "Editor.h"

RenderMaterialEffect::~RenderMaterialEffect()
{
    compiledEffect->Release();
}

void RenderMaterialEffect::Deserialize()
{
    BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
    unsigned int effectDataOffset = binaryReader.Read<unsigned int>();
    unsigned int effectDataSize = binaryReader.Read<unsigned int>();

    binaryReader.Seek(effectDataOffset);

    void* effectData = binaryReader.Read<void>(effectDataSize);

    D3DX11CreateEffectFromMemory(effectData, effectDataSize, 0, Editor::GetInstance().GetDirectXRenderer()->GetD3D11Device(), &compiledEffect);

    delete[] effectData;

    D3DX11_EFFECT_DESC effectDesc;

    compiledEffect->GetDesc(&effectDesc);

    GetConstantBufferNames(compiledEffect, effectDesc);
    GetGlobalVariableNames(compiledEffect, effectDesc);
    GetTechniques(compiledEffect, effectDesc);
}

void RenderMaterialEffect::GetConstantBufferNames(ID3DX11Effect* compiledEffect, D3DX11_EFFECT_DESC& effectDesc)
{
    constantBufferNames.reserve(effectDesc.ConstantBuffers);

    for (unsigned int i = 0; i < effectDesc.ConstantBuffers; ++i)
    {
        ID3DX11EffectConstantBuffer* constantBuffer = compiledEffect->GetConstantBufferByIndex(i);
        D3DX11_EFFECT_VARIABLE_DESC variableDesc;

        constantBuffer->GetDesc(&variableDesc);

        constantBufferNames.push_back(variableDesc.Name);

        constantBuffer->Release();
    }
}

void RenderMaterialEffect::GetGlobalVariableNames(ID3DX11Effect* compiledEffect, D3DX11_EFFECT_DESC& effectDesc)
{
    globalVariables.reserve(effectDesc.GlobalVariables);

    for (unsigned int i = 0; i < effectDesc.GlobalVariables; ++i)
    {
        ID3DX11EffectVariable* effectVariable = compiledEffect->GetVariableByIndex(i);
        D3DX11_EFFECT_VARIABLE_DESC variableDesc;

        effectVariable->GetDesc(&variableDesc);

        ID3DX11EffectType* effectVariableType = effectVariable->GetType();
        D3DX11_EFFECT_TYPE_DESC effectTypeDesc;

        effectVariableType->GetDesc(&effectTypeDesc);

        std::string variable = std::format("{} {}", effectTypeDesc.TypeName, variableDesc.Name);

        globalVariables.push_back(variable);

        effectVariableType->Release();
        effectVariable->Release();
    }
}

void RenderMaterialEffect::GetTechniques(ID3DX11Effect* compiledEffect, D3DX11_EFFECT_DESC& effectDesc)
{
    techniques.reserve(effectDesc.Techniques);

    for (unsigned int i = 0; i < effectDesc.Techniques; ++i)
    {
        Technique technique;
        ID3DX11EffectTechnique* effectTechnique = compiledEffect->GetTechniqueByIndex(i);
        D3DX11_TECHNIQUE_DESC techniqueDesc;

        effectTechnique->GetDesc(&techniqueDesc);

        technique.name = techniqueDesc.Name;

        technique.passes.reserve(techniqueDesc.Passes);

        for (unsigned int j = 0; j < techniqueDesc.Passes; ++j)
        {
            ID3DX11EffectPass* effectPass = effectTechnique->GetPassByIndex(j);
            D3DX11_PASS_DESC passDesc;

            effectPass->GetDesc(&passDesc);

            technique.passes.push_back(passDesc.Name);

            GetShaders(effectPass, technique.name);

            effectPass->Release();
        }

        for (unsigned int j = 0; j < techniqueDesc.Annotations; ++j)
        {
            ID3DX11EffectVariable* effectVariable = effectTechnique->GetAnnotationByIndex(j);
            D3DX11_EFFECT_VARIABLE_DESC variableDesc;

            effectVariable->GetDesc(&variableDesc);

            technique.annotations.push_back(variableDesc.Name);

            effectVariable->Release();
        }

        techniques.push_back(technique);

        effectTechnique->Release();
    }
}

void RenderMaterialEffect::GetShaders(ID3DX11EffectPass* effectPass, const std::string& techniqueName)
{
    D3DX11_PASS_SHADER_DESC passShaderDesc;

    effectPass->GetVertexShaderDesc(&passShaderDesc);
    AddShader(passShaderDesc, techniqueName, "VS");

    effectPass->GetPixelShaderDesc(&passShaderDesc);
    AddShader(passShaderDesc, techniqueName, "PS");

    effectPass->GetComputeShaderDesc(&passShaderDesc);
    AddShader(passShaderDesc, techniqueName, "CS");

    effectPass->GetGeometryShaderDesc(&passShaderDesc);
    AddShader(passShaderDesc, techniqueName, "GS");

    effectPass->GetDomainShaderDesc(&passShaderDesc);
    AddShader(passShaderDesc, techniqueName, "DS");

    effectPass->GetHullShaderDesc(&passShaderDesc);
    AddShader(passShaderDesc, techniqueName, "HS");
}

std::vector<std::string>& RenderMaterialEffect::GetConstantBufferNames()
{
    return constantBufferNames;
}

std::vector<std::string>& RenderMaterialEffect::GetGlobalVariables()
{
    return globalVariables;
}

std::vector<RenderMaterialEffect::Technique>& RenderMaterialEffect::GetTechniques()
{
    return techniques;
}

std::vector<RenderMaterialEffect::Shader>& RenderMaterialEffect::GetShaders()
{
    return shaders;
}

void RenderMaterialEffect::AddShader(const D3DX11_PASS_SHADER_DESC& passShaderDesc, const std::string& techniqueName, const std::string& shaderType)
{
    if (passShaderDesc.pShaderVariable && passShaderDesc.pShaderVariable->IsValid())
    {
        D3DX11_EFFECT_SHADER_DESC effectShaderDesc;

        passShaderDesc.pShaderVariable->GetShaderDesc(passShaderDesc.ShaderIndex, &effectShaderDesc);

        Shader shader;

        shader.name = std::format("{}_{}", techniqueName, shaderType);
        shader.byteCode = effectShaderDesc.pBytecode;
        shader.byteCodeLength = effectShaderDesc.BytecodeLength;

        shaders.push_back(shader);
    }
}
