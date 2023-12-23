#pragma once

#include <algorithm>
#include <filesystem>

class StringUtility
{
public:
	template <typename T>
	static std::string ConvertValueToHexString(const T value)
	{
		std::stringstream stringstream;

		stringstream << "0x" << std::hex << std::uppercase << value;

		return stringstream.str();
	}

	static std::wstring AnsiStringToWideString(const std::string& string);
	static std::string WideStringToAnsiString(const std::wstring& wstring);
	static std::wstring GetExecutableFolderPath();
	static std::string ToUpperCase(const std::string string);
	static std::string ToLowerCase(const std::string string);
	static std::vector<std::string> Split(const std::string& string, const char delimiter);
	static std::vector<std::string> Split(const std::string& string, const std::string& delimiter);
	static void Split(const std::string& string, std::vector<std::string>& parts, const char delimiter);
	static void Split(const std::string& string, std::vector<std::string>& parts, const std::string& delimiter);
	static size_t Count(const std::string& string, const char ch);
	static std::string ConvertFloatToString(const float value);

	static std::string GetStringAfterExpression(const std::string& string, const std::string& expression);
	static std::string GetStringBetweenExpressions(const std::string& string, const std::string& expressionA, const std::string& expressionB);

	static size_t FindString(const char* str, const char* stringToFind, size_t length, size_t position = 0);
	static bool CompareStrings(const char* first, const char* second, size_t length, size_t position);
};
