cmake_minimum_required(VERSION 3.19)
project(fps)

set(CMAKE_CXX_STANDARD 14)

include_directories(include)
file(GLOB SOURCES
        src/*.c
        src/*.cpp
        src/*.h
        src/shader/*.cpp
        src/shader/*.h
        src/camera/*.cpp
        src/camera/*.h
        src/light/*.cpp
        src/light/*.h
        src/model/*.cpp
        src/model/*.h
)

add_subdirectory(lib/glfw)
include_directories(lib/glfw/include)
add_subdirectory(lib/assimp-3.1.1)
include_directories(lib/assimp-3.1.1/include)

add_executable(fps ${SOURCES})
target_link_libraries(fps
        glfw
        assimp
        ${GLFW_LIBRARIES}
)
