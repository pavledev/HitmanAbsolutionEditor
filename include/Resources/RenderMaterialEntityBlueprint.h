#pragma once

#include "Resource.h"

class RenderMaterialEntityBlueprint : public Resource
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
		ModifierType type;
		std::string name;
	};

	void Deserialize() override;
	void Export(const std::string& outputPath, const std::string& exportOption) override;
	std::vector<std::shared_ptr<Modifier>>& GetModifiers();

private:
	std::vector<std::shared_ptr<Modifier>> modifiers;
};
