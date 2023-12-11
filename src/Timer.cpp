#include <thread>

#include "Timer.h"
#include "Display.h"

void Timer::Initialize()
{
    endSleepTime = std::chrono::high_resolution_clock::now();
}

void Timer::Tick()
{
    startSleepTime = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> deltaTime = startSleepTime - endSleepTime;

    double targetMs = 1000.0 / fpsLimit;

    while (deltaTime.count() < targetMs)
    {
        // Yield the CPU to other threads/processes instead of busy waiting.
        std::this_thread::yield();

        deltaTime = std::chrono::steady_clock::now() - startSleepTime;
    }

    endSleepTime = std::chrono::steady_clock::now();
    deltaTimeMs = static_cast<double>(deltaTime.count());
}

const double Timer::GetDeltaTime()
{
    return deltaTimeMs / 1000.0;
}

void Timer::SetFPSLimit()
{
    fpsLimit = static_cast<float>(Display::GetRefreshRate());
}
