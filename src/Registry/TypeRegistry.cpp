#include <fstream>

#include "rapidjson/istreamwrapper.h"

#include "Registry/TypeRegistry.h"
#include "Logger.h"

TypeRegistry::~TypeRegistry()
{
	for (auto it = typeIDs.begin(); it != typeIDs.end(); ++it)
	{
		delete it->second;
	}
}

TypeRegistry& TypeRegistry::GetInstance()
{
    static TypeRegistry instance;

    return instance;
}

void TypeRegistry::Load()
{
	std::ifstream inputFileStream = std::ifstream("assets/TypeIDs.json");
	rapidjson::IStreamWrapper streamWrapper(inputFileStream);
	rapidjson::Document document;

	document.ParseStream(streamWrapper);

	const rapidjson::Value& typeIDsArray = document["typeIDs"];

	for (rapidjson::Value::ConstValueIterator it = typeIDsArray.Begin(); it != typeIDsArray.End(); ++it)
	{
		const rapidjson::Value& object = it->GetObj();
		const rapidjson::Value& object2 = object["pTypeInfo"].GetObj();

		STypeID* typeID = new STypeID();

		typeID->flags = object["flags"].GetUint();
		typeID->typeNum = object["typeNum"].GetUint();

		const char* typeName = object2["pszTypeName"].GetString();
		const unsigned int typeSize = object2["m_nTypeSize"].GetUint();
		const unsigned int typeAlignment = object2["m_nTypeAlignment"].GetUint();
		const unsigned int typeInfoFlags = object2["m_nTypeInfoFlags"].GetUint();

		IType* type = new IType(typeSize, typeAlignment, typeInfoFlags, typeName, typeID);

		typeID->pTypeInfo = type;

		typeIDs.insert(std::make_pair(typeName, typeID));
	}

	Logger::GetInstance().Log(Logger::Level::Info, "Sucessfully loaded type ids.");
}

STypeID* TypeRegistry::GetTypeID(const std::string& typeName) const
{
    auto it = typeIDs.find(typeName);

    if (it != typeIDs.end())
    {
        return it->second;
    }

    return nullptr;
}

const std::unordered_map<std::string, STypeID*>& TypeRegistry::GetTypeIDs() const
{
	return typeIDs;
}

void TypeRegistry::RegisterType(STypeID* typeID)
{
	typeIDs.insert(std::make_pair(typeID->pTypeInfo->GetTypeName(), typeID));
}
