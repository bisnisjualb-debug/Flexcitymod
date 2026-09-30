#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <sys/mman.h>
#include <cstring>
#include <cstdint>
#include <unistd.h>

#define LOG_TAG "FlexCityMod"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

static uintptr_t libil2cpp_base = 0;

static void make_writable(void* addr) {
    uintptr_t page = (uintptr_t)addr & ~(uintptr_t)(sysconf(_SC_PAGESIZE) - 1);
    mprotect((void*)page, sysconf(_SC_PAGESIZE) * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
}

static void patch_return_float(uintptr_t addr, float value) {
    make_writable((void*)addr);
    uint32_t bits;
    memcpy(&bits, &value, 4);
    uint16_t lo = bits & 0xFFFF;
    uint16_t hi = (bits >> 16) & 0xFFFF;
    uint16_t* p = (uint16_t*)addr;
    p[0] = (uint16_t)(0xF240 | ((lo >> 12) & 0xF));
    p[1] = (uint16_t)(0x0000 | (lo & 0xFFF));
    p[2] = (uint16_t)(0xF2C0 | ((hi >> 12) & 0xF));
    p[3] = (uint16_t)(0x0000 | (hi & 0xFFF));
    p[4] = 0x4770;
    __builtin___clear_cache((char*)addr, (char*)addr + 10);
}

static void patch_return_int(uintptr_t addr, int value) {
    make_writable((void*)addr);
    uint16_t lo = value & 0xFFFF;
    uint16_t hi = (value >> 16) & 0xFFFF;
    uint16_t* p = (uint16_t*)addr;
    p[0] = (uint16_t)(0xF240 | ((lo >> 12) & 0xF));
    p[1] = (uint16_t)(0x0000 | (lo & 0xFFF));
    p[2] = (uint16_t)(0xF2C0 | ((hi >> 12) & 0xF));
    p[3] = (uint16_t)(0x0000 | (hi & 0xFFF));
    p[4] = 0x4770;
    __builtin___clear_cache((char*)addr, (char*)addr + 10);
}

JNIEXPORT void JNICALL
Java_com_flexcity_modmenu_NativeLib_init(JNIEnv *env, jclass clazz) {
    void* handle = dlopen("libil2cpp.so", RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        LOGD("dlopen failed: %s", dlerror());
        return;
    }
    libil2cpp_base = (uintptr_t)handle;
    LOGD("base = 0x%lx", (unsigned long)libil2cpp_base);
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}
