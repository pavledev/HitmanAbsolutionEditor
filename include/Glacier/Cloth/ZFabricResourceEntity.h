#pragma once

#include "../Math/SVector3.h"
#include "../Resource/ZRuntimeResourceID.h"
#include "../Resource/ZResourcePtr.h"
#include "../Entity/TEntityRef.h"

class ZSpatialEntity;

class ZFabricResourceEntity
{
public:
	struct SAddedPropertyValues
	{
		struct SClothPiecePropertySet
		{
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

			SVector3 m_vGravity;
			unsigned int m_nNumIterations;
			float m_fDamping;
		};

		struct SClothPieceExtendedPropertySet
		{
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

			ZRuntimeResourceID m_MaterialInstance;
			ZResourcePtr m_MaterialInstancePtr;
			SVector3 m_vGravity;
			unsigned int m_nNumIterations;
			float m_fDamping;
		};

		struct STransformPropertySet
		{
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

			TEntityRef<ZSpatialEntity> m_TransformHook;
		};

		struct SStrandsPropertySet
		{
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

			float m_fStrandWidth;
		};
	};
};
