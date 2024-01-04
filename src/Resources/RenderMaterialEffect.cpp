#include <filesystem>

#include "Resources/RenderMaterialEffect.h"
#include "Editor.h"
#include "HLSL/HLSLDecompiler.h"
#include "Utility/D3D11Utility.h"

RenderMaterialEffect::~RenderMaterialEffect()
{
    D3D11Utility::Release(compiledEffect);
}

void RenderMaterialEffect::Deserialize()
{
    BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
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

    isResourceDeserialized = true;
}

void RenderMaterialEffect::Export(const std::string& outputPath, const std::string& exportOption)
{
    if (exportOption.starts_with("Raw"))
    {
        ExportRawData(outputPath);
    }
    else
    {
        const std::string fileName = outputPath.substr(outputPath.find_last_of("\\") + 1);
        const std::string jsonFilePath = std::format("{}\\{}.json", outputPath, fileName);

        SerializeToJson(jsonFilePath);

        for (size_t i = 0; i < shaders.size(); ++i)
        {
            std::string hlsl;
            std::string model;

            Decompile(shaders[i].byteCode, shaders[i].byteCodeLength, &hlsl, &model);

            std::string hlslFilePath = std::format("{}\\{}_{}.hlsl", outputPath, shaders[i].name, i + 1);
            BinaryWriter binaryWriter = BinaryWriter(hlslFilePath);

            binaryWriter.WriteString(hlsl);
        }
    }
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

void RenderMaterialEffect::SerializeToJson(const std::string& outputFilePath)
{
    rapidjson::StringBuffer stringBuffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

    writer.StartObject();

    writer.String("constantBufferNames");
    writer.StartArray();

    for (size_t i = 0; i < constantBufferNames.size(); ++i)
    {
        writer.String(constantBufferNames[i].c_str());
    }

    writer.EndArray();

    writer.String("globalVariables");
    writer.StartArray();

    for (size_t i = 0; i < globalVariables.size(); ++i)
    {
        writer.String(globalVariables[i].c_str());
    }

    writer.EndArray();

    writer.String("techniques");
    writer.StartArray();

    for (size_t i = 0; i < techniques.size(); ++i)
    {
        writer.StartObject();

        writer.String("name");
        writer.String(techniques[i].name.c_str());

        writer.String("passes");
        writer.StartArray();

        for (size_t j = 0; j < techniques[i].passes.size(); ++j)
        {
            writer.String(techniques[i].passes[j].c_str());
        }

        writer.EndArray();

        writer.String("annotations");
        writer.StartArray();

        for (size_t j = 0; j < techniques[i].annotations.size(); ++j)
        {
            writer.String(techniques[i].annotations[j].c_str());
        }

        writer.EndArray();

        writer.EndObject();
    }

    writer.EndArray();

    std::ofstream ofstream = std::ofstream(outputFilePath);

    ofstream << stringBuffer.GetString();

    ofstream.close();
}
