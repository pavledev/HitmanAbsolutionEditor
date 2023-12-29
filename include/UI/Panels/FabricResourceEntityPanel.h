#pragma once

#include "Utility/UI.h"
#include "BasePanel.h"
#include "Resources/FabricResourceEntity.h"
#include "Resources/FabricResourceEntityBlueprint.h"

class FabricResourceEntityPanel : public BasePanel
{
public:
	FabricResourceEntityPanel(const char* name, const char* icon, std::shared_ptr<FabricResourceEntity> fabricResourceEntity);
	void Render() override;
	void OnResourceLoaded();

private:
	void RenderProperties();

	void SClothPiecePropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::SClothPiecePropertySet& clothPiecePropertySet);
	void SClothPieceExtendedPropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::SClothPieceExtendedPropertySet& clothPieceExtendedPropertySet);
	void STransformPropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::STransformPropertySet& transformPropertySet);
	void SStrandsPropertySetProperty(const char* propertyName, ZFabricResourceEntity::SAddedPropertyValues::SStrandsPropertySet& strandsPropertySet);
	void SVector3Property(const char* propertyName, SVector3& vector);
	void ZRuntimeResourceIDProperty(const char* propertyName, ZRuntimeResourceID& runtimeResourceID);

	std::shared_ptr<FabricResourceEntity> fabricResourceEntity;
	std::shared_ptr<FabricResourceEntityBlueprint> fabricResourceEntityBlueprint;
	std::vector<UI::TableColumn> tableColumns;
};
