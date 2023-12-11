#pragma once

#include <vector>
#include <string>
#include <format>

#include "Mutex.h"

#undef GetCurrentTime

class Logger
{
public:
	enum class Level
	{
		Info,
		Warning,
		Error
	};

	enum class Type
	{
		Editor,
		SDK,
		Pipe
	};

	struct Message
	{
		std::string ToString() const
		{
			std::string type = TypeToString(this->type);
			std::string level = LevelToString(this->level);

			return std::format("ID: {}, Type: {}, Level: {}, Time: {}, Process: {}, Content: {}", id, type, level, time, process, content);
		}

		int id;
		Type type;
		Level level;
		std::string content;
		std::string time;
		std::string process;
	};

	static Logger& GetInstance();
	static const char* LevelToString(const Level level);
	static const char* TypeToString(const Type type);
	std::vector<Message>& GetMessages();
	void ClearMessage(const unsigned int index);
	void ClearAllMessages();
	static std::string GetCurrentTime();
	static std::string GetLastError();

	template <typename... Args>
	void Log(const Level level, const std::string& format, const Args&... args)
	{
		Log(Type::Editor, "HitmanAbsolutionEditor.exe", level, format, args...);
	}

	template <typename... Args>
	void Log(const Type type, const std::string& process, const Level level, const std::string& format, const Args&... args)
	{
		ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&srwLock);
		Message message;

		message.id = static_cast<int>(messages.size());
		message.type = type;
		message.level = level;
		message.content = std::vformat(format, std::make_format_args(args...));
		message.time = GetCurrentTime();
		message.process = process;

		messages.push_back(message);
	}

private:
	Logger();
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;

	SRWLOCK srwLock;
	std::vector<Message> messages;
};
