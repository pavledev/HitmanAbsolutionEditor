#include <regex>
#include <Windows.h>

#include "Utility/StringUtility.h"

std::wstring StringUtility::AnsiStringToWideString(const std::string& string)
{
	std::wstring result = std::wstring(string.length(), 0);

	std::transform(string.begin(), string.end(), result.begin(), [](char c)
	{
		return (wchar_t)c;
	});

	return result;
}

std::string StringUtility::WideStringToAnsiString(const std::wstring& wstring)
{
	std::string result = std::string(wstring.length(), 0);

	std::transform(wstring.begin(), wstring.end(), result.begin(), [](wchar_t c)
	{
		return (char)c;
	});

	return result;
}

std::wstring StringUtility::GetExecutableFolderPath()
{
	wchar_t path[MAX_PATH] = { 0 };

	GetModuleFileNameW(nullptr, path, MAX_PATH);

	std::wstring folderPath = path;

	return folderPath.substr(0, folderPath.find_last_of(L"\\"));
}

std::string StringUtility::ToUpperCase(std::string string)
{
	for (size_t i = 0; i < string.length(); ++i)
	{
		string[i] = std::toupper(string[i]);
	}

	return string;
}

std::string StringUtility::ToLowerCase(std::string string)
{
	for (size_t i = 0; i < string.length(); ++i)
	{
		string[i] = std::tolower(string[i]);
	}

	return string;
}

std::vector<std::string> StringUtility::Split(const std::string& string, const char delimiter)
{
	std::vector<std::string> result;
	std::stringstream ss(string);
	std::string item;

	while (getline(ss, item, delimiter))
	{
		result.push_back(item);
	}

	return result;
}

std::vector<std::string> StringUtility::Split(const std::string& string, const std::string& delimiter)
{
	size_t startPosition = 0;
	size_t endPosition = 0;
	size_t delimiterLength = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((endPosition = string.find(delimiter, startPosition)) != std::string::npos)
	{
		token = string.substr(startPosition, endPosition - startPosition);
		startPosition = endPosition + delimiterLength;
		res.push_back(token);
	}

	res.push_back(string.substr(startPosition));

	return res;
}

void StringUtility::Split(const std::string& string, std::vector<std::string>& parts, const char delimiter)
{
	std::stringstream ss(string);
	std::string item;

	while (getline(ss, item, delimiter))
	{
		parts.push_back(item);
	}
}

void StringUtility::Split(const std::string& string, std::vector<std::string>& parts, const std::string& delimiter)
{
	size_t startPosition = 0;
	size_t endPosition = 0;
	size_t delimiterLength = delimiter.length();
	std::string token;

	while ((endPosition = string.find(delimiter, startPosition)) != std::string::npos)
	{
		token = string.substr(startPosition, endPosition - startPosition);
		startPosition = endPosition + delimiterLength;
		parts.push_back(token);
	}

	parts.push_back(string.substr(startPosition));
}

size_t StringUtility::Count(const std::string& string, const char ch)
{
	return std::count(string.begin(), string.end(), ch);
}

std::string StringUtility::ConvertFloatToString(const float value)
{
	std::stringstream stringstream;

	stringstream << std::fixed << std::setprecision(10) << value;

	return stringstream.str();
}

std::string StringUtility::GetStringAfterExpression(const std::string& string, const std::string& expression)
{
	// ("The quick brown fox", "brown") -> "fox"
	const size_t position = string.find(expression);
	return position != std::string::npos ? string.substr(position + expression.length()) : "";
}

std::string StringUtility::GetStringBetweenExpressions(const std::string& string, const std::string& expressionA, const std::string& expressionB)
{
	// ("The quick brown fox", "The ", " brown") -> "quick"

	const std::regex baseRegex(expressionA + "(.*)" + expressionB);
	std::smatch baseMatch;

	if (regex_search(string, baseMatch, baseRegex))
	{
		// The first sub_match is the whole string; the next
		// sub_match is the first parenthesized expression.
		if (baseMatch.size() == 2)
		{
			return baseMatch[1].str();
		}
	}

	return string;
}
