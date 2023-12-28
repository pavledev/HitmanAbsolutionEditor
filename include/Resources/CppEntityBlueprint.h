#pragma once

#include <Glacier/Entity/SCppEntityBlueprint.h>

#include "Resource.h"

class CppEntityBlueprint : public Resource
{
public:
	void Deserialize() override;
	void Parse(void* cppEntityBlueprint);
	std::shared_ptr<SCppEntityBlueprint> GetCppEntityBlueprint();

private:
	std::shared_ptr<SCppEntityBlueprint> cppEntityBlueprint;
};
