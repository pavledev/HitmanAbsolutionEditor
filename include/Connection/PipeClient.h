#pragma once

#include <string>
#include <functional>
#include <thread>
#include <queue>
#include <Windows.h>
#include "Delegate.h"

#undef SendMessage

class PipeClient
{
public:
	using MessageCallback = Delegate<void(const std::string& type, const std::string& content)>;

	PipeClient();
	~PipeClient();
	static PipeClient& GetInstance();
	void Connect();
	void Disconnect();
	void Update();
	void SendMessage(const std::string& type, const std::string& content);
	void SetMessageCallback(MessageCallback messageCallback);
	bool IsConnectedWithEngine() const;

private:
	HANDLE pipe;
	bool isConnectedWithEngine;
	std::thread thread;
	std::queue<std::string> queuedData;
	bool isRunning;
	unsigned int pendingMessageLength;
	MessageCallback messageCallback;
	SRWLOCK srwLock;
};
