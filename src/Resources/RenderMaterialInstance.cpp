#include "Resources/RenderMaterialInstance.h"
#include "Registry/ResourceInfoRegistry.h"
#include "Logger.h"
#include "Resources/Texture.h"

void RenderMaterialInstance::Property::SerializeToJson(const std::vector<std::shared_ptr<Resource>>& references, rapidjson::Value& propertyObject, rapidjson::Document::AllocatorType& allocator)
{
	const std::string& propertyName = materialPropertyNames[propertyInfo.lName];
	const PROPERTY_TYPE propertyType = static_cast<PROPERTY_TYPE>(propertyInfo.lType);

	switch (propertyType)
	{
		case PROPERTY_TYPE::PT_FLOAT:
		{
			propertyObject.AddMember(rapidjson::Value(propertyName.c_str(), allocator).Move(), floatValue, allocator);
			break;
		}
		case PROPERTY_TYPE::PT_CHAR:
		{
			propertyObject.AddMember(rapidjson::Value(propertyName.c_str(), allocator).Move(), rapidjson::Value(stringValue.c_str(), allocator).Move(), allocator);
			break;
		}
		case PROPERTY_TYPE::PT_UINT32:
		{
			if (propertyInfo.lName == 'TXID')
			{
				std::string textureResourceID;

				if (uint32Value != -1)
				{
					textureResourceID = references[uint32Value]->GetResourceID();
				}

				propertyObject.AddMember(rapidjson::Value(propertyName.c_str(), allocator).Move(), rapidjson::Value(textureResourceID.c_str(), allocator).Move(), allocator);
			}
			else
			{
				propertyObject.AddMember(rapidjson::Value(propertyName.c_str(), allocator).Move(), uint32Value, allocator);
			}

			break;
		}
		case PROPERTY_TYPE::PT_LIST:
		{
			rapidjson::Value childObject;

			childObject.SetObject();

			for (size_t i = 0; i < childProperties.size(); ++i)
			{
				childProperties[i].SerializeToJson(references, childObject, allocator);
			}

			if (propertyObject.HasMember(propertyName.c_str()))
			{
				propertyObject[propertyName.c_str()].GetArray().PushBack(childObject, allocator);
			}
			else
			{
				rapidjson::Value array;

				array.SetArray();
				array.PushBack(childObject, allocator);

				propertyObject.AddMember(rapidjson::Value(propertyName.c_str(), allocator).Move(), array, allocator);
			}

			break;
		}
	}
}

RenderMaterialInstance::RenderMaterialInstance()
{
	if (materialPropertyNames.empty())
	{
		materialPropertyNames.insert(std::make_pair('AREF', "Alpha Reference"));
		materialPropertyNames.insert(std::make_pair('ATST', "Alpha Test Enabled"));
		materialPropertyNames.insert(std::make_pair('BENA', "Blend Enabled"));
		materialPropertyNames.insert(std::make_pair('BIND', "Binder"));
		materialPropertyNames.insert(std::make_pair('BMOD', "Blend Mode"));
		materialPropertyNames.insert(std::make_pair('COLO', "Color"));
		materialPropertyNames.insert(std::make_pair('CULL', "Culling Mode"));
		materialPropertyNames.insert(std::make_pair('DBDE', "Decal Blend Diffuse"));
		materialPropertyNames.insert(std::make_pair('DBEE', "Decal Blend Emission"));
		materialPropertyNames.insert(std::make_pair('DBNE', "Decal Blend Normal"));
		materialPropertyNames.insert(std::make_pair('DBRE', "Decal Blend Roughness"));
		materialPropertyNames.insert(std::make_pair('DBSE', "Decal Blend Specular"));
		materialPropertyNames.insert(std::make_pair('ENAB', "Enabled"));
		materialPropertyNames.insert(std::make_pair('FENA', "Fog Enabled"));
		materialPropertyNames.insert(std::make_pair('FLTV', "Float Value"));
		materialPropertyNames.insert(std::make_pair('INST', "Instance"));
		materialPropertyNames.insert(std::make_pair('NAME', "Name"));
		materialPropertyNames.insert(std::make_pair('OPAC', "Opacity"));
		materialPropertyNames.insert(std::make_pair('RSTA', "Render State"));
		materialPropertyNames.insert(std::make_pair('SSBW', "Subsurface Value"));
		materialPropertyNames.insert(std::make_pair('SSVB', "Subsurface Blue"));
		materialPropertyNames.insert(std::make_pair('SSVG', "Subsurface Green"));
		materialPropertyNames.insert(std::make_pair('SSVR', "Subsurface Red"));
		materialPropertyNames.insert(std::make_pair('TAGS', "Tags"));
		materialPropertyNames.insert(std::make_pair('TEXT', "Texture"));
		materialPropertyNames.insert(std::make_pair('TILU', "Tiling U"));
		materialPropertyNames.insert(std::make_pair('TILV', "Tiling V"));
		materialPropertyNames.insert(std::make_pair('TXID', "Texture Id"));
		materialPropertyNames.insert(std::make_pair('TYPE', "Type"));
		materialPropertyNames.insert(std::make_pair('VALU', "Value"));
		materialPropertyNames.insert(std::make_pair('ZBIA', "Z Bias"));
		materialPropertyNames.insert(std::make_pair('ZOFF', "Z Offset"));
	}
}

