#pragma once

#include <string>
#include <unordered_map>

class TextListHashRegistry
{
public:
	static TextListHashRegistry& GetInstance();
	void Load();
	const std::string& GetName(const unsigned int hash) const;

private:
	TextListHashRegistry() = default;
	TextListHashRegistry(const TextListHashRegistry& other) = delete;
	TextListHashRegistry& operator=(const TextListHashRegistry& other) = delete;

	std::unordered_map<unsigned int, std::string> textListHashes;
};
