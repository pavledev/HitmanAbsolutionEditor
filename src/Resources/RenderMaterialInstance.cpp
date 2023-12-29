#include "Resources/RenderMaterialInstance.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Logger.h"
#include "Resources/Texture.h"

void RenderMaterialInstance::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(GetResourceData(), GetResourceDataSize());
	const unsigned int materialInfoOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(materialInfoOffset, SeekOrigin::Begin);

	materialPropertyList = binaryReader.Read<SRMaterialPropertyList>();

	ReadProperty(instanceProperty, binaryReader, materialPropertyList.lPropertyList);

	isResourceDeserialized = true;
}

void RenderMaterialInstance::ReadProperty(Property& property, BinaryReader& binaryReader, const unsigned int propertyOffset)
{
	binaryReader.Seek(propertyOffset, SeekOrigin::Begin);

	property.propertyInfo = binaryReader.Read<SProperty>();

	const PROPERTY_TYPE propertyType = static_cast<PROPERTY_TYPE>(property.propertyInfo.lType);

	switch (propertyType)
	{
		case PROPERTY_TYPE::PT_FLOAT:
		{
			if (property.propertyInfo.lSize == 1)
			{
				property.floatValue = *reinterpret_cast<float*>(&property.propertyInfo.lData);
			}
			else
			{
				binaryReader.Seek(property.propertyInfo.lData, SeekOrigin::Begin);

				property.floatValue = binaryReader.Read<float>();
			}

			break;
		}
		case PROPERTY_TYPE::PT_CHAR:
		{
			binaryReader.Seek(property.propertyInfo.lData, SeekOrigin::Begin);

			property.stringValue = binaryReader.ReadString();

			break;
		}
		case PROPERTY_TYPE::PT_UINT32:
		{
			if (property.propertyInfo.lSize == 1)
			{
				property.uint32Value = property.propertyInfo.lData;
			}
			else
			{
				binaryReader.Seek(property.propertyInfo.lData, SeekOrigin::Begin);

				property.uint32Value = binaryReader.Read<unsigned int>();
			}

			break;
		}
		case PROPERTY_TYPE::PT_LIST:
		{
			unsigned int childPropertyOffset = property.propertyInfo.lData;

			property.childProperties.reserve(property.propertyInfo.lSize);

			for (unsigned int i = 0; i < property.propertyInfo.lSize; ++i)
			{
				Property childProperty;

				ReadProperty(childProperty, binaryReader, childPropertyOffset);

				property.childProperties.push_back(childProperty);

				childPropertyOffset += sizeof(SProperty);
			}

			break;
		}
	}
}

RenderMaterialInstance::Property& RenderMaterialInstance::GetInstanceProperty()
{
	return instanceProperty;
}

void RenderMaterialInstance::GetTextures(std::shared_ptr<Resource> matiResource, std::vector<RenderMaterialInstance::Texture>& textures)
{
	bool foundNormalTexture = false;
	bool foundDiffuseTexture = false;
	bool foundSpecularTexture = false;

	GetTextures(instanceProperty, matiResource, textures, foundNormalTexture, foundDiffuseTexture, foundSpecularTexture);

	std::vector<std::shared_ptr<Resource>>& matiReferences = matiResource->GetReferences();
	Texture texture{};

	if (!foundNormalTexture || !foundDiffuseTexture || !foundSpecularTexture)
	{
		for (size_t i = 0; i < matiReferences.size(); ++i)
		{
			std::shared_ptr<Resource> reference = matiReferences[i];
			const ResourceInfoRegistry::ResourceInfo& referenceInfo = ResourceInfoRegistry::GetInstance().GetResourceInfo(reference->GetHash());

			if (referenceInfo.type == "TEXT")
			{
				if (!foundNormalTexture && referenceInfo.resourceID.contains("/normal"))
				{
					texture.type = Texture::Type::Normal;
					texture.textureReferenceIndex = i;
					foundNormalTexture = true;
				}
				else if (!foundDiffuseTexture && referenceInfo.resourceID.contains("/diffuse"))
				{
					texture.type = Texture::Type::Diffuse;
					texture.textureReferenceIndex = i;
					foundDiffuseTexture = true;
				}
				else if (!foundDiffuseTexture && referenceInfo.resourceID.contains("/specular"))
				{
					texture.type = Texture::Type::Specular;
					texture.textureReferenceIndex = i;
					foundSpecularTexture = true;
				}
			}
		}

		if (texture.type != Texture::Type::Unknown)
		{
			textures.push_back(texture);
		}
	}

	if (!foundNormalTexture)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Normal texture not found!");
	}

	if (!foundDiffuseTexture)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Diffuse texture not found!");
	}

	if (!foundSpecularTexture)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Specular texture not found!");
	}
}

void RenderMaterialInstance::GetTextures(const Property& property, std::shared_ptr<Resource> matiResource, std::vector<Texture>& textures, bool& foundNormalTexture, bool& foundDiffuseTexture, bool& foundSpecularTexture)
{
	bool isTextureProperty = false;

	if (property.propertyInfo.lName == 'TEXT')
	{
		isTextureProperty = true;
	}

	Texture texture{};

	for (size_t i = 0; i < property.childProperties.size(); ++i)
	{
		if (isTextureProperty)
		{
			if (property.childProperties[i].propertyInfo.lName == 'NAME' &&
				property.childProperties[i].stringValue == "mapTextureNormal_01")
			{
				texture.type = Texture::Type::Normal;
				foundNormalTexture = true;
			}
			else if (property.childProperties[i].propertyInfo.lName == 'NAME' &&
				property.childProperties[i].stringValue == "mapDiffuse_01")
			{
				texture.type = Texture::Type::Diffuse;
				foundDiffuseTexture = true;
			}
			else if (property.childProperties[i].propertyInfo.lName == 'NAME' &&
				property.childProperties[i].stringValue == "mapSpecular_01")
			{
				texture.type = Texture::Type::Specular;
				foundSpecularTexture = true;
			}

			if (property.childProperties[i].propertyInfo.lName == 'TXID')
			{
				if (property.childProperties[i].uint32Value != -1)
				{
					texture.textureReferenceIndex = property.childProperties[i].uint32Value;
				}
				else
				{
					if (texture.type == Texture::Type::Normal)
					{
						foundNormalTexture = false;
					}
					else if (texture.type == Texture::Type::Diffuse)
					{
						foundDiffuseTexture = false;
					}
				}
			}
		}

		GetTextures(property.childProperties[i], matiResource, textures, foundNormalTexture, foundDiffuseTexture, foundSpecularTexture);
	}

	if (texture.type != Texture::Type::Unknown && texture.textureReferenceIndex != -1)
	{
		textures.push_back(texture);
	}
}
