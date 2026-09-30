LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := flexcitymod
LOCAL_SRC_FILES := native-lib.cpp
LOCAL_LDLIBS := -llog -ldl
LOCAL_CFLAGS := -std=c++17 -fPIC
include $(BUILD_SHARED_LIBRARY)
