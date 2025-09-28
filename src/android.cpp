#if defined(__ANDROID__)
#include <Geode/Geode.hpp>
#include <Geode/cocos/platform/android/jni/JniHelper.h>
#include <jni.h>
namespace battery
{
    static JNIEnv *getEnvAttached(bool &shouldDetach)
    {
        shouldDetach = false;
        JNIEnv *env = nullptr;
        auto jvm = cocos2d::JniHelper::getJavaVM();
        if (!jvm)
            return nullptr;
        jint get = jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        if (get == JNI_OK && env)
            return env;
        if (get == JNI_EDETACHED)
        {
            if (jvm->AttachCurrentThread(&env, nullptr) == JNI_OK)
            {
                shouldDetach = true;
                return env;
            }
        }
        return nullptr;
    }

    static void clearIfException(JNIEnv *env)
    {
        if (env && env->ExceptionCheck())
        {
            env->ExceptionClear();
        }
    }

    static jobject getContext(JNIEnv *env)
    {
        cocos2d::JniMethodInfo mi;
        if (cocos2d::JniHelper::getStaticMethodInfo(mi,
                                                    "org/cocos2dx/lib/Cocos2dxActivity",
                                                    "getContext",
                                                    "()Landroid/content/Context;"))
        {
            jobject ctx = env->CallStaticObjectMethod(mi.classID, mi.methodID);
            clearIfException(env);
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
        clearIfException(env);
        jint sdk = sdkField ? env->GetStaticIntField(versionClass, sdkField) : 0;
        env->DeleteLocalRef(versionClass);
        return sdk;
    }

    int getBatteryLevel()
    {
        bool shouldDetach = false;
        JNIEnv *env = getEnvAttached(shouldDetach);
        if (!env)
            return -1;
        jobject context = getContext(env);
        if (!context)
        {
            if (shouldDetach)
                cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
            return -1;
        }

        int result = -1;

        if (getSDKInt(env) >= 21)
        {
            jclass ctxCls = env->GetObjectClass(context);
            jmethodID getSys = ctxCls ? env->GetMethodID(ctxCls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : nullptr;
            clearIfException(env);
            jstring batterySvc = env->NewStringUTF("batterymanager");
            jobject bm = (getSys && batterySvc) ? env->CallObjectMethod(context, getSys, batterySvc) : nullptr;
            clearIfException(env);
            if (bm)
            {
                jclass bmCls = env->GetObjectClass(bm);
                jmethodID getIntProp = bmCls ? env->GetMethodID(bmCls, "getIntProperty", "(I)I") : nullptr;
                clearIfException(env);
                const jint BATTERY_PROPERTY_CAPACITY = 4;
                if (getIntProp)
                {
                    jint lvl = env->CallIntMethod(bm, getIntProp, BATTERY_PROPERTY_CAPACITY);
                    clearIfException(env);
                    if (lvl >= 0 && lvl <= 100)
                        result = static_cast<int>(lvl);
                }
                if (bmCls)
                    env->DeleteLocalRef(bmCls);
                env->DeleteLocalRef(bm);
            }
            if (batterySvc)
                env->DeleteLocalRef(batterySvc);
            if (ctxCls)
                env->DeleteLocalRef(ctxCls);
        }

        if (context)
            env->DeleteLocalRef(context);
        if (shouldDetach)
            cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
        return result;
    }

    bool isCharging()
    {
        bool shouldDetach = false;
        JNIEnv *env = getEnvAttached(shouldDetach);
        if (!env)
            return false;
        jobject context = getContext(env);
        if (!context)
        {
            if (shouldDetach)
                cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
            return false;
        }

        bool result = false;
        jclass ctxCls = env->GetObjectClass(context);
        jmethodID getSys = ctxCls ? env->GetMethodID(ctxCls, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : nullptr;
        clearIfException(env);
        jstring batterySvc = env->NewStringUTF("batterymanager");
        jobject bm = (getSys && batterySvc) ? env->CallObjectMethod(context, getSys, batterySvc) : nullptr;
        clearIfException(env);
        if (bm)
        {
            jclass bmCls = env->GetObjectClass(bm);
            jmethodID isCh = bmCls ? env->GetMethodID(bmCls, "isCharging", "()Z") : nullptr;
            clearIfException(env);
            if (isCh)
            {
                jboolean ch = env->CallBooleanMethod(bm, isCh);
                clearIfException(env);
                result = (ch == JNI_TRUE);
            }
            if (bmCls)
                env->DeleteLocalRef(bmCls);
            env->DeleteLocalRef(bm);
        }
        if (batterySvc)
            env->DeleteLocalRef(batterySvc);
        if (ctxCls)
            env->DeleteLocalRef(ctxCls);

        env->DeleteLocalRef(context);
        if (shouldDetach)
            cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
        return result;
    }

    bool isBatterySaver()
    {
        bool shouldDetach = false;
        JNIEnv *env = getEnvAttached(shouldDetach);
        if (!env)
            return false;
        jobject context = getContext(env);
        if (!context)
        {
            if (shouldDetach)
                cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
            return false;
        }

        jclass ctxClass = env->GetObjectClass(context);
        jmethodID getSys = ctxClass ? env->GetMethodID(ctxClass, "getSystemService", "(Ljava/lang/String;)Ljava/lang/Object;") : nullptr;
        bool result = false;
        if (getSys)
        {
            jstring pwr = env->NewStringUTF("power");
            clearIfException(env);
            jobject pm = (pwr ? env->CallObjectMethod(context, getSys, pwr) : nullptr);
            clearIfException(env);
            if (pm)
            {
                jclass pmClass = env->GetObjectClass(pm);
                jmethodID isPSM = pmClass ? env->GetMethodID(pmClass, "isPowerSaveMode", "()Z") : nullptr;
                clearIfException(env);
                if (isPSM)
                {
                    jboolean on = env->CallBooleanMethod(pm, isPSM);
                    result = (on == JNI_TRUE);
                }
                if (pmClass)
                    env->DeleteLocalRef(pmClass);
                env->DeleteLocalRef(pm);
            }
            if (pwr)
                env->DeleteLocalRef(pwr);
        }
        if (ctxClass)
            env->DeleteLocalRef(ctxClass);
        env->DeleteLocalRef(context);
        if (shouldDetach)
            cocos2d::JniHelper::getJavaVM()->DetachCurrentThread();
        return result;
    }

}

#endif
