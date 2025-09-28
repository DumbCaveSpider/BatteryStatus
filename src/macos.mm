#if defined(__APPLE__)
#import <TargetConditionals.h>
#if (defined(TARGET_OS_OSX) && TARGET_OS_OSX) || (defined(TARGET_OS_MAC) && TARGET_OS_MAC && !TARGET_OS_IPHONE)

#define CommentType CommentTypeDummy
#import <Foundation/Foundation.h>
#import <IOKit/ps/IOPSKeys.h>
#import <IOKit/ps/IOPowerSources.h>
#undef CommentType

// i assume this works fine for macos users
namespace battery
{

    int getBatteryLevel()
    {
        @autoreleasepool
        {
            CFTypeRef powerSourceInfo = IOPSCopyPowerSourcesInfo();
            if (!powerSourceInfo)
                return -1;

            CFArrayRef powerSources = IOPSCopyPowerSourcesList(powerSourceInfo);
            if (!powerSources)
            {
                CFRelease(powerSourceInfo);
                return -1;
            }

            CFIndex count = CFArrayGetCount(powerSources);
            if (count == 0)
            {
                CFRelease(powerSources);
                CFRelease(powerSourceInfo);
                return -1;
            }

            CFDictionaryRef powerSource = NULL;
            for (CFIndex i = 0; i < count; i++)
            {
                powerSource = (CFDictionaryRef)CFArrayGetValueAtIndex(powerSources, i);
                CFStringRef powerSourceState =
                    (CFStringRef)CFDictionaryGetValue(powerSource, CFSTR(kIOPSTypeKey));

                if (powerSourceState &&
                    CFEqual(powerSourceState, CFSTR(kIOPSInternalBatteryType)))
                {
                    break;
                }
                powerSource = NULL;
            }

            if (!powerSource)
            {
                CFRelease(powerSources);
                CFRelease(powerSourceInfo);
                return -1;
            }

            CFNumberRef currentCapacity = (CFNumberRef)CFDictionaryGetValue(
                powerSource, CFSTR(kIOPSCurrentCapacityKey));
            CFNumberRef maxCapacity = (CFNumberRef)CFDictionaryGetValue(
                powerSource, CFSTR(kIOPSMaxCapacityKey));

            int currentValue = 0;
            int maxValue = 0;

            if (currentCapacity && maxCapacity &&
                CFNumberGetValue(currentCapacity, kCFNumberIntType, &currentValue) &&
                CFNumberGetValue(maxCapacity, kCFNumberIntType, &maxValue) &&
                maxValue > 0)
            {

                int percentage =
                    static_cast<int>((float)currentValue / (float)maxValue * 100.0f);

                CFRelease(powerSources);
                CFRelease(powerSourceInfo);

                return percentage;
            }

            CFRelease(powerSources);
            CFRelease(powerSourceInfo);
        }

        return -1;
    }

    bool isCharging()
    {
        @autoreleasepool
        {
            CFTypeRef powerSourceInfo = IOPSCopyPowerSourcesInfo();
            if (!powerSourceInfo)
                return false;

            CFArrayRef powerSources = IOPSCopyPowerSourcesList(powerSourceInfo);
            if (!powerSources)
            {
                CFRelease(powerSourceInfo);
                return false;
            }

            CFIndex count = CFArrayGetCount(powerSources);
            if (count == 0)
            {
                CFRelease(powerSources);
                CFRelease(powerSourceInfo);
                return false;
            }

            CFDictionaryRef powerSource = NULL;
            for (CFIndex i = 0; i < count; i++)
            {
                powerSource = (CFDictionaryRef)CFArrayGetValueAtIndex(powerSources, i);
                CFStringRef powerSourceState =
                    (CFStringRef)CFDictionaryGetValue(powerSource, CFSTR(kIOPSTypeKey));

                if (powerSourceState &&
                    CFEqual(powerSourceState, CFSTR(kIOPSInternalBatteryType)))
                {
                    break;
                }
                powerSource = NULL;
            }

            if (!powerSource)
            {
                CFRelease(powerSources);
                CFRelease(powerSourceInfo);
                return false;
            }

            CFStringRef charging = (CFStringRef)CFDictionaryGetValue(
                powerSource, CFSTR(kIOPSPowerSourceStateKey));

            bool isCharging = charging && CFEqual(charging, CFSTR(kIOPSACPowerValue));

            CFRelease(powerSources);
            CFRelease(powerSourceInfo);

            return isCharging;
        }
    }

    bool isBatterySaver()
    {
        @autoreleasepool
        {
            if (@available(macOS 10.14, *))
            {
                NSProcessInfo *processInfo = [NSProcessInfo processInfo];
                if (@available(macOS 12.0, *))
                {
                    return processInfo.lowPowerModeEnabled;
                }
                return false;
            }
            return false;
        }
    }

}

#endif
#endif