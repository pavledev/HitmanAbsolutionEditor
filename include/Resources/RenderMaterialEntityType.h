#pragma once

#include "Resource.h"

class RenderMaterialEntityType : public Resource
{
public:
	enum class ModifierType
	{
		UInt = 1,
		SColorRGB,
		SColorRGBA,
		Float,
		SVector2,
		SVector3,
		SVector4
	};

	struct Modifier
	{
		Modifier();
		~Modifier();

		ModifierType type;
		void* value;
	};

	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	void SerializeToJson(const std::string& outputFilePath);
	std::vector<std::shared_ptr<Modifier>>& GetModifiers();

private:
	std::vector<std::shared_ptr<Modifier>> modifiers;
};
