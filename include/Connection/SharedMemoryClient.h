#pragma once

#include <Windows.h>

#include "imgui.h"

class SharedMemoryClient
{
public:
	struct Data
	{
		ImVec2 mouseCoordinates;
		bool sceneViewPortHasFocus;
	};

	SharedMemoryClient();
	~SharedMemoryClient();
	static SharedMemoryClient& GetInstance();
	Data* GetData();
	void Connect();

private:
	HANDLE mapFile;
	Data* data;
};
