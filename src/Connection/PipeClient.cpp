#include <format>

#include "Connection/PipeClient.h"
#include "Mutex.h"
#include "Logger.h"

PipeClient::PipeClient()
{
	pipe = nullptr;
	isConnectedWithEngine = false;
	isRunning = true;
	pendingMessageLength = 0;

	InitializeSRWLock(&srwLock);
}

PipeClient::~PipeClient()
{
	if (isConnectedWithEngine)
	{
		Disconnect();
	}
}

PipeClient& PipeClient::GetInstance()
{
	static PipeClient instance;

	return instance;
}

void PipeClient::Connect()
{
	pipe = CreateFileA("\\\\.\\pipe\\pipe", GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

	if (pipe == INVALID_HANDLE_VALUE)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to connect editor with engine! Error: {}", Logger::GetLastError());

		return;
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Info, "Successfully connected editor with engine.");

		isConnectedWithEngine = true;
	}

	thread = std::thread(&PipeClient::Update, this);
}

void PipeClient::Disconnect()
{
	if (isConnectedWithEngine)
	{
		DisconnectNamedPipe(pipe);
	}
}

void PipeClient::Update()
{
	while (isRunning)
	{
		Sleep(5);

		if (pipe == nullptr)
		{
			continue;
		}

		Logger& logger = Logger::GetInstance();

		AcquireSRWLockExclusive(&srwLock);

		if (!queuedData.empty())
		{
			std::string data;
			unsigned int dataLength = static_cast<unsigned int>(queuedData.front().size());

			data.append(reinterpret_cast<char*>(&dataLength), 4);
			data.append(queuedData.front().data(), dataLength);

			DWORD written;

			if (!WriteFile(pipe, data.data(), static_cast<DWORD>(data.size()), &written, nullptr))
			{
				logger.Log(Logger::Level::Error, "Failed to send pipe data. Error: {}", Logger::GetLastError());

				CloseHandle(pipe);
				pipe = nullptr;
				isConnectedWithEngine = false;

				continue;
			}

			queuedData.pop();
		}

		ReleaseSRWLockExclusive(&srwLock);

		DWORD pendingBytes;

		if (!PeekNamedPipe(pipe, nullptr, 0, nullptr, &pendingBytes, nullptr))
		{
			logger.Log(Logger::Level::Error, "Failed to peek pipe. Error: {}", Logger::GetLastError());

			CloseHandle(pipe);
			pipe = nullptr;
			isConnectedWithEngine = false;

			continue;
		}

		if (pendingBytes <= 0)
		{
			continue;
		}

		if (pendingMessageLength == 0)
		{
			char messageLengthBuffer[4];
			DWORD messageLengthBufferLength;

			if (!ReadFile(pipe, messageLengthBuffer, 4, &messageLengthBufferLength, nullptr) || messageLengthBufferLength != 4)
			{
				logger.Log(Logger::Level::Error, "Failed to read message length. Error: {}", Logger::GetLastError());

				CloseHandle(pipe);
				pipe = nullptr;
				isConnectedWithEngine = false;

				continue;
			}

			pendingMessageLength = *(unsigned int*)&messageLengthBuffer[0];

			continue;
		}

		if (pendingMessageLength > 0 && pendingBytes < pendingMessageLength)
		{
			continue;
		}

		char* messageBuffer = new char[pendingMessageLength];
		DWORD messageBufferLength;

		// Read the handshake length.
		if (!ReadFile(pipe, messageBuffer, pendingMessageLength, &messageBufferLength, nullptr) || messageBufferLength != pendingMessageLength)
		{
			logger.Log(Logger::Level::Error, "Failed to read message. Error: {}", Logger::GetLastError());

			delete[] messageBuffer;

			CloseHandle(pipe);
			pipe = nullptr;
			isConnectedWithEngine = false;

			continue;
		}

		std::string message = std::string(messageBuffer, pendingMessageLength);
		std::string type = message.substr(0, message.find("#"));
		std::string content = message.substr(message.find("#") + 1);

		pendingMessageLength = 0;
		delete[] messageBuffer;

		if (type.starts_with("sdk"))
		{
			Logger::Level level = static_cast<Logger::Level>(std::atoi(type.substr(4).c_str()));

			logger.Log(Logger::Type::SDK, "engine.exe", level, content);
		}
		else
		{
			logger.Log(Logger::Type::Pipe, "engine.exe", Logger::Level::Info, content);

			messageCallback(type, content);
		}
	}
}

void PipeClient::SendMessage(const std::string& type, const std::string& content)
{
	ScopedExclusiveGuard scopedSharedGuard = ScopedExclusiveGuard(&srwLock);

	std::string data = std::format("{}#{}", type, content);

	queuedData.push(data);
}

void PipeClient::SetMessageCallback(MessageCallback messageCallback)
{
	this->messageCallback = messageCallback;
}

bool PipeClient::IsConnectedWithEngine() const
{
	return isConnectedWithEngine;
}
