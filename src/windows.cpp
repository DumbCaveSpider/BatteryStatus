#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace battery
{
    int getBatteryLevel()
    {
        SYSTEM_POWER_STATUS s;
        if (!GetSystemPowerStatus(&s))
            return -1;
        if (s.BatteryLifePercent == 255)
            return -1;
        return s.BatteryLifePercent;
    }

    bool isCharging()
    {
        SYSTEM_POWER_STATUS s;
        if (!GetSystemPowerStatus(&s))
            return false;
        return (s.ACLineStatus == 1);
    }

    bool isBatterySaver()
    {
        SYSTEM_POWER_STATUS s;
        if (GetSystemPowerStatus(&s))
        {
            return (s.SystemStatusFlag & 1) != 0;
        }
        return false;
    }
}
#endif