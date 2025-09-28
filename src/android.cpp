#if defined(__ANDROID__)
#include <jni.h>
#include <android/native_activity.h>
#include <android/log.h>

#include <Geode/cocos/platform/android/jni/JniHelper.h>

namespace battery {


int getBatteryLevel() {
    cocos2d::JniMethodInfo methodInfo;
    int batteryLevel = -1;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
        "org/cocos2dx/lib/Cocos2dxHelper",
        "getBatteryLevel",
        "()I")) {
        batteryLevel = methodInfo.env->CallStaticIntMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return batteryLevel;
}

bool isCharging() {
    cocos2d::JniMethodInfo methodInfo;
    bool charging = false;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
        "org/cocos2dx/lib/Cocos2dxHelper",
        "isCharging",
        "()Z")) {
        charging = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return charging;
}

bool isBatterySaver() {
    cocos2d::JniMethodInfo methodInfo;
    bool saver = false;
    if (cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
        "org/cocos2dx/lib/Cocos2dxHelper",
        "isBatterySaver",
        "()Z")) {
        saver = methodInfo.env->CallStaticBooleanMethod(methodInfo.classID, methodInfo.methodID);
        methodInfo.env->DeleteLocalRef(methodInfo.classID);
    }
    return saver;
}

}
#endif
