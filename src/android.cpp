#if defined(__ANDROID__)
#include <jni.h>
#include <Geode/Geode.hpp>

#include <platform/android/jni/JniHelper.h>
#include <cocos/platform/android/jni/JniHelper.h>

using namespace geode::prelude;

namespace
{
    JNIEnv *getEnv()
    {
        return cocos2d::JniHelper::getEnv();
    }

    jobject getActivity(JNIEnv *env)
    {
        jclass helperCls = env->FindClass("org/cocos2dx/lib/Cocos2dxHelper");
        if (!helperCls)
            return nullptr;
        jmethodID mid = env->GetStaticMethodID(helperCls, "getActivity", "()Landroid/app/Activity;");
        if (!mid)
            return nullptr;
        jobject activity = env->CallStaticObjectMethod(helperCls, mid);
        return activity;
    }

    int callGetLevel(JNIEnv *env, jobject activity)
    {
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            return -1;
        jmethodID mid = env->GetStaticMethodID(cls, "getLevel", "(Landroid/app/Activity;)I");
        if (!mid)
            return -1;
        jint result = env->CallStaticIntMethod(cls, mid, activity);
        return static_cast<int>(result);
    }

    bool callIsCharging(JNIEnv *env, jobject activity)
    {
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            return false;
        jmethodID mid = env->GetStaticMethodID(cls, "isCharging", "(Landroid/app/Activity;)Z");
        if (!mid)
            return false;
        jboolean result = env->CallStaticBooleanMethod(cls, mid, activity);
        return result == JNI_TRUE;
    }

    bool callIsBatterySaver(JNIEnv *env, jobject activity)
    {
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            return false;
        jmethodID mid = env->GetStaticMethodID(cls, "isBatterySaver", "(Landroid/app/Activity;)Z");
        if (!mid)
            return false;
        jboolean result = env->CallStaticBooleanMethod(cls, mid, activity);
        return result == JNI_TRUE;
    }
}

namespace battery
{
    int getBatteryLevel()
    {
        JNIEnv *env = getEnv();
        if (!env)
            return -1;
        jobject activity = getActivity(env);
        if (!activity)
            return -1;
        int level = callGetLevel(env, activity);
        return level;
    }

    bool isCharging()
    {
        JNIEnv *env = getEnv();
        if (!env)
            return false;
        jobject activity = getActivity(env);
        if (!activity)
            return false;
        return callIsCharging(env, activity);
    }

    bool isBatterySaver()
    {
        JNIEnv *env = getEnv();
        if (!env)
            return false;
        jobject activity = getActivity(env);
        if (!activity)
            return false;
        return callIsBatterySaver(env, activity);
    }
}
#endif