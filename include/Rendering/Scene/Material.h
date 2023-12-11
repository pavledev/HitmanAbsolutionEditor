#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "../RenderTexture.h"

class Material
{
public:
	enum class TextureType
	{
		Diffuse = 1,
		Normal,
		Specular
	};

	Material() = default;
	Material(const std::string& name);
	Material(const std::string& name, const std::vector<RenderMaterialInstance::Texture>& textures);
	RenderTexture* GetTexture(const TextureType textureType);
	void SetName(const std::string& name);
	void AddTexture(const RenderMaterialInstance::Texture& texture);
	const bool HasDiffuseTexture() const;
	const bool HasNormalTexture() const;
	const bool HasSpecularTexture() const;

private:
	std::string name;
	std::unordered_map<TextureType, std::shared_ptr<RenderTexture>> textures;
};
