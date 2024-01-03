#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "DirectXTex.h"

#include "Glacier/Material/SProperty.h"
#include "Glacier/Material/SRMaterialPropertyList.h"
#include "Glacier/Material/PROPERTY_TYPE.h"

#include "IO/BinaryReader.h"
#include "Resource.h"

class RenderMaterialInstance : public Resource
{
public:
	struct Property
	{
		void SerializeToJson(const std::vector<std::shared_ptr<Resource>>& references, rapidjson::Value& propertyObject, rapidjson::Document::AllocatorType& allocator);

		SProperty propertyInfo;
		std::vector<Property> childProperties;
		unsigned int uint32Value;
		float floatValue;
		std::string stringValue;
	};

	struct Texture
	{
		enum class Type
		{
			Unknown,
			Diffuse,
			Normal,
			Specular,
			Emissive
		};

		Type type;
		DirectX::Blob* blob = nullptr;
		int textureReferenceIndex = -1;
		std::string name;
	};

	RenderMaterialInstance();
	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(const std::string& outputFilePath);
	SRMaterialPropertyList& GetMaterialPropertyList();
	std::string& GetMaterialClassType();
	Property& GetInstanceProperty();
	void GetTextures(std::shared_ptr<Resource> matiResource, std::vector<RenderMaterialInstance::Texture>& textures);
	void GetTextures(const Property& property, std::shared_ptr<Resource> matiResource, std::vector<Texture>& textures, bool& foundNormalTexture, bool& foundDiffuseTexture, bool& foundSpecularTexture);
	static std::unordered_map<unsigned int, std::string>& GetMaterialPropertyNames();

private:
	void ReadProperty(Property& property, BinaryReader& binaryReader, const unsigned int propertyOffset);

	SRMaterialPropertyList materialPropertyList;
	std::string materialClassType;
	Property instanceProperty;
	inline static std::unordered_map<unsigned int, std::string> materialPropertyNames;
};
