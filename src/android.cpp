#if defined(__ANDROID__)
#include <jni.h>
#include <Geode/Geode.hpp>

// cocos2dx JNI helper
#if __has_include(<Geode/cocos/platform/android/jni/JniHelper.h>)
#include <Geode/cocos/platform/android/jni/JniHelper.h>
#elif __has_include(<platform/android/jni/JniHelper.h>)
#include <platform/android/jni/JniHelper.h>
#elif __has_include(<cocos/platform/android/jni/JniHelper.h>)
#include <cocos/platform/android/jni/JniHelper.h>
#else
#error "JniHelper.h not found. Adjust include path for your cocos2d-x layout."
#endif

using namespace geode::prelude;

namespace
{
    JNIEnv *getEnv()
    {
        return cocos2d::JniHelper::getEnv();
    }

    jobject getContext(JNIEnv *env) {
        if (jclass helperCls = env->FindClass("org/cocos2dx/lib/Cocos2dxHelper")) {
            if (jmethodID mid = env->GetStaticMethodID(helperCls, "getActivity", "()Landroid/app/Activity;")) {
                if (jobject activity = env->CallStaticObjectMethod(helperCls, mid)) {
                    return activity; // use as Context
                }
            }
            env->ExceptionClear();
        }
        if (jclass actCls = env->FindClass("org/cocos2dx/lib/Cocos2dxActivity")) {
            if (jmethodID mid = env->GetStaticMethodID(actCls, "getContext", "()Landroid/content/Context;")) {
                if (jobject ctx = env->CallStaticObjectMethod(actCls, mid)) {
                    return ctx;
                }
            }
            env->ExceptionClear();
        }
        return nullptr;
    }

    int callGetLevel(JNIEnv *env, jobject context)
    {
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            return -1;
        jmethodID mid = env->GetStaticMethodID(cls, "getLevel", "(Landroid/content/Context;)I");
        if (!mid)
            return -1;
        jint result = env->CallStaticIntMethod(cls, mid, context);
        return static_cast<int>(result);
    }

    bool callIsCharging(JNIEnv *env, jobject context)
    {
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            return false;
        jmethodID mid = env->GetStaticMethodID(cls, "isCharging", "(Landroid/content/Context;)Z");
        if (!mid)
            return false;
        jboolean result = env->CallStaticBooleanMethod(cls, mid, context);
        return result == JNI_TRUE;
    }

    bool callIsBatterySaver(JNIEnv *env, jobject context)
    {
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            return false;
        jmethodID mid = env->GetStaticMethodID(cls, "isBatterySaver", "(Landroid/content/Context;)Z");
        if (!mid)
            return false;
        jboolean result = env->CallStaticBooleanMethod(cls, mid, context);
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
        jobject context = getContext(env);
        if (!context)
            return -1;
        int level = callGetLevel(env, context);
        return level;
    }

    bool isCharging()
    {
        JNIEnv *env = getEnv();
        if (!env)
            return false;
        jobject context = getContext(env);
        if (!context)
            return false;
        return callIsCharging(env, context);
    }

    bool isBatterySaver()
    {
        JNIEnv *env = getEnv();
        if (!env)
            return false;
        jobject context = getContext(env);
        if (!context)
            return false;
        return callIsBatterySaver(env, context);
    }
}
#endif