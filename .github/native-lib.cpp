#include <jni.h>
#include <android/log.h>
#include <shadowhook.h>

#define LOG_TAG "FlexCityMod"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static void* orig_GetHealth = nullptr;
static float hook_GetHealth(void* instance) {
    return 999999.0f;
}

static void* orig_GetDamage = nullptr;
static int hook_GetDamage(void* instance) {
    int base = ((int(*)(void*))orig_GetDamage)(instance);
    return base * 1000;
}

JNIEXPORT void JNICALL
Java_com_flexcity_modmenu_NativeLib_init(JNIEnv *env, jclass clazz) {
    shadowhook_init(SHADOWHOOK_MODE_UNIQUE, false);
    shadowhook_hook_func_addr((void*)0xOFFSET_HEALTH, (void*)hook_GetHealth, &orig_GetHealth);
    shadowhook_hook_func_addr((void*)0xOFFSET_DAMAGE, (void*)hook_GetDamage, &orig_GetDamage);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}