void RenderMaterialInstance::Deserialize()
{
	BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);
	const unsigned int materialInfoOffset = binaryReader.Read<unsigned int>();

	binaryReader.Seek(materialInfoOffset, SeekOrigin::Begin);

	materialPropertyList = binaryReader.Read<SRMaterialPropertyList>();

	binaryReader.Seek(materialPropertyList.lMaterialClassType, SeekOrigin::Begin);

	materialClassType = binaryReader.ReadString();

	ReadProperty(instanceProperty, binaryReader, materialPropertyList.lPropertyList);

	isResourceDeserialized = true;
}

void RenderMaterialInstance::Export(const std::string& outputPath, const std::string& exportOption)
{
	if (exportOption.starts_with("Raw"))
	{
		ExportRawData(outputPath);
	}
	else
	{
		SerializeToJson(outputPath);
	}
}

void RenderMaterialInstance::SerializeToJson(const std::string& outputFilePath)
{
	std::vector<std::shared_ptr<Resource>>& references = GetReferences();
	rapidjson::Document document;
	rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

	document.SetObject();

	materialPropertyList.SerializeToJson(materialClassType, references, document);

	rapidjson::Value object;

	object.SetObject();

	instanceProperty.SerializeToJson(references, object, allocator);

	document.AddMember("material", object, allocator);

	rapidjson::StringBuffer stringBuffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(stringBuffer);

	document.Accept(writer);

	std::ofstream outputFileStream = std::ofstream(outputFilePath);

	outputFileStream << stringBuffer.GetString();

	outputFileStream.close();
}

SRMaterialPropertyList& RenderMaterialInstance::GetMaterialPropertyList()
{
	return materialPropertyList;
}

std::string& RenderMaterialInstance::GetMaterialClassType()
{
	return materialClassType;
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
	bool foundEmissiveTexture = false;
	bool foundAlphaTexture = false;

	GetTextures(instanceProperty, matiResource, textures, foundNormalTexture, foundDiffuseTexture, foundSpecularTexture, foundEmissiveTexture);

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
				else if (!foundDiffuseTexture && referenceInfo.resourceID.contains("/emissive"))
				{
					texture.type = Texture::Type::Emissive;
					texture.textureReferenceIndex = i;
					foundEmissiveTexture = true;
				}
				else if (referenceInfo.resourceID.contains("/alpha"))
				{
					texture.type = Texture::Type::Alpha;
					texture.textureReferenceIndex = i;
					foundAlphaTexture = true;
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

	if (!foundEmissiveTexture)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Emissive texture not found!");
	}

	if (!foundAlphaTexture)
	{
		Logger::GetInstance().Log(Logger::Level::Warning, "Alpha texture not found!");
	}
}

void RenderMaterialInstance::GetTextures(const Property& property, std::shared_ptr<Resource> matiResource, std::vector<Texture>& textures, bool& foundNormalTexture, bool& foundDiffuseTexture, bool& foundSpecularTexture, bool& foundEmissiveTexture)
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
			else if (property.childProperties[i].propertyInfo.lName == 'NAME' &&
				property.childProperties[i].stringValue == "mapEmissive_01")
			{
				texture.type = Texture::Type::Emissive;
				foundEmissiveTexture = true;
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
					else if (texture.type == Texture::Type::Specular)
					{
						foundSpecularTexture = false;
					}
					else if (texture.type == Texture::Type::Emissive)
					{
						foundEmissiveTexture = false;
					}
				}
			}
		}

		GetTextures(property.childProperties[i], matiResource, textures, foundNormalTexture, foundDiffuseTexture, foundSpecularTexture, foundEmissiveTexture);
	}

	if (texture.type != Texture::Type::Unknown && texture.textureReferenceIndex != -1)
	{
		textures.push_back(texture);
	}
}

std::unordered_map<unsigned int, std::string>& RenderMaterialInstance::GetMaterialPropertyNames()
{
	return materialPropertyNames;
}
