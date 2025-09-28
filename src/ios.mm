#if defined(GEODE_IS_IOS)
#import <UIKit/UIKit.h>

namespace battery
{

    int getBatteryLevel()
    {
        @autoreleasepool
        {
            UIDevice *d = [UIDevice currentDevice];
            if (!d.batteryMonitoringEnabled)
            {
                d.batteryMonitoringEnabled = YES;
                [NSThread sleepForTimeInterval:0.05];
            }

            float level = d.batteryLevel;
            if (level < 0.0f)
                return -1;
            return static_cast<int>(level * 100.0f);
        }
    }

    bool isCharging()
    {
        @autoreleasepool
        {
            UIDevice *d = [UIDevice currentDevice];
            if (!d.batteryMonitoringEnabled)
            {
                d.batteryMonitoringEnabled = YES;
                [NSThread sleepForTimeInterval:0.05];
            }

            UIDeviceBatteryState s = d.batteryState;
            return (s == UIDeviceBatteryStateCharging || s == UIDeviceBatteryStateFull);
        }
    }

    bool isBatterySaver()
    {
        @autoreleasepool
        {
            NSProcessInfo *processInfo = [NSProcessInfo processInfo];
            return [processInfo isLowPowerModeEnabled];
        }
    }
}
#endif