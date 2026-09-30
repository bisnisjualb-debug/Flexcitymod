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

// ARM Thumb: MOVW/MOVT return value + BX LR
// return 999999.0f = 0x497423F0
static void patch_return_float(uintptr_t addr, float value) {
    make_writable((void*)addr);
    uint32_t bits;
    memcpy(&bits, &value, 4);
    uint16_t lo = bits & 0xFFFF;
    uint16_t hi = (bits >> 16) & 0xFFFF;
    uint16_t movw[] = { (uint16_t)(0xF240 | ((lo >> 12) & 0xF)), (uint16_t)(0x0000 | (lo & 0xFFF)) };
    uint16_t movt[] = { (uint16_t)(0xF2C0 | ((hi >> 12) & 0xF)), (uint16_t)(0x0000 | (hi & 0xFFF)) };
    uint16_t bx_lr = 0x4770;
    uint16_t* p = (uint16_t*)addr;
    p[0] = movw[0]; p[1] = movw[1];
    p[2] = movt[0]; p[3] = movt[1];
    p[4] = bx_lr;
    __builtin___clear_cache((char*)addr, (char*)addr + 10);
}

static void patch_return_int(uintptr_t addr, int value) {
    make_writable((void*)addr);
    uint16_t lo = value & 0xFFFF;
    uint16_t hi = (value >> 16) & 0xFFFF;
    uint16_t movw[] = { (uint16_t)(0xF240 | ((lo >> 12) & 0xF)), (uint16_t)(0x0000 | (lo & 0xFFF)) };
    uint16_t movt[] = { (uint16_t)(0xF2C0 | ((hi >> 12) & 0xF)), (uint16_t)(0x0000 | (hi & 0xFFF)) };
    uint16_t bx_lr = 0x4770;
    uint16_t* p = (uint16_t*)addr;
    p[0] = movw[0]; p[1] = movw[1];
    p[2] = movt[0]; p[3] = movt[1];
    p[4] = bx_lr;
    __builtin___clear_cache((char*)addr, (char*)addr + 10);
}

JNIEXPORT void JNICALL
Java_com_flexcity_modmenu_NativeLib_init(JNIEnv *env, jclass clazz) {
    void* handle = dlopen("libil2cpp.so", RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        LOGD("dlopen libil2cpp.so failed: %s", dlerror());
        return;
    }
    libil2cpp_base = (uintptr_t)handle;
    LOGD("libil2cpp base = 0x%lx", (unsigned long)libil2cpp_base);

    // Ganti OFFSET sesuai RVA Il2CppDumper (dikurangi base image 0x0)
    // Contoh:
    // patch_return_float(libil2cpp_base + 0x1A2B3C4, 999999.0f); // get_Health
    // patch_return_int(libil2cpp_base + 0x1A2B3C5, 999999999);   // get_Money
}

JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}
