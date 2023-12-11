#include "Connection/SharedMemoryClient.h"
#include "Logger.h"

SharedMemoryClient::SharedMemoryClient()
{
	mapFile = nullptr;
	data = nullptr;
}

SharedMemoryClient::~SharedMemoryClient()
{
	UnmapViewOfFile(data);
	CloseHandle(mapFile);
}

SharedMemoryClient& SharedMemoryClient::GetInstance()
{
	static SharedMemoryClient instance;

	return instance;
}

SharedMemoryClient::Data* SharedMemoryClient::GetData()
{
	return data;
}

void SharedMemoryClient::Connect()
{
	mapFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, "FileMappingObject");

	if (mapFile == INVALID_HANDLE_VALUE)
	{
		Logger::GetInstance().Log(Logger::Level::Error, "Failed to open file mapping object. Error: {}", Logger::GetLastError());

		return;
	}
	else
	{
		Logger::GetInstance().Log(Logger::Level::Info, "Successfully created file mapping object.");
	}

	if (mapFile)
	{
		data = static_cast<Data*>(MapViewOfFile(mapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Data)));

		if (!data)
		{
			Logger::GetInstance().Log(Logger::Level::Error, "Could not map view of file. Error: {}", Logger::GetLastError());

			CloseHandle(mapFile);
		}
	}
}
