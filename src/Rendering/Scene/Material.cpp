#include "Rendering/Scene/Material.h"

Material::Material(const std::string& name) : name(name)
{
}

Material::Material(const std::string& name, const std::vector<RenderMaterialInstance::Texture>& textures) : name(name)
{
	for (size_t i = 0; i < textures.size(); ++i)
	{
		this->textures.insert(std::make_pair(static_cast<TextureType>(textures[i].type), std::make_shared<RenderTexture>(textures[i])));
	}
}

RenderTexture* Material::GetTexture(const TextureType textureType)
{
	return textures[textureType].get();
}

void Material::SetName(const std::string& name)
{
	this->name = name;
}

void Material::AddTexture(const RenderMaterialInstance::Texture& texture)
{
	this->textures.insert(std::make_pair(static_cast<TextureType>(texture.type), std::make_shared<RenderTexture>(texture)));
}

const bool Material::HasDiffuseTexture() const
{
	return textures.contains(TextureType::Diffuse);
}

const bool Material::HasNormalTexture() const
{
	return textures.contains(TextureType::Normal);
}

const bool Material::HasSpecularTexture() const
{
	return textures.contains(TextureType::Specular);
}
