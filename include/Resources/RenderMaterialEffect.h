#pragma once

#include <Effects11/d3dx11effect.h>

#include "Resource.h"

class RenderMaterialEffect : public Resource
{
public:
	struct Technique
	{
		std::string name;
		std::vector<std::string> passes;
		std::vector<std::string> annotations;
	};

	struct Shader
	{
		std::string name;
		const unsigned char* byteCode;
		unsigned int byteCodeLength;
		std::string hlslCode;
	};

	~RenderMaterialEffect() override;
	void Deserialize();
	void GetConstantBufferNames(ID3DX11Effect* compiledEffect, D3DX11_EFFECT_DESC& effectDesc);
	void GetGlobalVariableNames(ID3DX11Effect* compiledEffect, D3DX11_EFFECT_DESC& effectDesc);
	void GetTechniques(ID3DX11Effect* compiledEffect, D3DX11_EFFECT_DESC& effectDesc);
	void GetShaders(ID3DX11EffectPass* effectPass, const std::string& techniqueName);
	std::vector<std::string>& GetConstantBufferNames();
	std::vector<std::string>& GetGlobalVariables();
	std::vector<Technique>& GetTechniques();
	std::vector<Shader>& GetShaders();
	void AddShader(const D3DX11_PASS_SHADER_DESC& passShaderDesc, const std::string& techniqueName, const std::string& shaderType);

private:
	ID3DX11Effect* compiledEffect;
	std::vector<std::string> constantBufferNames;
	std::vector<std::string> globalVariables;
	std::vector<Technique> techniques;
	std::vector<Shader> shaders;
};
