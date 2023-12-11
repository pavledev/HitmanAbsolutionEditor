#pragma once

#include <chrono>

class Timer
{
public:
	static void Initialize();
	static void Tick();
	static const double GetDeltaTime();
	static void SetFPSLimit();

private:
	inline static double deltaTimeMs;
	inline static std::chrono::high_resolution_clock::time_point startSleepTime;
	inline static std::chrono::high_resolution_clock::time_point endSleepTime;
	inline static float fpsLimit;
};
