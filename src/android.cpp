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
            log::debug("getEnv() called");
        JNIEnv* env = nullptr;
        auto vm = cocos2d::JniHelper::getJavaVM();
        if (!vm) return nullptr;
        jint res = vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6);
        if (res == JNI_EDETACHED) {
            if (vm->AttachCurrentThread(&env, nullptr) != JNI_OK) {
                    log::debug("Failed to attach current thread to JavaVM");
                return nullptr;
            }
        } else if (res != JNI_OK) {
                log::debug("Failed to get JNIEnv from JavaVM");
            return nullptr;
        }
            log::debug("JNIEnv acquired: {}", reinterpret_cast<uintptr_t>(env));
        return env;
    }

    jobject getContext(JNIEnv *env) {
            log::debug("getContext() called");
        if (jclass helperCls = env->FindClass("org/cocos2dx/lib/Cocos2dxHelper")) {
            if (jmethodID mid = env->GetStaticMethodID(helperCls, "getActivity", "()Landroid/app/Activity;")) {
                if (jobject activity = env->CallStaticObjectMethod(helperCls, mid)) {
                        log::debug("Got Activity context via Cocos2dxHelper.getActivity()");
                    return activity; // use as Context
                }
            }
            env->ExceptionClear();
        }
        if (jclass actCls = env->FindClass("org/cocos2dx/lib/Cocos2dxActivity")) {
            if (jmethodID mid = env->GetStaticMethodID(actCls, "getContext", "()Landroid/content/Context;")) {
                if (jobject ctx = env->CallStaticObjectMethod(actCls, mid)) {
                        log::debug("Got Context via Cocos2dxActivity.getContext()");
                    return ctx;
                }
            }
            env->ExceptionClear();
        }
            log::debug("Failed to acquire Android Context");
        return nullptr;
    }

    int callGetLevel(JNIEnv *env, jobject context)
    {
            log::debug("callGetLevel() called");
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            {
                log::debug("BatteryStatusHelper class not found");
            return -1;
            }
        jmethodID mid = env->GetStaticMethodID(cls, "getLevel", "(Landroid/content/Context;)I");
        if (!mid)
            {
                log::debug("getLevel method not found");
            return -1;
            }
        jint result = env->CallStaticIntMethod(cls, mid, context);
            log::debug("Battery level returned from Java: {}", static_cast<int>(result));
        return static_cast<int>(result);
    }

    bool callIsCharging(JNIEnv *env, jobject context)
    {
            log::debug("callIsCharging() called");
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            {
                log::debug("BatteryStatusHelper class not found");
            return false;
            }
        jmethodID mid = env->GetStaticMethodID(cls, "isCharging", "(Landroid/content/Context;)Z");
        if (!mid)
            {
                log::debug("isCharging method not found");
            return false;
            }
        jboolean result = env->CallStaticBooleanMethod(cls, mid, context);
            log::debug("Charging status returned from Java: {}", result == JNI_TRUE);
        return result == JNI_TRUE;
    }

    bool callIsBatterySaver(JNIEnv *env, jobject context)
    {
            log::debug("callIsBatterySaver() called");
        jclass cls = env->FindClass("com/geode/mods/batterystatus/BatteryStatusHelper");
        if (!cls)
            {
                log::debug("BatteryStatusHelper class not found");
            return false;
            }
        jmethodID mid = env->GetStaticMethodID(cls, "isBatterySaver", "(Landroid/content/Context;)Z");
        if (!mid)
            {
                log::debug("isBatterySaver method not found");
            return false;
            }
        jboolean result = env->CallStaticBooleanMethod(cls, mid, context);
            log::debug("Battery saver status returned from Java: {}", result == JNI_TRUE);
        return result == JNI_TRUE;
    }
}

namespace battery
{
    int getBatteryLevel()
    {
            log::debug("battery::getBatteryLevel() called");
        JNIEnv *env = getEnv();
        if (!env)
            {
                log::debug("JNIEnv is null");
            return -1;
            }
        jobject context = getContext(env);
        if (!context)
            {
                log::debug("Context is null");
            return -1;
            }
        int level = callGetLevel(env, context);
            log::debug("battery::getBatteryLevel() returning {}", level);
        return level;
    }

    bool isCharging()
    {
            log::debug("battery::isCharging() called");
        JNIEnv *env = getEnv();
        if (!env)
            {
                log::debug("JNIEnv is null");
            return false;
            }
        jobject context = getContext(env);
        if (!context)
            {
                log::debug("Context is null");
            return false;
            }
        return callIsCharging(env, context);
    }

    bool isBatterySaver()
    {
            log::debug("battery::isBatterySaver() called");
        JNIEnv *env = getEnv();
        if (!env)
            {
                log::debug("JNIEnv is null");
            return false;
            }
        jobject context = getContext(env);
        if (!context)
            {
                log::debug("Context is null");
            return false;
            }
        return callIsBatterySaver(env, context);
    }
}
#endif