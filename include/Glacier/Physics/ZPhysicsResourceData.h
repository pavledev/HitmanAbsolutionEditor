#pragma once

#include <map>
#include <string>

#include "rapidjson/prettywriter.h"

#include "SShatterConfig.h"
#include "G2NxShapeDesc.h"
#include "SKinematicLinkedPrimResourceData.h"
#include "EObjectEntity.h"
#include "ECollisionPack.h"

class ZPhysicsResourceData
{
public:
	struct SHeaderData
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);
		static std::string ConvertObjectTypeToString(const EObjectEntity objectEntity);
		static std::string ConvertCollisionFlagsToString(const ECollisionPack collisionPack);

		unsigned int m_eObjType;
		unsigned int m_nCollFlags;
	};

	struct SShatterData
	{
		void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

		struct SShardData
		{
			void SerializeToJson(rapidjson::PrettyWriter<rapidjson::StringBuffer>& writer);

			unsigned int m_nShardIndex;
			std::vector<G2NxShapeDesc*> m_shapes;
		};

		SShatterConfig m_sShatterConfig;
		std::map<unsigned int, SShatterData::SShardData> m_shardMap;
	};
};
