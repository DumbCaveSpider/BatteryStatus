#if defined(__ANDROID__)
#include <Geode/Geode.hpp>
#include <Geode/cocos/platform/android/jni/JniHelper.h>
#include <jni.h>
namespace battery
{

    static jobject getContext(JNIEnv *env)
    {
        cocos2d::JniMethodInfo mi;
        if (cocos2d::JniHelper::getStaticMethodInfo(mi,
                                                    "org/cocos2dx/lib/Cocos2dxHelper",
                                                    "getActivity",
                                                    "()Landroid/app/Activity;"))
        {
            jobject activity = env->CallStaticObjectMethod(mi.classID, mi.methodID);
            env->DeleteLocalRef(mi.classID);
            return activity;
        }
        if (cocos2d::JniHelper::getStaticMethodInfo(mi,
                                                    "org/cocos2dx/lib/Cocos2dxActivity",
                                                    "getContext",
                                                    "()Landroid/content/Context;"))
        {
            jobject ctx = env->CallStaticObjectMethod(mi.classID, mi.methodID);
            env->DeleteLocalRef(mi.classID);
            return ctx;
        }
        return nullptr;
    }

    static jint getSDKInt(JNIEnv *env)
    {
        jclass versionClass = env->FindClass("android/os/Build$VERSION");
        if (!versionClass)
            return 0;
        jfieldID sdkField = env->GetStaticFieldID(versionClass, "SDK_INT", "I");
        jint sdk = sdkField ? env->GetStaticIntField(versionClass, sdkField) : 0;
        env->DeleteLocalRef(versionClass);
        return sdk;
    }

    int getBatteryLevel()
    {
        JNIEnv *env = nullptr;
        if (cocos2d::JniHelper::getJavaVM()->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK || !env)
            return -1;
        jobject context = getContext(env);
        if (!context)
            return -1;

        // API 34+: android.hardware.BatteryState
        if (getSDKInt(env) >= 34)
        {
            jclass bsClass = env->FindClass("android/hardware/BatteryState");
            if (bsClass)
            {
                jmethodID createMid = env->GetStaticMethodID(bsClass, "create", "(Landroid/content/Context;)Landroid/hardware/BatteryState;");
                if (createMid)
                {
                    jobject state = env->CallStaticObjectMethod(bsClass, createMid, context);
                    if (state)
                    {
                        jmethodID pctMid = env->GetMethodID(bsClass, "getBatteryPercent", "()F");
                        if (pctMid)
                        {
                            jfloat pct = env->CallFloatMethod(state, pctMid);
                            env->DeleteLocalRef(state);
                            env->DeleteLocalRef(bsClass);
                            env->DeleteLocalRef(context);
                            return (int)(pct * 100.0f);
                        }
                        env->DeleteLocalRef(state);
                    }
                }
                env->DeleteLocalRef(bsClass);
            }
        }

        // Fallback: ACTION_BATTERY_CHANGED intent
        jclass ifClass = env->FindClass("android/content/IntentFilter");
        jmethodID ifCtor = ifClass ? env->GetMethodID(ifClass, "<init>", "(Ljava/lang/String;)V") : nullptr;
        jstring action = env->NewStringUTF("android.intent.action.BATTERY_CHANGED");
        jobject filter = (ifClass && ifCtor && action) ? env->NewObject(ifClass, ifCtor, action) : nullptr;
        jclass ctxClass = env->GetObjectClass(context);
        jmethodID regMid = ctxClass ? env->GetMethodID(ctxClass, "registerReceiver", "(Landroid/content/BroadcastReceiver;Landroid/content/IntentFilter;)Landroid/content/Intent;") : nullptr;
        jobject intent = (regMid && filter) ? env->CallObjectMethod(context, regMid, nullptr, filter) : nullptr;

        int result = -1;
        if (intent)
        {
            jclass intentClass = env->GetObjectClass(intent);
            jmethodID getIntExtra = intentClass ? env->GetMethodID(intentClass, "getIntExtra", "(Ljava/lang/String;I)I") : nullptr;
            if (getIntExtra)
            {
                jstring levelStr = env->NewStringUTF("level");
                jstring scaleStr = env->NewStringUTF("scale");
                jint level = env->CallIntMethod(intent, getIntExtra, levelStr, -1);
                jint scale = env->CallIntMethod(intent, getIntExtra, scaleStr, -1);
                if (level >= 0 && scale > 0)
                {
                    result = (int)((level / (float)scale) * 100.0f);
                }
                env->DeleteLocalRef(levelStr);
                env->DeleteLocalRef(scaleStr);
            }
            if (intentClass)
                env->DeleteLocalRef(intentClass);
        }

        // Cleanup
        if (intent)
            env->DeleteLocalRef(intent);
        if (filter)
            env->DeleteLocalRef(filter);
        if (ifClass)
            env->DeleteLocalRef(ifClass);
        if (ctxClass)
            env->DeleteLocalRef(ctxClass);
        env->DeleteLocalRef(action);
        env->DeleteLocalRef(context);
        return result;
    }

