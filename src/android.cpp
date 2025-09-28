#if defined(__ANDROID__)
#include <Geode/cocos/platform/android/jni/JniHelper.h>
#include <jni.h>

namespace battery {

int getBatteryLevel() {
    cocos2d::JniMethodInfo mi;
    int level = -1;
    if (cocos2d::JniHelper::getStaticMethodInfo(mi,
        "org/cocos2dx/lib/Cocos2dxHelper",
        "getBatteryLevel",
        "()I")) {
        level = mi.env->CallStaticIntMethod(mi.classID, mi.methodID);
        mi.env->DeleteLocalRef(mi.classID);
    }
    return level;
}

bool isCharging() {
    cocos2d::JniMethodInfo mi;
    bool charging = false;
    if (cocos2d::JniHelper::getStaticMethodInfo(mi,
        "org/cocos2dx/lib/Cocos2dxHelper",
        "isCharging",
        "()Z")) {
        charging = mi.env->CallStaticBooleanMethod(mi.classID, mi.methodID);
        mi.env->DeleteLocalRef(mi.classID);
    }
    return charging;
}

bool isBatterySaver() {
    cocos2d::JniMethodInfo mi;
    bool saver = false;
    if (cocos2d::JniHelper::getStaticMethodInfo(mi,
        "org/cocos2dx/lib/Cocos2dxHelper",
        "isBatterySaver",
        "()Z")) {
        saver = mi.env->CallStaticBooleanMethod(mi.classID, mi.methodID);
        mi.env->DeleteLocalRef(mi.classID);
    }
    return saver;
}

}

#endif
