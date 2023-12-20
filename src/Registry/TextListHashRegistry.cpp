#include <fstream>

#include "rapidjson/istreamwrapper.h"
#include "rapidjson/document.h"

#include "Registry/TextListHashRegistry.h"
#include "Logger.h"

TextListHashRegistry& TextListHashRegistry::GetInstance()
{
	static TextListHashRegistry instance;

	return instance;
}

void TextListHashRegistry::Load()
{
	std::ifstream inputFileStream = std::ifstream("assets/TextList_Hashes.json");
	rapidjson::IStreamWrapper streamWrapper(inputFileStream);
	rapidjson::Document document;

	document.ParseStream(streamWrapper);

	for (rapidjson::Value::ConstMemberIterator it = document.MemberBegin(); it != document.MemberEnd(); ++it)
	{
		unsigned int hash = std::strtoul(it->name.GetString(), nullptr, 16);

		textListHashes.insert(std::make_pair(hash, it->value.GetString()));
	}

	Logger::GetInstance().Log(Logger::Level::Info, "Sucessfully loaded text list names and hashes.");
}

const std::string& TextListHashRegistry::GetName(const unsigned int hash) const
{
	auto iterator = textListHashes.find(hash);

	if (iterator != textListHashes.end())
	{
		return iterator->second;
	}

	static std::string name;

	return name;
}
