cmake_minimum_required(VERSION 3.19)
project(fps)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
file(GLOB SOURCES
        src/*.c
        src/*.cpp
        src/*.h
        src/util/*.h
        src/shader/*.cpp
        src/shader/*.h
        src/camera/*.cpp
        src/camera/*.h
)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)

add_executable(fps ${SOURCES})
target_link_libraries(fps
        glfw
        ${GLFW_LIBRARIES}
)