    bool isCharging()
    {
        JNIEnv *env = nullptr;
        if (cocos2d::JniHelper::getJavaVM()->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK || !env)
            return false;
        jobject context = getContext(env);
        if (!context)
            return false;

        // API 34 BatteryState
        if (getSDKInt(env) >= 34)
        {
            jclass bsClass = env->FindClass("android/hardware/BatteryState");
            if (bsClass)
            {
                jmethodID createMid = env->GetStaticMethodID(bsClass, "create", "(Landroid/content/Context;)Landroid/hardware/BatteryState;");
                if (createMid)
                {
                    jobject state = env->CallStaticObjectMethod(bsClass, createMid, context);
                    if (state)
                    {
                        jmethodID chMid = env->GetMethodID(bsClass, "isCharging", "()Z");
                        if (chMid)
                        {
                            jboolean ch = env->CallBooleanMethod(state, chMid);
                            env->DeleteLocalRef(state);
                            env->DeleteLocalRef(bsClass);
                            env->DeleteLocalRef(context);
                            return ch == JNI_TRUE;
                        }
                        env->DeleteLocalRef(state);
                    }
                }
                env->DeleteLocalRef(bsClass);
            }
        }

        // Fallback: ACTION_BATTERY_CHANGED
        jclass ifClass = env->FindClass("android/content/IntentFilter");
        jmethodID ifCtor = ifClass ? env->GetMethodID(ifClass, "<init>", "(Ljava/lang/String;)V") : nullptr;
        jstring action = env->NewStringUTF("android.intent.action.BATTERY_CHANGED");
        jobject filter = (ifClass && ifCtor && action) ? env->NewObject(ifClass, ifCtor, action) : nullptr;
        jclass ctxClass = env->GetObjectClass(context);
        jmethodID regMid = ctxClass ? env->GetMethodID(ctxClass, "registerReceiver", "(Landroid/content/BroadcastReceiver;Landroid/content/IntentFilter;)Landroid/content/Intent;") : nullptr;
        jobject intent = (regMid && filter) ? env->CallObjectMethod(context, regMid, nullptr, filter) : nullptr;
        bool result = false;
        if (intent)
        {
            jclass intentClass = env->GetObjectClass(intent);
            jmethodID getIntExtra = intentClass ? env->GetMethodID(intentClass, "getIntExtra", "(Ljava/lang/String;I)I") : nullptr;
            if (getIntExtra)
            {
                jstring statusStr = env->NewStringUTF("status");
                jint status = env->CallIntMethod(intent, getIntExtra, statusStr, -1);
                // 2=BATTERY_STATUS_CHARGING, 5=BATTERY_STATUS_FULL
                result = (status == 2 || status == 5);
                env->DeleteLocalRef(statusStr);
            }
            if (intentClass)
                env->DeleteLocalRef(intentClass);
        }
        if (intent)
            env->DeleteLocalRef(intent);
        if (filter)
            env->DeleteLocalRef(filter);
        if (ifClass)
            env->DeleteLocalRef(ifClass);
        if (ctxClass)
            env->DeleteLocalRef(ctxClass);
        env->DeleteLocalRef(action);
        env->DeleteLocalRef(context);
        return result;
    }

    bool isBatterySaver()
    {
        JNIEnv *env = nullptr;
        if (cocos2d::JniHelper::getJavaVM()->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK || !env)
            return false;
        jobject context = getContext(env);
        if (!context)
            return false;

        if (getSDKInt(env) >= 34)
        {
            jclass bsClass = env->FindClass("android/hardware/BatteryState");
            if (bsClass)
            {
                jmethodID createMid = env->GetStaticMethodID(bsClass, "create", "(Landroid/content/Context;)Landroid/hardware/BatteryState;");
                if (createMid)
                {
                    jobject state = env->CallStaticObjectMethod(bsClass, createMid, context);
                    if (state)
                    {
                        jmethodID saverMid = env->GetMethodID(bsClass, "isBatterySaverOn", "()Z");
                        if (saverMid)
                        {
                            jboolean on = env->CallBooleanMethod(state, saverMid);
                            env->DeleteLocalRef(state);
                            env->DeleteLocalRef(bsClass);
                            env->DeleteLocalRef(context);
                            return on == JNI_TRUE;
                        }
                        env->DeleteLocalRef(state);
                    }
                }
                env->DeleteLocalRef(bsClass);
            }
        }

        // Fallback: PowerManager.isPowerSaveMode()
        jclass ctxClass = env->GetObjectClass(context);
        jmethodID getSys = ctxClass ? env->GetMethodID(ctxClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : nullptr;
        bool result = false;
        if (getSys)
        {
            jstring pwr = env->NewStringUTF("power"); // Context.POWER_SERVICE
            jobject pm = env->CallObjectMethod(context, getSys, pwr);
            if (pm)
            {
                jclass pmClass = env->GetObjectClass(pm);
                jmethodID isPSM = pmClass ? env->GetMethodID(pmClass, "isPowerSaveMode", "()Z") : nullptr;
                if (isPSM)
                {
                    jboolean on = env->CallBooleanMethod(pm, isPSM);
                    result = (on == JNI_TRUE);
                }
                if (pmClass)
                    env->DeleteLocalRef(pmClass);
                env->DeleteLocalRef(pm);
            }
            env->DeleteLocalRef(pwr);
        }
        if (ctxClass)
            env->DeleteLocalRef(ctxClass);
        env->DeleteLocalRef(context);
        return result;
    }

}

#endif
