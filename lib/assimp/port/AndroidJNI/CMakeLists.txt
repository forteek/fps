cmake_minimum_required(VERSION 2.8)

include_directories(.)
include_directories(../../../../../../Downloads/assimp-5.0-2.1)
add_library(android_jniiosystem STATIC AndroidJNIIOSystem.cpp)
TARGET_LINK_LIBRARIES(android_jniiosystem android log)
