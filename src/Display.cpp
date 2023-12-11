#include "Display.h"
#include "Logger.h"

unsigned int Display::GetWidth()
{
    RECT desktop;
    const HWND hwnd = GetDesktopWindow();

    GetWindowRect(hwnd, &desktop);

    return desktop.right;
}

unsigned int Display::GetHeight()
{
    RECT desktop;
    const HWND hwnd = GetDesktopWindow();

    GetWindowRect(hwnd, &desktop);

    return desktop.bottom;
}

unsigned int Display::GetRefreshRate()
{
    DEVMODE dm;

    dm.dmSize = sizeof(dm);

    if (EnumDisplaySettings(nullptr, ENUM_CURRENT_SETTINGS, &dm) == 0)
    {
        Logger::GetInstance().Log(Logger::Level::Error, "Failed to retrieve display settings!");

        return 60;
    }

    return dm.dmDisplayFrequency;
}
